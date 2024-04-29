#include "pch.h"
#include "CStandNumberPlugin.h"
#include <string>
#include "version.h"
#include <winnt.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define STN_PLUGIN_NAME "STN Plugin"
#define DISPLAY_WARNING(str) DisplayUserMessage(STN_PLUGIN_NAME, "Warning", str, true, true, true, true, false);
#define DISPLAY_INFO(str) DisplayUserMessage(STN_PLUGIN_NAME, "Info", str, true, true, true, true, false);
#define DISPLAY_DEBUG(str) DisplayUserMessage(STN_PLUGIN_NAME, "Debug", str, true, true, true, true, false);

vector<AirlineStands> AvailableStands;		// vector to store airline assigned stands loaded from json
vector<GatesAndStands> LHBPGatesAndStands;	// vector to store stands and gates loaded from json
vector<string> SchengenCountries;			// vector to store Schengen countries loaded from json
vector<Aircraft> Aircrafts;					// vector to store aircraft types loaded from json
map<string, int> CallsignGateMap;			// map to store callsign - gate assignment

int STN_AssignRange = 25;					// stand assignment range loaded from json
int STN_DeleteAlt = 700;					// stand assignment/deletion altitude loaded from json
int STN_OccupiedMAxSpeed = 5;				// stand occupation maximum speed in kts loaded from json
int STN_AcMaxDistanceToGate = 55;			// stand occupation max distance from the gate loaded from json

bool DebugPrint = false;					// enable debug printf flag

inline static bool startsWith(const char* pre, const char* str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
};

CStandNumberPlugin::CStandNumberPlugin()
	: CPlugIn(COMPATIBILITY_CODE,
		PLUGIN_NAME,
		VERSION_FILE_STR,
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT)
{
	char fullPluginPath[_MAX_PATH];
	ConnectionStatus = 0;

	string ver = VERSION_FILE_STR;
	string versioninfo{ "Version: "+ver+" loaded" };

	DISPLAY_INFO(versioninfo.c_str());

	GetModuleFileNameA((HINSTANCE)&__ImageBase, fullPluginPath, sizeof(fullPluginPath));
	std::string fullPluginPathStr(fullPluginPath);
	pluginDirectory = fullPluginPathStr.substr(0, fullPluginPathStr.find_last_of("\\"));
	LoadStandConfig("StandNumberPlugin-config.json");
	LoadAircraftConfig("ICAO_Aircraft.json");
}

CStandNumberPlugin::~CStandNumberPlugin()
{
}

bool CStandNumberPlugin::OnCompileCommand(const char* sCommandLine)
{
	string commandString(sCommandLine);
	cmatch matches;

	if (startsWith(".stnver", sCommandLine))
	{
		string ver = VERSION_FILE_STR;
		string versioninfo{ "Version: " + ver + " loaded" };

		DISPLAY_INFO(versioninfo.c_str());
		return NULL;
	}

	if (startsWith(".stnstat", sCommandLine))
	{
		DISPLAY_DEBUG(GetGateStatus().c_str());
		return NULL;
	}

	if (startsWith(".stndeb", sCommandLine))
	{
		DebugPrint = !DebugPrint;
		return NULL;
	}

	return false;
}

void CStandNumberPlugin::OnFlightPlanDisconnect(CFlightPlan FlightPlan)
{
	string Callsign = FlightPlan.GetCallsign();
	if (CallsignGateMap.find(Callsign) != CallsignGateMap.end())
	{
		map<string, int>::iterator CallsignGatePair = CallsignGateMap.find(Callsign);
		GatesAndStands GateToEdit = LHBPGatesAndStands.at(CallsignGatePair->second);
		if (GateToEdit.Occupied)
		{
			GateToEdit.Occupied = false;
			GateToEdit.Callsign = "";
			CallsignGateMap.erase(Callsign);
		}
	}
}

void CStandNumberPlugin::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
	string CallSign = RadarTarget.GetCallsign();
	CFlightPlan FLightPlan = FlightPlanSelect(RadarTarget.GetCallsign());
	string GateName;
	bool Success = false;


	if (IsRelevantFLightplan(FLightPlan))
	{
		if (IsDepartingAircraftWithStand(RadarTarget, FLightPlan))
		{
			FLightPlan.GetControllerAssignedData().SetScratchPadString("");
			FreeOccupiedGate(CallSign);
		}

		if (IsDepartingAircraftWithoutStand(RadarTarget, FLightPlan))
		{
			ReserveOccupiedGate(CallSign, RadarTarget, FLightPlan);
		}

		if (IsArrivingAircraftWithoutStand(RadarTarget, FLightPlan))
		{
			GateName = GetStand(IsFromSchengen(FLightPlan.GetFlightPlanData().GetOrigin()), CallSign, GetWingspan(FLightPlan));
			Success = FLightPlan.GetControllerAssignedData().SetScratchPadString(GateName.c_str());

			if (Success)
			{
				PlanGate(CallSign, GateName);
			}
		}

		if (!IsGateStillOccupiedByCallsign(CallSign, FLightPlan))
		{
			FreeOccupiedGate(CallSign);
		}
	}
}

void CStandNumberPlugin::OnTimer(int Counter)
{

}

void CStandNumberPlugin::LoadStandConfig(const std::string& filename) {
	std::ifstream file(pluginDirectory + "\\" + filename);
	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (fileContent.empty()) {
		DISPLAY_WARNING((filename + ": the JSON-file was not found or is empty").c_str());
		return;
	}

	using namespace rapidjson;
	Document document;
	document.Parse(fileContent.c_str());

	if (document.HasParseError()) {
		ParseErrorCode code = document.GetParseError();
		size_t offset = document.GetErrorOffset();
		std::string message = filename + ": error while parsing JSON at position " + std::to_string(offset) + ": '" + fileContent.substr(offset, 10) + "'";
		DISPLAY_WARNING(message.c_str());
		return;
	}

	assert(document.HasMember("stand_assignment_range"));
	const Value& range = document["stand_assignment_range"]; // Using a reference for consecutive access is handy and faster.
	assert(range.IsInt());
	STN_AssignRange = range.GetInt();

	assert(document.HasMember("stand_deletion_altitude"));
	const Value& alt = document["stand_deletion_altitude"]; // Using a reference for consecutive access is handy and faster.
	assert(alt.IsInt());
	STN_DeleteAlt = alt.GetInt();

	assert(document.HasMember("stand_ooccupied_max_speed"));
	const Value& speed = document["stand_ooccupied_max_speed"]; // Using a reference for consecutive access is handy and faster.
	assert(speed.IsInt());
	STN_OccupiedMAxSpeed = speed.GetInt();

	assert(document.HasMember("aircraft_max_distance_to_gate"));
	const Value& maxdist = document["aircraft_max_distance_to_gate"]; // Using a reference for consecutive access is handy and faster.
	assert(maxdist.IsInt());
	STN_AcMaxDistanceToGate = maxdist.GetInt();

	assert(document.HasMember("airlines"));
	const Value& airlines = document["airlines"]; // Using a reference for consecutive access is handy and faster.
	assert(airlines.IsArray());

	for (const auto& airline : document.GetObject()["airlines"].GetArray())
	{
		AirlineStands element;
		assert(airline["airlinecode"].IsString());
		element.AirlineCode = airline["airlinecode"].GetString();

		for (auto& stand : airline["stands"].GetArray())
		{
			assert(stand.IsString());
			element.PreferedStands.push_back(stand.GetString());
		}
		AvailableStands.push_back(element);
	}

	assert(document.HasMember("gates"));
	const Value& gates = document["gates"]; // Using a reference for consecutive access is handy and faster.
	assert(gates.IsArray());

	for (const auto& gate : document.GetObject()["gates"].GetArray())
	{
		GatesAndStands element;

		assert(gate["number"].IsString());
		assert(gate["loncoord"].IsString());
		assert(gate["latcoord"].IsString());
		assert(gate["wingspan"].IsNumber());
		assert(gate["wingspan"].IsDouble());
		assert(gate["schengen"].IsBool());

		element.Number = gate["number"].GetString();
		element.LongCoord = gate["loncoord"].GetString();
		element.LAtCoord = gate["latcoord"].GetString();
		element.Span = gate["wingspan"].GetDouble();
		element.Occupied = false;
		element.Planned = false;
		element.PlannedCallsign = "";
		element.Schengen = gate["schengen"].GetBool();
		element.Callsign = "";

		LHBPGatesAndStands.push_back(element);
	}

	assert(document.HasMember("schengencountries"));
	const Value& countries = document["schengencountries"]; // Using a reference for consecutive access is handy and faster.
	assert(countries.IsArray());

	for (const auto& countryicao : document.GetObject()["schengencountries"].GetArray())
	{
		assert(countryicao.IsString());

		SchengenCountries.push_back(countryicao.GetString());
	}
}

void CStandNumberPlugin::LoadAircraftConfig(const std::string& filename) {
	std::ifstream file(pluginDirectory + "\\" + filename);
	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (fileContent.empty()) {
		DISPLAY_WARNING((filename + ": the JSON-file was not found or is empty").c_str());
		return;
	}

	using namespace rapidjson;
	Document document;
	document.Parse(fileContent.c_str());

	if (document.HasParseError()) {
		ParseErrorCode code = document.GetParseError();
		size_t offset = document.GetErrorOffset();
		std::string message = filename + ": error while parsing JSON at position " + std::to_string(offset) + ": '" + fileContent.substr(offset, 10) + "'";
		DISPLAY_WARNING(message.c_str());
		return;
	}

	for (const auto& aircraft : document.GetArray())
	{
		Aircraft ac;

		if (aircraft.HasMember("Wingspan"))
		{
			assert(aircraft["ICAO"].IsString());
			ac.ICAO = aircraft["ICAO"].GetString();
			assert(aircraft["Wingspan"].IsString());
			ac.Wingspan = stod(aircraft["Wingspan"].GetString());

			Aircrafts.push_back(ac);
		}
	}
}


string CStandNumberPlugin::GetClosestStand(CPosition ACPos_f)
{
	struct {
		double Dist = 9999.0;
		string Number;
	} Closest;

	int index = 0;

	for (auto& gate : LHBPGatesAndStands)
	{
		CPosition standpos;
		bool success = standpos.LoadFromStrings(gate.LongCoord.c_str(), gate.LAtCoord.c_str());
		if (success)
		{
			double dist = ACPos_f.DistanceTo(standpos);
			if (dist < 10.0)
			{
				if (dist < Closest.Dist)
				{
					Closest.Dist = dist;
					Closest.Number = gate.Number;
				}
			}
		}
		index++;
	}
	
	if (DebugPrint)
	{
		string DisplayMsg = { "Closest distance " + to_string(Closest.Dist) };
		DISPLAY_DEBUG(DisplayMsg.c_str());
	}

	return Closest.Number;
}

bool CStandNumberPlugin::IsFromSchengen(string DepAirportICAO)
{
	for (auto& ICAO : SchengenCountries)
	{
		if (0 == DepAirportICAO.compare(0, 2, ICAO.substr(0, 2)))
		{
			return true;
		}
	}

	return false;
}

string CStandNumberPlugin::GetStand(bool IsFromSchengen, string Callsign, double WingSpan)
{
	string GateNum = " NO";
	string AirlineCode = Callsign.substr(0, 3);
	int MaxRetries = 10;
	
	for (auto& gates : AvailableStands)
	{
		if (strcmp(gates.AirlineCode.c_str(), AirlineCode.c_str()) == 0)
		{
			while (MaxRetries)
			{
				int idx = rand() % gates.PreferedStands.size();
				GateNum = gates.PreferedStands.at(idx);
				for (auto& gate : LHBPGatesAndStands)
				{
					if (strcmp(gate.Number.c_str(), GateNum.c_str()) == 0)
					{
						if (!gate.Planned && !gate.Occupied && (WingSpan <= gate.Span))
						{
							return GateNum;
						}
					}
				}
				string GateNum = " NO";
				MaxRetries--;
			}
		}
	}
	
	return " NO";
}

string CStandNumberPlugin::GetGateStatus(void)
{
	string status = "";
	for (auto& gate : LHBPGatesAndStands)
	{
		if (gate.Occupied || gate.Planned)
		{
			status += gate.Number + " ";
			status += (gate.Occupied) ? "X":"-";
			status += (gate.Planned) ? "X" : "-";
			if (gate.Occupied)
				status += " " + gate.Callsign;
			if (gate.Planned)
				status += " " + gate.PlannedCallsign;
			status += ", ";
		}
	}
	return status;
}


bool CStandNumberPlugin::IsRelevantFLightplan(CFlightPlan FP_f)
{
	string CtrlId = FP_f.GetTrackingControllerId();

	if (FP_f.IsValid() &&
		!FP_f.GetSimulated() &&
		(FP_f.GetTrackingControllerIsMe() || CtrlId.empty()))
	{
		return true;
	}

	return false;
}

bool CStandNumberPlugin::IsDepartingAircraftWithStand(CRadarTarget RT_f, CFlightPlan FP_f)
{
	if ((RT_f.GetPosition().GetPressureAltitude() >= STN_DeleteAlt) &&
		(strcmp("LHBP", FP_f.GetFlightPlanData().GetDestination()) != 0) &&
		(strcmp("LHBP", FP_f.GetFlightPlanData().GetOrigin()) == 0) &&
		(strlen(FP_f.GetControllerAssignedData().GetScratchPadString()) != 0))
	{
		return true;
	}

	return false;
}

bool CStandNumberPlugin::IsDepartingAircraftWithoutStand(CRadarTarget RT_f, CFlightPlan FP_f)
{
	if ((RT_f.GetPosition().GetPressureAltitude() < STN_DeleteAlt) &&
		(strcmp("LHBP", FP_f.GetFlightPlanData().GetOrigin()) == 0) &&
		(RT_f.GetPosition().GetReportedGS() < STN_OccupiedMAxSpeed) &&
		(strlen(FP_f.GetControllerAssignedData().GetScratchPadString()) == 0))
	{
		return true;
	}

	return false;
}

bool CStandNumberPlugin::IsArrivingAircraftWithoutStand(CRadarTarget RT_f, CFlightPlan FP_f)
{
	if ((strcmp("LHBP", FP_f.GetFlightPlanData().GetDestination()) == 0) &&
		(FP_f.GetDistanceToDestination() <= STN_AssignRange) &&
		(RT_f.GetPosition().GetPressureAltitude() >= STN_DeleteAlt) &&
		(strlen(FP_f.GetControllerAssignedData().GetScratchPadString()) == 0))
	{
		return true;
	}

	return false;
}

bool CStandNumberPlugin::GetGateByCallsign(string CallSign_f, int &GateIdx_f)
{
	if (CallsignGateMap.find(CallSign_f) != CallsignGateMap.end())
	{
		map<string, int>::iterator CallsignGatePair = CallsignGateMap.find(CallSign_f);
		GateIdx_f = CallsignGatePair->second;
		return true;
	}

	return false;
}

bool CStandNumberPlugin::GetGateByNumber(string GateNumber_f, int &GateIdx_f)
{
	GatesAndStands gate;
	for (unsigned int idx = 0; idx < LHBPGatesAndStands.size(); idx++)
	{
		gate = LHBPGatesAndStands.at(idx);

		if (strcmp(gate.Number.c_str(), GateNumber_f.c_str()) == 0)
		{
			GateIdx_f = idx;
			return true;
		}
	}

	return false;
}

void CStandNumberPlugin::FreeOccupiedGate(string Callsign_f)
{
	int GateIdx = 0;
	if (GetGateByCallsign(Callsign_f, GateIdx))
	{
		if (LHBPGatesAndStands.at(GateIdx).Occupied)
		{
			LHBPGatesAndStands.at(GateIdx).Occupied = false;
			LHBPGatesAndStands.at(GateIdx).Callsign = "";
			CallsignGateMap.erase(Callsign_f);
		}
	}
}

void CStandNumberPlugin::ReserveOccupiedGate(string Callsign_f, CRadarTarget RT_f, CFlightPlan FP_f)
{
	int GateIdx = 0;

	string ClosestGateStandName = CStandNumberPlugin::GetClosestStand(RT_f.GetPosition().GetPosition());

	if (GetGateByNumber(ClosestGateStandName, GateIdx))
	{
		LHBPGatesAndStands.at(GateIdx).Occupied = true;;
		LHBPGatesAndStands.at(GateIdx).Callsign = Callsign_f;
		CallsignGateMap.insert(pair<string, int>(Callsign_f, GateIdx));
		FP_f.GetControllerAssignedData().SetScratchPadString(LHBPGatesAndStands.at(GateIdx).Number.c_str());

		if (LHBPGatesAndStands.at(GateIdx).Planned)
		{
			CFlightPlan PlannedforFP = FlightPlanSelect(LHBPGatesAndStands.at(GateIdx).PlannedCallsign.c_str());

			if (IsRelevantFLightplan(PlannedforFP))
			{
				PlannedforFP.GetControllerAssignedData().SetScratchPadString("");
				CallsignGateMap.erase(LHBPGatesAndStands.at(GateIdx).PlannedCallsign);
			}

			LHBPGatesAndStands.at(GateIdx).Planned = false;
			LHBPGatesAndStands.at(GateIdx).PlannedCallsign = "";
		}
	}
}

void CStandNumberPlugin::PlanGate(string Callsign_f, string GateName_f)
{
	bool Success = false;
	GatesAndStands GateToEdit;
	int GateIdx = 0;

	Success = GetGateByNumber(GateName_f, GateIdx);
	if (Success)
	{
		LHBPGatesAndStands.at(GateIdx).Planned = true;
		LHBPGatesAndStands.at(GateIdx).PlannedCallsign = Callsign_f;
		CallsignGateMap.insert(pair<string, int>(Callsign_f, GateIdx));
	}
}

bool CStandNumberPlugin::IsGateStillOccupiedByCallsign(string Callsign_f, CFlightPlan FP_f)
{
	/* convert meter to nm */
	double MaxDist_nm = (STN_AcMaxDistanceToGate * 0.539956803) / 1000.0;
	double DistAcGate;

	if (CallsignGateMap.find(Callsign_f) != CallsignGateMap.end())
	{
		map<string, int>::iterator CallsignGatePair = CallsignGateMap.find(Callsign_f);
		GatesAndStands GateToEdit = LHBPGatesAndStands.at(CallsignGatePair->second);

		if (GateToEdit.Occupied)
		{
			CPosition standpos;
			standpos.LoadFromStrings(GateToEdit.LongCoord.c_str(), GateToEdit.LAtCoord.c_str());

			CPosition acpos = RadarTargetSelect(FP_f.GetCallsign()).GetPosition().GetPosition();

			DistAcGate = acpos.DistanceTo(standpos);

			if (DistAcGate <= MaxDist_nm)
			{
				return true;
			}
		}
	}

	return false;;
}

double CStandNumberPlugin::GetWingspan(CFlightPlan FP_f)
{
	double WingSpan = 999.0;
	string AircraftType = FP_f.GetFlightPlanData().GetAircraftFPType();

	for (auto& ac : Aircrafts)
	{
		if (strcmp(ac.ICAO.c_str(), AircraftType.c_str()) == 0)
		{
			WingSpan = ac.Wingspan;
		}
	}

	return WingSpan;
}