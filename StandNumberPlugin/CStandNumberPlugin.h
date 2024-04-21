#pragma once
#include "EuroScopePlugIn.h"
#include "RadarScreen.h"

using namespace std;
using namespace EuroScopePlugIn;

class CStandNumberPlugin :
    public CPlugIn
{
public:
    explicit CStandNumberPlugin(void);
    virtual ~CStandNumberPlugin(void);

	bool OnCompileCommand(const char* command);

	void OnTimer(int Counter);

	string GetClosestStand(CPosition ACPos_f);
private:
	int ConnectionStatus;
};

