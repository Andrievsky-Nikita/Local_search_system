#pragma once
#include "ClassSnapshotFile.h"
#include "functions.h"
#include "version.h"

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
    catch (const std::ios_base::failure&)
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

void SnapshotFile::saveSnapshotFile(const std::filesystem::path& pathToSave)
{
    std::ofstream file;

    file.exceptions(std::ios::failbit | std::ios::badbit);

    try
    {
        file.open(pathToSave, std::ios::out | std::ios::binary);

        file.write("SNAP", 4);
        file.write(reinterpret_cast<const char*>(&version::snapshotFormat), sizeof(version::snapshotFormat));

        file.write(reinterpret_cast<const char*>(static_cast<std::uintmax_t>(0)), sizeof(std::uintmax_t));

        std::uint32_t fileCount = static_cast<uint32_t>(fileData.size());

        file.write(reinterpret_cast<const char*>(&fileCount), 4);

        for (const auto& fileInfo : fileData)
        {
            auto pathString = fileInfo.path.u8string();

            uint32_t sizePath = pathString.size();

            file.write(reinterpret_cast<const char*>(&sizePath), sizeof(sizePath));
            file.write(reinterpret_cast<const char*>(&pathString), sizePath);

            uint8_t flag = fileInfo.bad ? 1 : 0;

            file.write(reinterpret_cast<const char*>(&flag), sizeof(flag));

            uint64_t sizeFile = fileInfo.size;

            file.write(reinterpret_cast<const char*>(&sizeFile), sizeof(sizeFile));

            auto time = fileInfo.lastWriteTime.time_since_epoch().count();

            file.write(reinterpret_cast<const char*>(&time),sizeof(time));
        }

        using Iterator = decltype(invertedIndex)::const_iterator;

        std::vector<Iterator> sortedKeys(invertedIndex.begin(), invertedIndex.end());

        std::sort(sortedKeys.begin(), sortedKeys.end(),
            [](const auto& lhs, const auto& rhs)
            {
                return lhs->first < rhs->first;
            });

        std::uint32_t wordCount = static_cast<std::uint32_t>(invertedIndex.size());

        file.write(reinterpret_cast<const char*>(&wordCount),sizeof(wordCount));

        










        /*uint64_t pos = static_cast<uint64_t>(file.tellp());

        file.seekp(8, std::ios::beg);

        file.write(reinterpret_cast<const char*>(&pos), sizeof(pos));

        file.seekp(pos, std::ios::beg);*/
    }
    catch (const std::ios_base::failure& error)
    {
        file.exceptions(std::ios::goodbit);
        file.close();


        std::error_code err;
        std::filesystem::remove(pathToSave, err);

        if (err)
        {
            std::throw_with_nested(std::system_error(err));
        }

        throw;
    }
}