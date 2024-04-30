#pragma once
#include <vector>
#include <string>

#ifndef COPYRIGHTS
constexpr auto PLUGIN_NAME = "Standnumber Plugin";
constexpr auto PLUGIN_AUTHOR = "Tamas Bohus";
constexpr auto PLUGIN_COPYRIGHT = "GPL v3";
constexpr auto GITHUB_LINK = "https://github.com/gammasugarzas/StandNumberPlugin";
#ifdef _DEBUG
#define VERSION_FILE_STR		"1.0.8 DEV"
#else
#define VERSION_FILE_STR		"1.0.7"
#endif
#endif // !COPYRIGHTS