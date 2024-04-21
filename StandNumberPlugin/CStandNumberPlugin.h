#pragma once
#include "EuroScopePlugIn.h"
#include "RadarScreen.h"

using namespace std;
using namespace EuroScopePlugIn;

struct ItemCodes
{
	enum ItemTypes : int
	{
		TAG_ITEM_ISMODES = 501,
		TAG_ITEM_EHS_HDG,
		TAG_ITEM_EHS_ROLL,
		TAG_ITEM_EHS_GS,
		TAG_ITEM_ERROR_MODES_USE,
		TAG_ITEM_SQUAWK
	};

	enum ItemFunctions : int
	{
		TAG_FUNC_SQUAWK_POPUP = 869,
		TAG_FUNC_ASSIGN_SQUAWK,
		TAG_FUNC_ASSIGN_SQUAWK_AUTO,
		TAG_FUNC_ASSIGN_SQUAWK_MANUAL,
		TAG_FUNC_ASSIGN_SQUAWK_VFR,
		TAG_FUNC_ASSIGN_SQUAWK_MODES,
		TAG_FUNC_ASSIGN_SQUAWK_DISCRETE
	};
};

struct EquipmentCodes
{
	string FAA{ "HLEGWQS" };
	string ICAO_MODE_S{ "EHILS" };
	string ICAO_EHS{ "EHLS" };
};

struct SquawkCodes
{
	const char* MODE_S{ "1000" };
	const char* VFR{ "7000" };
};

class CStandNumberPlugin :
    public CPlugIn
{
public:
    explicit CStandNumberPlugin(void);
    virtual ~CStandNumberPlugin(void);

	bool OnCompileCommand(const char* command);

	void OnTimer(int Counter);

	CRadarScreen* OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);
	
	void OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize);

protected:
	void RegisterPlugin();

private:
	int ConnectionStatus;
};

