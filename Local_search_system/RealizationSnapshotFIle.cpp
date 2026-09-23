#pragma once
#include "ClassSnapshotFile.h"
#include "functions.h"

void SnapshotFile::parseFile(const std::filesystem::path& filePath, uint32_t indFile)
{
    std::ifstream myFile(filePath);

    try
    {
        if (!myFile.is_open())
        {
            fileData[indFile].bad = true;
            return;
        }

        myFile.exceptions(std::ios::badbit);

        char c;
        std::string currStr;
        uint32_t countLines = 1;

        auto processWord =
            [&, indFile](const std::string& word, uint32_t line)
            {
                auto it = invertedIndex.find(word);

                if (it == invertedIndex.end())
                {
                    invertedIndex.emplace(word, std::vector<MatchInfo>{MatchInfo(indFile, line)});

                    return;
                }

                auto& matches = it->second;

                if (!matches.empty() &&
                    matches.back().docId == indFile)
                {
                    ++matches.back().count;
                    matches.back().lines.push_back(line);
                }
                else
                {
                    matches.emplace_back(indFile, line);
                }
            };

        while (myFile.get(c))
        {
            if (!isSeparator(c))
            {
                currStr.push_back(c);
            }
            else
            {
                if (currStr.size() != 0)
                {
                    processWord(currStr, countLines);

                    currStr.clear();
                }

                if (c == '\n')
                {
                    countLines++;
                }
            }
        }
        if (currStr.size() != 0)
        {
            processWord(currStr, countLines);
        }
    }
    catch (const std::ios_base::failure& ext)
    {
        fileData[indFile].bad = true;

        for (auto begin = invertedIndex.begin(), end = invertedIndex.end(); begin != end; begin++)
        {
            if (begin->second.back().docId == indFile)
            {
                begin->second.pop_back();
            }

            if (begin->second.empty())
            {
                invertedIndex.erase(begin);
            }
        }
    }
}

void SnapshotFile::parseFileSpace(const std::vector<std::filesystem::path>& files)
{
    uint32_t indFile = 0;

    for (const auto& path : files)
    {
        fileData.push_back(FileInfo(path, false, std::filesystem::last_write_time(path), std::filesystem::file_size(path)));
        pathToDocId.emplace(path, indFile);

        this->parseFile(path, indFile);

        indFile++;
    }
}

SnapshotFile::SnapshotFile(const std::filesystem::path& path, bool recursive, const std::unordered_set<std::string>& extensions)
{
    this->parseFileSpace(findFiles(path, recursive, extensions));
}

SnapshotFile::SnapshotFile(const std::vector<std::filesystem::path>& files)
{
    this->parseFileSpace(files);
}