#pragma once
#include <vector>
#include <string>

#ifndef COPYRIGHTS
constexpr auto PLUGIN_NAME = "Standnumber Plugin";
constexpr auto PLUGIN_AUTHOR = "Tamas Bohus";
constexpr auto PLUGIN_COPYRIGHT = "GPL v3";
constexpr auto GITHUB_LINK = "https://github.com/gammasugarzas/StandNumberPlugin";
constexpr auto SCREEN_VIEW_NAME = "Stand number radar screen";
#ifdef _DEBUG
#define VERSION_FILE_STR		"1.0.0 DEV"
#else
#define VERSION_FILE_STR		"1.0.0"
#endif
#endif // !COPYRIGHTS