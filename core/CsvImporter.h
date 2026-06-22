#pragma once

#include <map>
#include <string>
#include <vector>

struct CsvData
{
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    bool hasHeader = true;

    std::map<std::string, std::string> rowAsVariables(std::size_t rowIndex, const std::map<std::string, std::string>& mapping) const;
};

class CsvImporter
{
public:
    static CsvData loadFile(const std::string& path, bool autoDetectHeader = true);
    static CsvData parse(const std::string& content, bool autoDetectHeader = true);

private:
    static std::vector<std::string> parseLine(const std::string& line);
    static bool looksLikeHeader(const std::vector<std::string>& firstRow, const std::vector<std::string>& secondRow);
};
