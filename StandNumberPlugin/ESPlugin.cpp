#include "stdafx.h"
#include "ESPlugin.h"

CStandNumberPlugin* pSNumPlug = NULL;

//---EuroScopePlugInInit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance)
{
	// create the instance
	*ppPlugInInstance = pSNumPlug = new CStandNumberPlugin();
}


//---EuroScopePlugInExit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInExit()
{
	// delete the instance
	delete pSNumPlug;
}
