#include "pch.h"
#include "CStandNumberPlugin.h"
#include <string>
#include "version.h"

#define STN_PLUGIN_NAME "STN Plugin"

RadarScreen* openedRadar = nullptr;

CStandNumberPlugin::CStandNumberPlugin()
	: CPlugIn(COMPATIBILITY_CODE,
		PLUGIN_NAME,
		VERSION_FILE_STR,
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT)
{
	ConnectionStatus = 0;
	this->RegisterPlugin();
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


void CStandNumberPlugin::OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize) {
	if (FlightPlan.IsValid()) {

	}
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

	if (ControllerMyself().IsValid() && (ControllerMyself().GetFacility() >= 5))
	{
		for (CFlightPlan FP = FlightPlanSelectFirst(); FP.IsValid(); FP = FlightPlanSelectNext(FP))
		{
			if (FP.GetTrackingControllerIsMe() && (FP.GetFPTrackPosition().GetFlightLevel() >= 700) && (strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) != 0))
			{	
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) != 0)
				{
#ifdef _DEBUG
					string DisplayMsg{ "The following aircraft's " + string { FP.GetCallsign() } + " gate number: " + string {FP.GetControllerAssignedData().GetScratchPadString()} + " deleted"};
					DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
					FP.GetControllerAssignedData().SetScratchPadString("");
				}
			}
		}		
	}
	else
	{
		for (CFlightPlan FP = FlightPlanSelectFirst(); FP.IsValid(); FP = FlightPlanSelectNext(FP))
		{
			/* above 700 feet */
			if (FP.GetTrackingControllerIsMe() && (FP.GetFPTrackPosition().GetFlightLevel() >= 700) && (strlen(FP.GetCoordinatedNextController()) == 0) && (strcmp("LHBP", FP.GetFlightPlanData().GetDestination()) != 0))
			{
				if (strlen(FP.GetControllerAssignedData().GetScratchPadString()) != 0)
				{
#ifdef _DEBUG
					string DisplayMsg{ "The following aircraft's " + string { FP.GetCallsign() } + " gate number: " + string {FP.GetControllerAssignedData().GetScratchPadString()} + " deleted" };
					DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
					FP.GetControllerAssignedData().SetScratchPadString("");
				}
			}
		}
	}
}


CRadarScreen* CStandNumberPlugin::OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated)
{
	if (!strcmp(sDisplayName, SCREEN_VIEW_NAME))
	{
		openedRadar = new RadarScreen();
		return openedRadar;
	}

	return nullptr;
}

void CStandNumberPlugin::RegisterPlugin() {
	RegisterDisplayType(SCREEN_VIEW_NAME, false, true, true, true);
}
