// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

// string
#include <string>
#include <format>
#include <fstream>
#include <sstream>
#include <regex>
// containers
#include <array>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
// threading
#include <shared_mutex>
#include <thread>
#include <atomic>
// others
#include <algorithm>
#include <memory>
#include <functional>

#endif //PCH_H
