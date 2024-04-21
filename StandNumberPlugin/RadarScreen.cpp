#include "stdafx.h"
#include "RadarScreen.h"

#define STN_NUM_GATES_STANDS	102

GatesAndStands LHBPGatesAndStands_st[STN_NUM_GATES_STANDS] = {
	{"R221", "E019.15.21.420", "N047.26.02.040", 36.0},
	{"R222", "E019.15.23.292", "N047.26.03.480", 36.0},
	{"R223", "E019.15.24.804", "N047.26.04.560", 36.0},
	{"R224", "E019.15.29.232", "N047.26.07.872", 36.0},
	{"R225", "E019.15.30.708", "N047.26.08.988", 36.0},
	{"R226", "E019.15.32.616", "N047.26.10.428", 36.0},
	{"R211", "E019.15.36.792", "N047.26.07.908", 36.0},
	{" 044", "E019.15.44.784", "N047.26.04.164", 36.0},
	{"R271", "E019.15.49.968", "N047.26.08.268", 36.0},
	{"R272", "E019.15.51.372", "N047.26.07.188", 36.0},
	{"R273", "E019.15.53.064", "N047.26.06.360", 36.0},
	{"R274", "E019.15.54.828", "N047.26.05.028", 36.0},
	{"R275", "E019.15.56.304", "N047.26.04.020", 36.0},
	{"R276", "E019.15.57.888", "N047.26.03.084", 36.0},
	{"R278", "E019.16.01.524", "N047.26.00.708", 43.0},
	{"R279", "E019.16.03.324", "N047.25.59.376", 50.0},
	{"R270", "E019.15.48.420", "N047.26.09.204", 36.0},
	{"R277", "E019.15.59.436", "N047.26.02.148", 36.0},
	{"R210", "E019.15.34.956", "N047.26.06.396", 36.0},
	{"R212", "E019.15.38.376", "N047.26.08.952", 36.0},
	{"R220", "E019.15.20.088", "N047.26.00.852", 36.0},
	{"R227", "E019.15.34.128", "N047.26.11.508", 36.0},
	{" 042", "E019.15.40.356", "N047.26.02.940", 38.0},
	{" 043", "E019.15.41.976", "N047.26.03.840", 51.0},
	{" 045", "E019.15.46.800", "N047.26.03.192", 36.0},
	{"  L1", "E019.14.33.000", "N047.25.47.388", 34.09},
	{"  L2", "E019.14.34.800", "N047.25.46.560", 34.09},
	{"  L3", "E019.14.36.600", "N047.25.45.552", 34.09},
	{"  A1", "E019.14.16.764", "N047.25.52.572", 34.09},
	{"  A2", "E019.14.19.068", "N047.25.51.816", 34.09},
	{"  A3", "E019.14.21.408", "N047.25.51.060", 34.09},
	{"  A4", "E019.14.23.712", "N047.25.50.304", 34.09},
	{"G130", "E019.14.09.708", "N047.25.54.948", 23.8},
	{"G131", "E019.14.07.800", "N047.25.53.400", 19.98},
	{"G132", "E019.14.11.832", "N047.25.54.228", 23.8},
	{"G133", "E019.14.09.024", "N047.25.53.256", 19.98},
	{"G134", "E019.14.13.632", "N047.25.53.436", 23.8},
	{"G135", "E019.14.10.104", "N047.25.52.896", 19.98},
	{"G137", "E019.14.11.148", "N047.25.52.536", 19.98},
	{"G139", "E019.14.12.228", "N047.25.52.212", 19.98},
	{"G141", "E019.14.13.308", "N047.25.51.816", 19.98},

	{" C4R", "E019.16.41.821", "N047.25.33.148", 36.0},
	{"  C4", "E019.16.43.418", "N047.25.33.453", 68.59},
	{" C4L", "E019.16.43.773", "N047.25.36.148", 36.0},
	{" C3R", "E019.16.44.371", "N047.25.35.515", 36.0},
	{"  C3", "E019.16.45.988", "N047.25.35.260", 68.59},
	{" C3L", "E019.16.46.367", "N047.25.36.277", 36.0},

	{" 031", "E019.15.36.072", "N047.25.51.096", 36.0},
	{" 032", "E019.15.34.740", "N047.25.52.284", 36.0},
	{" 033", "E019.15.33.048", "N047.25.53.148", 36.0},
	{" 34R", "E019.15.31.932", "N047.25.54.444", 36.0},
	{" 034", "E019.15.31.464", "N047.25.54.840", 65.0},
	{" 34L", "E019.15.29.700", "N047.25.54.876", 36.0},
	{" 35R", "E019.15.28.980", "N047.25.56.388", 36.0},
	{" 035", "E019.15.28.548", "N047.25.56.712", 65.0},
	{" 35L", "E019.15.26.784", "N047.25.56.820", 36.0},
	{" 36R", "E019.15.26.028", "N047.25.58.368", 36.0},
	{" 036", "E019.15.25.452", "N047.25.58.584", 65.0},
	{" 36L", "E019.15.23.940", "N047.25.58.692", 28.72},
	{" 037", "E019.15.28.584", "N047.26.01.788", 36.0},
	{" 038", "E019.15.30.204", "N047.26.00.780", 36.0},
	{" 39R", "E019.15.32.004", "N047.25.59.736", 36.0},
	{" 039", "E019.15.33.408", "N047.25.58.836", 65.0},
	{" 39L", "E019.15.33.768", "N047.25.58.692", 36.0},
	{"212A", "E019.15.36.900", "N047.26.07.872", 47.57},
	{" C1R", "E019.16.38.388", "N047.25.42.528", 36.0},
	{"  C1", "E019.16.37.920", "N047.25.42.204", 68.59},
	{" C1L", "E019.16.37.704", "N047.25.41.052", 36.0},
	{" C2R", "E019.16.35.580", "N047.25.40.440", 36.0},
	{"  C2", "E019.16.35.112", "N047.25.40.152", 68.59},
	{" C2L", "E019.16.34.896", "N047.25.38.964", 36.0},
	{"R101", "E019.13.23.772", "N047.26.38.760", 48.0},
	{"R102", "E019.13.25.644", "N047.26.36.168", 48.0},
	{"R103", "E019.13.27.624", "N047.26.33.576", 48.0},
	{"R105", "E019.13.29.424", "N047.26.31.056", 48.0},
	{"R104", "E019.13.29.964", "N047.26.30.912", 36.0},
	{"R106", "E019.13.30.900", "N047.26.29.616", 36.0},
	{"R108", "E019.13.31.116", "N047.26.28.680", 48.0},
	{"R107", "E019.13.31.908", "N047.26.28.248", 36.0},
	{"G150", "E019.13.31.404", "N047.26.26.484", 30.4},
	{"G151", "E019.13.33.204", "N047.26.25.980", 30.4},
	{"G152", "E019.13.33.060", "N047.26.25.080", 30.4},
	{"G153", "E019.13.30.720", "N047.26.25.044", 28.65},
	{"G154", "E019.13.32.520", "N047.26.22.200", 28.65},
	{"G155", "E019.13.30.540", "N047.26.22.200", 28.65},
	{"G160", "E019.13.32.520", "N047.26.20.580", 19.5},
	{"G161", "E019.13.31.764", "N047.26.20.112", 19.5},
	{"G162", "E019.13.31.368", "N047.26.19.500", 19.5},
	{"G170", "E019.13.33.564", "N047.26.19.104", 23.9},
	{"G171", "E019.13.33.636", "N047.26.17.484", 23.9},
	{"G172", "E019.13.34.680", "N047.26.15.936", 23.9},
	{"R110", "E019.13.38.424", "N047.26.22.128", 36.0},
	{"R111", "E019.13.39.360", "N047.26.21.804", 52.0},
	{"R112", "E019.13.39.756", "N047.26.21.084", 36.0},
	{"R113", "E019.13.41.088", "N047.26.20.328", 52.0},
	{"R114", "E019.13.41.016", "N047.26.20.000", 36.0},
	{"R115", "E019.13.43.572", "N047.26.18.528", 68.59},
	{"R116", "E019.13.45.624", "N047.26.16.188", 36.0},
	{"R117", "E019.13.45.912", "N047.26.16.404", 68.59},
	{"117A", "E019.13.45.804", "N047.26.16.224", 68.59},
	{"B5HB", "E019.15.24.912", "N047.26.30.480", 99.99},
	{"Heli", "E019.13.57.792", "N047.26.07.908", 99.99}
};

RadarScreen::RadarScreen()
{
}

RadarScreen::~RadarScreen()
{
}

void RadarScreen::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
	if (!RadarTarget.IsValid() || !RadarTarget.GetPosition().IsValid())
		return;

	CRadarTargetPositionData RtPos = RadarTarget.GetPosition();

	CFlightPlan fp = GetPlugIn()->FlightPlanSelect(RadarTarget.GetCallsign());

	if (fp.IsValid()) {
		/* below 700 feet */
		if (fp.GetFPTrackPosition().GetFlightLevel() < 700)
		{
			//FP.GetFlightPlanData().GetAircraftType();
			if (strlen(fp.GetControllerAssignedData().GetScratchPadString()) == 0)
			{
				string StNr = GetClosestStand(RtPos.GetPosition());
				bool success = fp.GetControllerAssignedData().SetScratchPadString(StNr.c_str());
#ifdef _DEBUG
				//				string DisplayMsg{ "The following aircraft's " + string { FP.GetCallsign() } + " stand was updated to " + StNr };
				//				DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
			}
		}
	}
}

string RadarScreen::GetClosestStand(CPosition ACPos_f)
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
			if (dist < 9999.0)
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


void RadarScreen::OnRefresh(HDC hDC, int Phase)
{

}

void RadarScreen::OnMoveScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, bool Released)
{
}

void RadarScreen::OnOverScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area)
{
}

void RadarScreen::OnFlightPlanControllerAssignedDataUpdate(CFlightPlan FlightPlan, int DataType)
{
}

void RadarScreen::OnClickScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, int Button)
{
}

void RadarScreen::OnFunctionCall(int FunctionId, const char * sItemString, POINT Pt, RECT Area)
{
}

void RadarScreen::OnDoubleClickScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, int Button)
{
}

void RadarScreen::OnAsrContentToBeSaved()
{
};

void RadarScreen::OnAsrContentLoaded(bool Loaded)
{
};