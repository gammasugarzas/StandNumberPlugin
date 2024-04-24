#include "pch.h"
#include "CStandNumberPlugin.h"
#include <string>
#include "version.h"
#include <winnt.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define STN_PLUGIN_NAME "STN Plugin"

#define STN_NUM_ASSIGN_STAND_RANGE		25
#define STN_NUM_DELETION_ALT_FEET		700
#define STN_NUM_ADDITION_MAX_SPEED_KTS	5
#define STN_NUM_MAX_DISTANCE_TO_GATE_M	55

#define DISPLAY_WARNING(str) DisplayUserMessage(STN_PLUGIN_NAME, "Warning", str, true, true, true, true, false);
#define DISPLAY_INFO(str) DisplayUserMessage(STN_PLUGIN_NAME, "Info", str, true, true, true, true, false);

vector <AirlineStands> AvailableStands;
vector<GatesAndStands> LHBPGatesAndStands;
vector<string> SchengenCountries;
vector<Aircraft> Aircrafts;

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

inline static bool startsWith(const char* pre, const char* str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
};

CStandNumberPlugin::~CStandNumberPlugin()
{
}

bool CStandNumberPlugin::OnCompileCommand(const char* sCommandLine)
{
	string commandString(sCommandLine);
	cmatch matches;

	if (startsWith(".help", sCommandLine))
	{
		DisplayUserMessage(STN_PLUGIN_NAME, "", "Stand number assigner and deletion plugin", true, true, true, true, false);
		return NULL;
	}

	return false;
}

void CStandNumberPlugin::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
	string Callsign = RadarTarget.GetCallsign();
	CFlightPlan FP = FlightPlanSelect(RadarTarget.GetCallsign());

	if (!FP.IsValid())
	{
#ifdef _DEBUG
		string DisplayMsg{ "Selected flightplan invalid" };
		DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
	}
	else
	{
		if (false == FP.GetSimulated())
		{
			/* if LHBP is the departure airport, but LHBP is not the dest airport, and the altitude is higher than STN_NUM_DELETION_ALT feet delete the schratchpad contents*/
			if ((RadarTarget.GetPosition().GetPressureAltitude() >= STN_NUM_DELETION_ALT_FEET) && (strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) != 0) && (strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0))
			{
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) != 0)
				{
#ifdef _DEBUG
					string DisplayMsg{ Callsign + " gate number " + string {FP.GetControllerAssignedData().GetScratchPadString()} + " was deleted" };
					DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
					FP.GetControllerAssignedData().SetScratchPadString("");
				}
			}
			/* if LHBP is the departure airport and the altitude is lower than 700 feet and the ground speed is smaller than STN_NUM_ADDITION_MAX_SPEED */
			if ((RadarTarget.GetPosition().GetPressureAltitude() < STN_NUM_DELETION_ALT_FEET) && (strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0) && (RadarTarget.GetPosition().GetReportedGS() < STN_NUM_ADDITION_MAX_SPEED_KTS))
			{
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) == 0)
				{
					string actualstand = GetClosestStand(RadarTarget.GetPosition().GetPosition());

					for (auto& gate : LHBPGatesAndStands)
					{
						if (strcmp(gate.Number.c_str(), actualstand.c_str()) == 0)
						{
							gate.Occupied = true;
							gate.Callsign = Callsign;
							gate.Planned = false;
							gate.PlannedCallsign = "";

							bool success = FP.GetControllerAssignedData().SetScratchPadString(gate.Number.c_str());
#ifdef _DEBUG
							string DisplayMsg{ Callsign + " gate number was set to " + gate.Number };
							DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
							break;
						}
					}
				}
			}

			if ((strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) == 0) && (FP.GetDistanceToDestination() < STN_NUM_ASSIGN_STAND_RANGE))
			{
				if (RadarTarget.GetPosition().GetPressureAltitude() >= STN_NUM_DELETION_ALT_FEET)
				{
					if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) == 0)
					{
						double ws;
						string actype = FP.GetFlightPlanData().GetAircraftFPType();
						for (auto& ac : Aircrafts)
						{
							if (strcmp(ac.ICAO.c_str(), actype.c_str()) == 0)
							{
								ws = ac.Wingspan;
							}
						}
						string Gate = GetStand(IsFromSchengen(FP.GetFlightPlanData().GetOrigin()), Callsign, ws);

						bool success = FP.GetControllerAssignedData().SetScratchPadString(Gate.c_str());
						if (success)
						{
							for (auto& gate : LHBPGatesAndStands)
							{
								if (strcmp(gate.Number.c_str(), Gate.c_str()) == 0)
								{
									gate.Planned = true;
									gate.PlannedCallsign = Callsign;
								}
							}
						}
#ifdef _DEBUG
						string DisplayMsg{ Callsign + " gate number suggestion is added " + Gate };
						DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
					}
				}
				else
				{
					for (auto& gate : LHBPGatesAndStands)
					{
						if (strcmp(gate.PlannedCallsign.c_str(), Callsign.c_str()))
						{
							gate.Planned = false;
							gate.PlannedCallsign = "";
						}
					}
				}
			}
		}
	}
}

void CStandNumberPlugin::OnTimer(int Counter)
{
	/* convert meter to nm */
	double MaxDist_nm = (STN_NUM_MAX_DISTANCE_TO_GATE_M * 0.539956803) / 1000.0;

	for (auto& gate : LHBPGatesAndStands)
	{
		if (gate.Occupied)
		{
			bool fpfound = false;
			CPosition standpos;
			bool success = standpos.LoadFromStrings(gate.LongCoord.c_str(), gate.LAtCoord.c_str());

			for (CFlightPlan FP = FlightPlanSelectFirst(); FP.IsValid(); FP = FlightPlanSelectNext(FP))
			{
				if (false == FP.GetSimulated() && (strcmp(gate.Callsign.c_str(), FP.GetCallsign()) == 0))
				{
					fpfound = true;
					CPosition acpos = RadarTargetSelect(FP.GetCallsign()).GetPosition().GetPosition();
					double DistAcGate = acpos.DistanceTo(standpos);

					if (DistAcGate > MaxDist_nm)
					{

						gate.Occupied = false;
						gate.Callsign = "";
						break;
					}
				}
			}
			if (!fpfound)
			{
				gate.Occupied = false;
				gate.Callsign = "";
			}
#ifdef _DEBUG
			if (!gate.Occupied)
			{
				string DisplayMsg{ "Gate " + gate.Number + " is set to free" };
				DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
			}
#endif
		}
	}
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
#ifdef _DEBUG
	string DisplayMsg = { "Closest distance " + to_string(Closest.Dist) };
	DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
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
							break;
						}
					}
				}
				MaxRetries--;
			}
		}
	}
	
	return GateNum;
}
