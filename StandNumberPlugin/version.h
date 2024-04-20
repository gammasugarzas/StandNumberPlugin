#pragma once
#include <vector>
#include <string>

#ifndef COPYRIGHTS
constexpr auto PLUGIN_NAME = "Standnumber Plugin";
constexpr auto PLUGIN_AUTHOR = "Tamas Bohus";
constexpr auto PLUGIN_COPYRIGHT = "MIT License, Copyright (c) 2024 Tamas Bohus";
constexpr auto GITHUB_LINK = "https://github.com/gammasugarzas/StandNumberPlugin";
#ifdef _DEBUG
#define VERSION_FILE_STR		"1.0.0 DEV"
#else
#define VERSION_FILE_STR		"1.0.0"
#endif
#endif // !COPYRIGHTS