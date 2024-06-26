#pragma once
#include "EuroScopePlugIn.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace EuroScopePlugIn;

typedef struct GatesAndStands
{
	string Number;
	string LongCoord;
	string LAtCoord;
	double Span;
	bool Occupied;
	bool Planned;
	string PlannedCallsign;
	bool Schengen;
	string Callsign;
} GatesAndStands;

typedef struct AirlineStands
{
	string AirlineCode;
	vector <string> PreferedStands;
} AirlineStands;

typedef struct Aircraft
{
	string ICAO;
	double Wingspan;
}Aircraft;

class CStandNumberPlugin :
    public CPlugIn
{
public:
    explicit CStandNumberPlugin(void);
    virtual ~CStandNumberPlugin(void);

	virtual bool OnCompileCommand(const char* command);
	virtual void OnFlightPlanDisconnect(CFlightPlan FP_f);
	virtual void OnRadarTargetPositionUpdate(CRadarTarget RadarTarget);
	virtual void OnTimer(int Counter);
	virtual void OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode,
		int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize);
	virtual void OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area);

protected:
	CFlightPlanList  m_GateStatusList;

private:
	std::string pluginDirectory;

	int ConnectionStatus;
	void LoadStandConfig(void);
	void LoadAircraftConfig(const std::string& filename);
	string GetGateStatus(void);
	string GetClosestStand(CPosition ACPos_f);
	bool CheckStandOccupation(CPosition ACPos_f);
	bool IsFromSchengen(string DepAirportICAO);
	string GetStand(bool IsSchengen, string Callsign, double WingSpan);
	
	bool IsRelevantFLightplan(CFlightPlan FP_f);
	bool IsDepartingAircraftWithStand(CRadarTarget RT_f, CFlightPlan FP_f);
	bool IsDepartingAircraftWithoutStand(CRadarTarget RT_f, CFlightPlan FP_f);
	bool IsArrivingAircraftWithoutStand(CRadarTarget RT_f, CFlightPlan FP_f);
	bool CheckArrivinAircraftsStandForUpdate(CRadarTarget RT_f, CFlightPlan FP_f);
	bool IsGateStillOccupiedByCallsign(string Callsign_f, CFlightPlan FP_f);
	bool GetGateByCallsign(string CallSign_f, int& GateIdx);
	bool GetGateByNumber(string GateNumber_f, int& GateIdx_f);
	void FreeOccupiedGate(string Callsign_f, CFlightPlan FP_f);
	void ReserveOccupiedGate(string Callsign_f, CRadarTarget RT_f, CFlightPlan FP_f);
	void PlanGate(string Callsign_f, string GateName_f, CFlightPlan FP_f);
	double GetWingspan(CFlightPlan FP_f);
};

