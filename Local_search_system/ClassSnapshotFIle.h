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

class SnapshotFile
{
private:

    struct MatchInfo;
    struct FileInfo;

    std::unordered_map<std::string, std::vector<MatchInfo>> invertedIndex;
    std::unordered_map<std::filesystem::path, uint32_t> pathToDocId;
    std::vector<FileInfo> fileData;

    void parseFile(const std::filesystem::path& filePath, uint32_t indFile);
    void parseFileSpace(const std::vector<std::filesystem::path>& files);

public:

    struct SearchResult;

    SnapshotFile(const std::filesystem::path& pathDir, bool recursive, const std::unordered_set<std::string>& extensions);
    SnapshotFile(const std::vector<std::filesystem::path>& files);

    void saveSnapshotFile(const std::filesystem::path& pathToSave);
};

struct SnapshotFile::FileInfo
{
    std::filesystem::path path;
    bool bad;
    std::filesystem::file_time_type lastWriteTime;
    std::uintmax_t size;

    FileInfo(std::filesystem::path newPath,
        bool newBad,
        std::filesystem::file_time_type newLastWriteTime,
        std::uintmax_t newSize) :
        path(newPath),
        bad(newBad),
        lastWriteTime(newLastWriteTime),
        size(newSize)
    {}
};

struct SnapshotFile::MatchInfo
{
    uint32_t docId;
    uint32_t count;
    std::vector<uint32_t> lines;

    MatchInfo(uint32_t doc, uint32_t line) :
        docId(doc),
        count(1),
        lines(std::vector<uint32_t> {line})
    {}
};

struct SnapshotFile::SearchResult
{
    std::string filePath;
    uint32_t occurrences;
    std::vector<uint32_t> lines;
};
