#include "core/CsvImporter.h"

#include <cctype>
#include <fstream>
#include <sstream>

std::map<std::string, std::string> CsvData::rowAsVariables(std::size_t rowIndex, const std::map<std::string, std::string>& mapping) const
{
    std::map<std::string, std::string> variables;
    if (rowIndex >= rows.size())
    {
        return variables;
    }

    const auto& row = rows[rowIndex];
    for (const auto& mapEntry : mapping)
    {
        const std::string& placeholder = mapEntry.first;
        const std::string& header = mapEntry.second;
        for (std::size_t i = 0; i < headers.size(); ++i)
        {
            if (headers[i] == header && i < row.size())
            {
                variables[placeholder] = row[i];
                break;
            }
        }
    }

    return variables;
}

CsvData CsvImporter::loadFile(const std::string& path, bool autoDetectHeader)
{
    std::ifstream file(path);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return parse(buffer.str(), autoDetectHeader);
}

CsvData CsvImporter::parse(const std::string& content, bool autoDetectHeader)
{
    std::istringstream input(content);
    std::string line;
    std::vector<std::vector<std::string>> parsedRows;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (!line.empty())
        {
            parsedRows.push_back(parseLine(line));
        }
    }

    CsvData data;
    if (parsedRows.empty())
    {
        return data;
    }

    data.hasHeader = autoDetectHeader && parsedRows.size() > 1 && looksLikeHeader(parsedRows[0], parsedRows[1]);
    if (data.hasHeader)
    {
        data.headers = parsedRows.front();
        data.rows.assign(parsedRows.begin() + 1, parsedRows.end());
    }
    else
    {
        data.rows = parsedRows;
        for (std::size_t i = 0; i < parsedRows.front().size(); ++i)
        {
            data.headers.push_back("Column" + std::to_string(i + 1));
        }
    }

    return data;
}

std::vector<std::string> CsvImporter::parseLine(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;

    for (std::size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (quoted)
        {
            if (c == '"' && i + 1 < line.size() && line[i + 1] == '"')
            {
                field += '"';
                ++i;
            }
            else if (c == '"')
            {
                quoted = false;
            }
            else
            {
                field += c;
            }
        }
        else if (c == '"')
        {
            quoted = true;
        }
        else if (c == ',')
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}

bool CsvImporter::looksLikeHeader(const std::vector<std::string>& firstRow, const std::vector<std::string>& secondRow)
{
    if (firstRow.empty() || firstRow.size() != secondRow.size())
    {
        return false;
    }

    int headerish = 0;
    for (const std::string& field : firstRow)
    {
        if (!field.empty() && !std::isdigit(static_cast<unsigned char>(field[0])))
        {
            ++headerish;
        }
    }
    return headerish >= static_cast<int>(firstRow.size() / 2 + 1);
}
