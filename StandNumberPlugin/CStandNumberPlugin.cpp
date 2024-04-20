#include "pch.h"
#include "CStandNumberPlugin.h"
#include <string>
#include "version.h"

#define STN_PLUGIN_NAME "STN Plugin"

CStandNumberPlugin::CStandNumberPlugin(void)
	: CPlugIn(COMPATIBILITY_CODE,
		PLUGIN_NAME,
		VERSION_FILE_STR,
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT)
{
	ConnectionStatus = 0;
}

CStandNumberPlugin::~CStandNumberPlugin(void)
{
}

bool CStandNumberPlugin::OnCompileCommand(const char* command)
{
	string commandString(command);
	cmatch matches;

	if (_stricmp(command, ".help") == 0)
	{
		DisplayUserMessage(STN_PLUGIN_NAME, "", "Stand number assigner and deletion plugin", true, true, true, true, false);
		return NULL;
	}

	return false;
}

void CStandNumberPlugin::OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize)
{
	if (!FlightPlan.IsValid())
		return;

	if (ItemCode == ItemCodes::TAG_ITEM_ISMODES)
	{
		if (IsAcModeS(FlightPlan))
			strcpy_s(sItemString, 16, "S");
		else
			strcpy_s(sItemString, 16, "A");
	}
	else
	{
		if (!RadarTarget.IsValid())
			return;

		if (ItemCode == ItemCodes::TAG_ITEM_EHS_HDG)
		{

		}
		else if (ItemCode == ItemCodes::TAG_ITEM_EHS_ROLL)
		{


		}
		else if (ItemCode == ItemCodes::TAG_ITEM_EHS_GS)
		{

		}

		else if (ItemCode == ItemCodes::TAG_ITEM_ERROR_MODES_USE)
		{

		}
		else if (ItemCode == ItemCodes::TAG_ITEM_SQUAWK)
		{

		}

	}
}

void CStandNumberPlugin::OnTimer(int Counter)
{
	//	if (fUpdateString.valid() && fUpdateString.wait_for(0ms) == future_status::ready)
	//		DoInitialLoad(fUpdateString);

	if (GetConnectionType() > 0)
		ConnectionStatus++;
	else if (GetConnectionType() != ConnectionStatus)
	{
		ConnectionStatus = 0;
		//		if (ProcessedFlightPlans.size() > 0)
		//		{
		//			ProcessedFlightPlans.clear();
#ifdef _DEBUG
		DisplayUserMessage(STN_PLUGIN_NAME, "Debug", "Connection Status 0 detected, all processed flight plans are removed from the list", true, false, false, false, false);
#endif
		//		}
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
			if (FP.GetTrackingControllerIsMe() && (FP.GetFPTrackPosition().GetFlightLevel() >= 700))
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
			if (FP.GetTrackingControllerIsMe() && (FP.GetFPTrackPosition().GetFlightLevel() >= 700) && (strlen(FP.GetCoordinatedNextController()) == 0))
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

void CStandNumberPlugin::OnRefreshFpListContent(CFlightPlanList AcList)
{
	string DisplayMsg{ "teszt" };
	DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);

	if (ControllerMyself().IsValid() && (ControllerMyself().GetFacility()  >= 5)) // && RadarTargetSelectASEL().IsValid())
	{
#ifdef _DEBUG
		string DisplayMsg{ "The following aircraft appeared in your sector " + string { FlightPlanSelectASEL().GetCallsign() } };
		DisplayUserMessage(STN_PLUGIN_NAME, "Debug", DisplayMsg.c_str(), true, false, false, false, false);
#endif
		//for (CFlightPlan FP = FlightPlanSelectFirst(); FP.IsValid(); FP = FlightPlanSelectNext(FP))
		{
		//	FpListEHS.RemoveFpFromTheList(FP);
		}
		//FpListEHS.AddFpToTheList(FlightPlanSelectASEL());
	}
}

inline bool CStandNumberPlugin::IsFlightPlanProcessed(CFlightPlan& FlightPlan)
{
	return false;
}

bool CStandNumberPlugin::IsAcModeS(const CFlightPlan& FlightPlan) const
{
	return false;// HasEquipment(FlightPlan, acceptEquipmentFAA, acceptEquipmentICAO, EquipmentCodesICAO);
}

bool CStandNumberPlugin::IsEHS(const CFlightPlan& FlightPlan) const
{
	return false;// HasEquipment(FlightPlan, acceptEquipmentFAA, true, EquipmentCodesICAOEHS);
}

bool CStandNumberPlugin::HasEquipment(const CFlightPlan& FlightPlan, bool acceptEquipmentFAA, bool acceptEquipmentICAO, string CodesICAO) const
{
	return false;
}