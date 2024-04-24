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

	bool OnCompileCommand(const char* command);
	void OnRadarTargetPositionUpdate(CRadarTarget RadarTarget);
	void OnTimer(int Counter);

	string GetClosestStand(CPosition ACPos_f);
	bool CheckStandOccupation(CPosition ACPos_f);
	bool IsFromSchengen(string DepAirportICAO);
	string GetStand(bool IsSchengen, string Callsign, double WingSpan);
private:
	std::string pluginDirectory;

	int ConnectionStatus;
	void LoadStandConfig(const std::string& filename);
	void LoadAircraftConfig(const std::string& filename);
};

