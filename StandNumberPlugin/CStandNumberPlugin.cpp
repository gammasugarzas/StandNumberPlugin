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
map<string, GatesAndStands> CallignGateMap;	// map to store callsign - gate assignment

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

void CStandNumberPlugin::OnRadarTargetDisconnect(CRadarTarget RadarTarget)
{
	string Callsign = RadarTarget.GetCallsign();
	if (CallignGateMap.find(Callsign) != CallignGateMap.end())
	{
		map<string, GatesAndStands>::iterator CallsignGatePair = CallignGateMap.find(Callsign);
		if (CallsignGatePair->second.Occupied)
		{
			CallsignGatePair->second.Occupied = false;
			CallsignGatePair->second.Callsign = "";
			CallignGateMap.erase(Callsign);

			if (DebugPrint)
			{
				string DisplayMsg{ "Gate " + CallsignGatePair->second.Number + " is set to free" };
				DISPLAY_DEBUG(DisplayMsg.c_str());
			}
		}
	}
}

void CStandNumberPlugin::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
	string Callsign = RadarTarget.GetCallsign();
	CFlightPlan FP = FlightPlanSelect(RadarTarget.GetCallsign());
	string Gate;
	double WingSpan;
	string AircraftType;

	string id = FP.GetTrackingControllerId();
	// Flightplan is valid, not simulated and tracked by me or not tracked by anybody else
	if (FP.IsValid() &&
		!FP.GetSimulated() &&
		(FP.GetTrackingControllerIsMe() || id.empty()))
	{
		/* if LHBP is the departure airport, but LHBP is not the dest airport, and the altitude is higher than STN_NUM_DELETION_ALT feet delete the schratchpad contents*/
		if ((RadarTarget.GetPosition().GetPressureAltitude() >= STN_DeleteAlt) &&
			(strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) != 0) &&
			(strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0))
		{
			if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) != 0)
			{
				if (DebugPrint)
				{
					string DisplayMsg{ Callsign + " gate number " + string {FP.GetControllerAssignedData().GetScratchPadString()} + " was deleted" };
					DISPLAY_DEBUG(DisplayMsg.c_str());
				}
				FP.GetControllerAssignedData().SetScratchPadString("");
			}
		}

		/* if LHBP is the departure airport and the altitude is lower than 700 feet and the ground speed is smaller than STN_NUM_ADDITION_MAX_SPEED */
		if ((RadarTarget.GetPosition().GetPressureAltitude() < STN_DeleteAlt) &&
			(strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0) && 
			(RadarTarget.GetPosition().GetReportedGS() < STN_OccupiedMAxSpeed))
		{
			if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) == 0)
			{
				string ClosestStand = GetClosestStand(RadarTarget.GetPosition().GetPosition());

				for (auto& gate : LHBPGatesAndStands)
				{
					if (strcmp(gate.Number.c_str(), ClosestStand.c_str()) == 0)
					{
						gate.Occupied = true;
						gate.Callsign = Callsign;

						CallignGateMap.insert(pair<string, GatesAndStands>(Callsign, gate));
						
						if (gate.Planned)
						{
							CFlightPlan PlannedforFP = FlightPlanSelect(gate.PlannedCallsign.c_str());
							PlannedforFP.GetControllerAssignedData().SetScratchPadString("");
							if (DebugPrint)
							{
								string DisplayMsg{ "Planned gate for " + gate.PlannedCallsign + " number " + gate.Number + " was deleted"};
								DISPLAY_DEBUG(DisplayMsg.c_str());
							}
							CallignGateMap.erase(gate.PlannedCallsign);
							gate.Planned = false;
							gate.PlannedCallsign = "";
						}

						bool success = FP.GetControllerAssignedData().SetScratchPadString(gate.Number.c_str());
						if (DebugPrint)
						{
							string DisplayMsg{ Callsign + " gate number was set to " + gate.Number };
							DISPLAY_DEBUG(DisplayMsg.c_str());
						}
						break;
					}
				}
			}
		}

		if ((strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) == 0) && 
			(FP.GetDistanceToDestination() <= STN_AssignRange) &&
			(RadarTarget.GetPosition().GetPressureAltitude() >= STN_DeleteAlt) &&
			(strlen(FP.GetControllerAssignedData().GetScratchPadString()) == 0))
		{
			AircraftType = FP.GetFlightPlanData().GetAircraftFPType();

			for (auto& ac : Aircrafts)
			{
				if (strcmp(ac.ICAO.c_str(), AircraftType.c_str()) == 0)
				{
					WingSpan = ac.Wingspan;
				}
			}

			Gate = GetStand(IsFromSchengen(FP.GetFlightPlanData().GetOrigin()), Callsign, WingSpan);

			bool success = FP.GetControllerAssignedData().SetScratchPadString(Gate.c_str());
				
			if (success)
			{
				for (auto& gate : LHBPGatesAndStands)
				{
					if (strcmp(gate.Number.c_str(), Gate.c_str()) == 0)
					{
						gate.Planned = true;
						gate.PlannedCallsign = Callsign;
						CallignGateMap.insert(pair<string, GatesAndStands>(Callsign, gate));
						if (DebugPrint)
						{
							string DisplayMsg{ Callsign + " gate number suggestion is added " + Gate };
							DISPLAY_DEBUG(DisplayMsg.c_str());
						}
					}
				}
			}
		}

		/* convert meter to nm */
		double MaxDist_nm = (STN_AcMaxDistanceToGate * 0.539956803) / 1000.0;

		if (CallignGateMap.find(Callsign) != CallignGateMap.end())
		{
			map<string, GatesAndStands>::iterator CallsignGatePair = CallignGateMap.find(Callsign);
			if (CallsignGatePair->second.Occupied)
			{
				CPosition standpos;
				bool success = standpos.LoadFromStrings(CallsignGatePair->second.LongCoord.c_str(), CallsignGatePair->second.LAtCoord.c_str());


				CPosition acpos = RadarTargetSelect(FP.GetCallsign()).GetPosition().GetPosition();
				double DistAcGate = acpos.DistanceTo(standpos);

				if (DistAcGate > MaxDist_nm)
				{
					CallsignGatePair->second.Occupied = false;
					CallsignGatePair->second.Callsign = "";
					CallignGateMap.erase(Callsign);
				}
				else
				{
					if (!CallsignGatePair->second.Occupied)
					{
						if (DebugPrint)
						{
							string DisplayMsg{ "Gate " + CallsignGatePair->second.Number + " is set to free" };
							DISPLAY_DEBUG(DisplayMsg.c_str());
						}
					}
				}
			}
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
