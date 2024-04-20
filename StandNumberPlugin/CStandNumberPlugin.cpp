#include "pch.h"
#include "CStandNumberPlugin.h"

#ifndef COPYRIGHTS
constexpr auto PLUGIN_NAME = "StandnumberPlugin";
constexpr auto PLUGIN_AUTHOR = "Tamas Bohus";
constexpr auto PLUGIN_COPYRIGHT = "MIT License, Copyright (c) 2024 Tamas Bohus";
constexpr auto GITHUB_LINK = "https://github.com/gammasugarzas/StandNumberPlugin";
#endif // !COPYRIGHTS

CStandNumberPlugin::CStandNumberPlugin(void)
	: CPlugIn(COMPATIBILITY_CODE,
		PLUGIN_NAME,
#ifdef DEBUG
		VERSION_FILE_STR " DEBUG",
#else
		VERSION_FILE_STR,
#endif // DEBUG
		PLUGIN_AUTHOR,
		PLUGIN_COPYRIGHT)
{

}