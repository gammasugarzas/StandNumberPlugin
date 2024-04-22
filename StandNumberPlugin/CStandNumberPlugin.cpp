#include "pch.h"
#include "CStandNumberPlugin.h"
#include <string>
#include "version.h"

#define STN_PLUGIN_NAME "STN Plugin"

#define STN_NUM_GATES_STANDS	102

map<int, bool> StandOccupation;

GatesAndStands LHBPGatesAndStands_st[STN_NUM_GATES_STANDS] = {
	{"221", "E019.15.21.420", "N047.26.02.040", 36.0},
	{"222", "E019.15.23.292", "N047.26.03.480", 36.0},
	{"223", "E019.15.24.804", "N047.26.04.560", 36.0},
	{"224", "E019.15.29.232", "N047.26.07.872", 36.0},
	{"225", "E019.15.30.708", "N047.26.08.988", 36.0},
	{"226", "E019.15.32.616", "N047.26.10.428", 36.0},
	{"211", "E019.15.36.792", "N047.26.07.908", 36.0},
	{"044", "E019.15.44.784", "N047.26.04.164", 36.0},
	{"271", "E019.15.49.968", "N047.26.08.268", 36.0},
	{"272", "E019.15.51.372", "N047.26.07.188", 36.0},
	{"273", "E019.15.53.064", "N047.26.06.360", 36.0},
	{"274", "E019.15.54.828", "N047.26.05.028", 36.0},
	{"275", "E019.15.56.304", "N047.26.04.020", 36.0},
	{"276", "E019.15.57.888", "N047.26.03.084", 36.0},
	{"278", "E019.16.01.524", "N047.26.00.708", 43.0},
	{"279", "E019.16.03.324", "N047.25.59.376", 50.0},
	{"270", "E019.15.48.420", "N047.26.09.204", 36.0},
	{"277", "E019.15.59.436", "N047.26.02.148", 36.0},
	{"210", "E019.15.34.956", "N047.26.06.396", 36.0},
	{"212", "E019.15.38.376", "N047.26.08.952", 36.0},
	{"220", "E019.15.20.088", "N047.26.00.852", 36.0},
	{"227", "E019.15.34.128", "N047.26.11.508", 36.0},
	{"042", "E019.15.40.356", "N047.26.02.940", 38.0},
	{"043", "E019.15.41.976", "N047.26.03.840", 51.0},
	{"045", "E019.15.46.800", "N047.26.03.192", 36.0},
	{" L1", "E019.14.33.000", "N047.25.47.388", 34.09},
	{" L2", "E019.14.34.800", "N047.25.46.560", 34.09},
	{" L3", "E019.14.36.600", "N047.25.45.552", 34.09},
	{" A1", "E019.14.16.764", "N047.25.52.572", 34.09},
	{" A2", "E019.14.19.068", "N047.25.51.816", 34.09},
	{" A3", "E019.14.21.408", "N047.25.51.060", 34.09},
	{" A4", "E019.14.23.712", "N047.25.50.304", 34.09},
	{"130", "E019.14.09.708", "N047.25.54.948", 23.8},
	{"131", "E019.14.07.800", "N047.25.53.400", 19.98},
	{"132", "E019.14.11.832", "N047.25.54.228", 23.8},
	{"133", "E019.14.09.024", "N047.25.53.256", 19.98},
	{"134", "E019.14.13.632", "N047.25.53.436", 23.8},
	{"135", "E019.14.10.104", "N047.25.52.896", 19.98},
	{"137", "E019.14.11.148", "N047.25.52.536", 19.98},
	{"139", "E019.14.12.228", "N047.25.52.212", 19.98},
	{"141", "E019.14.13.308", "N047.25.51.816", 19.98},

	{"C4R", "E019.16.41.821", "N047.25.33.148", 36.0},
	{" C4", "E019.16.43.418", "N047.25.33.453", 68.59},
	{"C4L", "E019.16.43.773", "N047.25.36.148", 36.0},
	{"C3R", "E019.16.44.371", "N047.25.35.515", 36.0},
	{" C3", "E019.16.45.988", "N047.25.35.260", 68.59},
	{"C3L", "E019.16.46.367", "N047.25.36.277", 36.0},

	{" 031", "E019.15.36.072", "N047.25.51.096", 36.0},
	{" 032", "E019.15.34.740", "N047.25.52.284", 36.0},
	{" 033", "E019.15.33.048", "N047.25.53.148", 36.0},
	{"34R", "E019.15.31.932", "N047.25.54.444", 36.0},
	{"034", "E019.15.31.464", "N047.25.54.840", 65.0},
	{"34L", "E019.15.29.700", "N047.25.54.876", 36.0},
	{"35R", "E019.15.28.980", "N047.25.56.388", 36.0},
	{"035", "E019.15.28.548", "N047.25.56.712", 65.0},
	{"35L", "E019.15.26.784", "N047.25.56.820", 36.0},
	{"36R", "E019.15.26.028", "N047.25.58.368", 36.0},
	{"036", "E019.15.25.452", "N047.25.58.584", 65.0},
	{"36L", "E019.15.23.940", "N047.25.58.692", 28.72},
	{"037", "E019.15.28.584", "N047.26.01.788", 36.0},
	{"038", "E019.15.30.204", "N047.26.00.780", 36.0},
	{"39R", "E019.15.32.004", "N047.25.59.736", 36.0},
	{"039", "E019.15.33.408", "N047.25.58.836", 65.0},
	{"39L", "E019.15.33.768", "N047.25.58.692", 36.0},
	{"12A", "E019.15.36.900", "N047.26.07.872", 47.57},
	{"C1R", "E019.16.38.388", "N047.25.42.528", 36.0},
	{" C1", "E019.16.37.920", "N047.25.42.204", 68.59},
	{"C1L", "E019.16.37.704", "N047.25.41.052", 36.0},
	{"C2R", "E019.16.35.580", "N047.25.40.440", 36.0},
	{" C2", "E019.16.35.112", "N047.25.40.152", 68.59},
	{"C2L", "E019.16.34.896", "N047.25.38.964", 36.0},
	{"101", "E019.13.23.772", "N047.26.38.760", 48.0},
	{"102", "E019.13.25.644", "N047.26.36.168", 48.0},
	{"103", "E019.13.27.624", "N047.26.33.576", 48.0},
	{"105", "E019.13.29.424", "N047.26.31.056", 48.0},
	{"104", "E019.13.29.964", "N047.26.30.912", 36.0},
	{"106", "E019.13.30.900", "N047.26.29.616", 36.0},
	{"108", "E019.13.31.116", "N047.26.28.680", 48.0},
	{"107", "E019.13.31.908", "N047.26.28.248", 36.0},
	{"150", "E019.13.31.404", "N047.26.26.484", 30.4},
	{"151", "E019.13.33.204", "N047.26.25.980", 30.4},
	{"152", "E019.13.33.060", "N047.26.25.080", 30.4},
	{"153", "E019.13.30.720", "N047.26.25.044", 28.65},
	{"154", "E019.13.32.520", "N047.26.22.200", 28.65},
	{"155", "E019.13.30.540", "N047.26.22.200", 28.65},
	{"160", "E019.13.32.520", "N047.26.20.580", 19.5},
	{"161", "E019.13.31.764", "N047.26.20.112", 19.5},
	{"162", "E019.13.31.368", "N047.26.19.500", 19.5},
	{"170", "E019.13.33.564", "N047.26.19.104", 23.9},
	{"171", "E019.13.33.636", "N047.26.17.484", 23.9},
	{"172", "E019.13.34.680", "N047.26.15.936", 23.9},
	{"110", "E019.13.38.424", "N047.26.22.128", 36.0},
	{"111", "E019.13.39.360", "N047.26.21.804", 52.0},
	{"112", "E019.13.39.756", "N047.26.21.084", 36.0},
	{"113", "E019.13.41.088", "N047.26.20.328", 52.0},
	{"114", "E019.13.41.016", "N047.26.20.000", 36.0},
	{"115", "E019.13.43.572", "N047.26.18.528", 68.59},
	{"116", "E019.13.45.624", "N047.26.16.188", 36.0},
	{"117", "E019.13.45.912", "N047.26.16.404", 68.59},
	{"17A", "E019.13.45.804", "N047.26.16.224", 68.59},
	{"B5HB", "E019.15.24.912", "N047.26.30.480", 99.99},
	{"Heli", "E019.13.57.792", "N047.26.07.908", 99.99}
};

CStandNumberPlugin::CStandNumberPlugin()
	: CPlugIn(COMPATIBILITY_CODE,
		PLUGIN_NAME,
		VERSION_FILE_STR,
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT)
{
	ConnectionStatus = 0;
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

void CStandNumberPlugin::OnTimer(int Counter)
{
	if (GetConnectionType() > 0)
		ConnectionStatus++;
	else if (GetConnectionType() != ConnectionStatus)
	{

	}

#ifdef _DEBUG
	if (ConnectionStatus == 10) {
		DisplayUserMessage(STN_PLUGIN_NAME, "Debug", "Active connection establish, automatic stand number assignment enabled", true, false, false, false, false);
	}
#endif

	for (CFlightPlan FP = FlightPlanSelectFirst(); FP.IsValid(); FP = FlightPlanSelectNext(FP))
	{
		if (false == FP.GetSimulated())
		{
			if ((FP.GetFPTrackPosition().GetFlightLevel() >= 700) && (strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) != 0) && (strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0))
			{
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) != 0)
				{
#ifdef _DEBUG
					string DisplayMsg{ string { FP.GetCallsign() } + " gate number " + string {FP.GetControllerAssignedData().GetScratchPadString()} + " was deleted" };
					DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
					FP.GetControllerAssignedData().SetScratchPadString("");
				}
			}

			if ((FP.GetFPTrackPosition().GetFlightLevel() < 700) && (strcmp("LHBP", FP.GetFlightPlanData().GetOrigin()) == 0) && (FP.GetFPTrackPosition().GetReportedGS() < 10))
			{
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) == 0)
				{
					string callsign = FP.GetCallsign();
					string stand = GetClosestStand(RadarTargetSelect(FP.GetCallsign()).GetPosition().GetPosition());
					bool success = FP.GetControllerAssignedData().SetScratchPadString(stand.c_str());
#ifdef _DEBUG
					string DisplayMsg{ string { FP.GetCallsign() } + " gate number was set to " + stand };
					DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
				}
			}
		}
	}		
}

string CStandNumberPlugin::GetClosestStand(CPosition ACPos_f)
{
	struct {
		string Name = "    ";
		double Dist = 9999.0;
	} Closest;

	for (const GatesAndStands& ActualGateStand : LHBPGatesAndStands_st)
	{
		CPosition standpos;
		bool success = standpos.LoadFromStrings(ActualGateStand.LongCoord.c_str(), ActualGateStand.LAtCoord.c_str());
		if (success)
		{
			double dist = ACPos_f.DistanceTo(standpos);
			if (dist < 10.0)
			{
				if (dist < Closest.Dist)
				{
					Closest.Dist = dist;
					Closest.Name = ActualGateStand.Number;
				}
			}
		}
	}
	return Closest.Name;
}

