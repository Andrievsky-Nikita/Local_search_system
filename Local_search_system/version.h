#pragma once
#include "functions.h"
#include <iostream>
#include <filesystem>
#include <system_error>
#include <fstream>
#include <vector>
#include <string>
#include <initializer_list>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace version
{
	inline constexpr uint32_t programMajor = 1;
	inline constexpr uint32_t programMinor = 0;
	inline constexpr uint32_t programPatch = 0;

	inline constexpr uint32_t snapshotFormat = 1;
}