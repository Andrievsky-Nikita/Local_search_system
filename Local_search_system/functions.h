#pragma once
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

bool isSupportedVersionSnapshot(uint32_t version);
bool isSeparator(char c);
std::vector<std::filesystem::path> findFiles(const std::filesystem::path& pathDir, bool recursive, const std::unordered_set<std::string>& extensions);