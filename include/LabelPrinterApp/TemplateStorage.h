#pragma once

#include <fstream>
#include <regex>
#include <sstream>
#include <string>

#include "LabelTemplate.h"

class TemplateStorage
{
public:
    static LabelTemplate LoadFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            return LoadDefaultTemplate();
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        LabelTemplate label = ParseTemplate(buffer.str());

        if (label.elements.empty())
        {
            return LoadDefaultTemplate();
        }

        return label;
    }

    static bool SaveToFile(const LabelTemplate& label, const std::string& path)
    {
        std::ofstream file(path, std::ios::trunc);
        if (!file)
        {
            return false;
        }

        file << "{\n";
        file << "  \"name\": \"" << EscapeJson(label.name) << "\",\n";
        file << "  \"labelWidthDots\": " << label.labelWidthDots << ",\n";
        file << "  \"labelHeightDots\": " << label.labelHeightDots << ",\n";
        file << "  \"elements\": [\n";

        for (std::size_t i = 0; i < label.elements.size(); ++i)
        {
            const LabelElement& element = label.elements[i];
            file << "    {\n";
            file << "      \"name\": \"" << EscapeJson(element.name) << "\",\n";
            file << "      \"type\": \"" << (element.type == LabelElementType::Barcode ? "Barcode" : "Text") << "\",\n";
            file << "      \"text\": \"" << EscapeJson(element.text) << "\",\n";
            file << "      \"x\": " << element.x << ",\n";
            file << "      \"y\": " << element.y;

            if (element.type == LabelElementType::Barcode)
            {
                file << ",\n";
                file << "      \"barcodeHeight\": " << element.barcodeHeight << "\n";
            }
            else
            {
                file << ",\n";
                file << "      \"fontHeight\": " << element.fontHeight << ",\n";
                file << "      \"fontWidth\": " << element.fontWidth << ",\n";
                file << "      \"bold\": " << (element.bold ? "true" : "false") << "\n";
            }

            file << "    }" << (i + 1 == label.elements.size() ? "\n" : ",\n");
        }

        file << "  ]\n";
        file << "}\n";
        return true;
    }

    static LabelTemplate LoadDefaultTemplate()
    {
        LabelTemplate label;
        label.name = "Default Label";
        label.labelWidthDots = 400;
        label.labelHeightDots = 240;

        label.addElement(LabelElement::Text("Item Number", "ITEM: 123456", 30, 30, 35, 35, true));
        label.addElement(LabelElement::Text("Part Name", "PART A-100", 30, 80, 30, 30));
        label.addElement(LabelElement::Text("Quantity", "QTY: 25", 30, 125, 30, 30));
        label.addElement(LabelElement::Barcode("Barcode", "123456", 30, 170, 50));

        return label;
    }

private:
    static LabelTemplate ParseTemplate(const std::string& json)
    {
        LabelTemplate label;
        label.name = MatchString(json, "name", label.name);
        label.labelWidthDots = MatchInt(json, "labelWidthDots", label.labelWidthDots);
        label.labelHeightDots = MatchInt(json, "labelHeightDots", label.labelHeightDots);

        std::size_t elementsPos = json.find("\"elements\"");
        std::size_t arrayStart = json.find('[', elementsPos);
        std::size_t arrayEnd = json.rfind(']');
        if (arrayStart == std::string::npos || arrayEnd == std::string::npos || arrayStart >= arrayEnd)
        {
            return label;
        }

        std::string elementsJson = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
        std::regex objectRegex("\\{([^{}]*)\\}");
        auto begin = std::sregex_iterator(elementsJson.begin(), elementsJson.end(), objectRegex);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it)
        {
            std::string object = (*it)[1].str();
            std::string type = MatchString(object, "type", "Text");
            std::string name = MatchString(object, "name", MatchString(object, "text", "Element"));
            std::string text = MatchString(object, "text", "");
            int x = MatchInt(object, "x", 20);
            int y = MatchInt(object, "y", 20);

            if (type == "Barcode")
            {
                label.addElement(LabelElement::Barcode(name, text, x, y, MatchInt(object, "barcodeHeight", 60)));
            }
            else
            {
                label.addElement(LabelElement::Text(
                    name,
                    text,
                    x,
                    y,
                    MatchInt(object, "fontHeight", 30),
                    MatchInt(object, "fontWidth", 30),
                    MatchBool(object, "bold", false)));
            }
        }

        return label;
    }

    static std::string MatchString(const std::string& json, const std::string& key, const std::string& fallback)
    {
        std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
        std::smatch match;
        return std::regex_search(json, match, pattern) ? UnescapeJson(match[1].str()) : fallback;
    }

    static int MatchInt(const std::string& json, const std::string& key, int fallback)
    {
        std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
        std::smatch match;
        return std::regex_search(json, match, pattern) ? std::stoi(match[1].str()) : fallback;
    }

    static bool MatchBool(const std::string& json, const std::string& key, bool fallback)
    {
        std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
        std::smatch match;
        return std::regex_search(json, match, pattern) ? match[1].str() == "true" : fallback;
    }

    static std::string EscapeJson(const std::string& value)
    {
        std::string escaped;
        for (char c : value)
        {
            if (c == '\\' || c == '"')
            {
                escaped += '\\';
            }
            escaped += c;
        }
        return escaped;
    }

    static std::string UnescapeJson(const std::string& value)
    {
        std::string unescaped;
        bool escaping = false;
        for (char c : value)
        {
            if (escaping)
            {
                unescaped += c;
                escaping = false;
            }
            else if (c == '\\')
            {
                escaping = true;
            }
            else
            {
                unescaped += c;
            }
        }
        return unescaped;
    }
};
