#pragma once
#include "functions.h"

std::vector<std::filesystem::path> findFiles(const std::filesystem::path& pathDir, bool recursive, const std::unordered_set<std::string>& extensions)
{
    std::vector<std::filesystem::path> files;

    namespace fs = std::filesystem;

    if (!fs::exists(pathDir))
    {
        throw std::invalid_argument("Введенный путь некорректен. По пути: " + pathDir.string() + " не существует объекта файловой системы");
    }

    if (fs::is_regular_file(pathDir))
    {
        files.push_back(pathDir);
        return files;
    }
    else if (fs::is_directory(pathDir))
    {
        auto generatorArrayFiles = [&]<typename Iterator>(Iterator it)
        {
            for (const auto& entry : it)
            {
                if (entry.is_regular_file() && extensions.contains(entry.path().extension().string()))
                {
                    files.push_back(entry.path());
                }
            }

            return files;
        };

        if (recursive)
        {
            return generatorArrayFiles(fs::recursive_directory_iterator(pathDir));
        }
        else
        {
            return generatorArrayFiles(fs::directory_iterator(pathDir));
        }
    }
    else
    {
        throw std::invalid_argument("Введенный путь некорректен. Объект по пути: " + pathDir.string() + " не является файлом или директорией");
    }
}

bool isSeparator(char c)
{
    return c == ' ' ||
        c == '\n' ||
        c == '\t' ||
        c == '.' ||
        c == ',' ||
        c == '!' ||
        c == '?' ||
        c == ';' ||
        c == ':' ||
        c == '"' ||
        c == '(' ||
        c == ')' ||
        c == '{' ||
        c == '}' ||
        c == '\\' ||
        c == '/' ||
        c == '_' ||
        c == '\r';
}

bool isSupportedVersionSnapshot(uint32_t version)
{
    switch (version)
    {
    case 1:
        return true;
    default:
        return false;
    }
}