#pragma once

#include <map>
#include <string>

#include "core/LabelElement.h"

struct VariableContext
{
    std::map<std::string, std::string> values;
    int serialNumber = 0;
    int recordIndex = 0;
};

class VariableResolver
{
public:
    static std::string resolveText(const std::string& input, const VariableContext& context);
    static std::string elementValue(const LabelElement& element, const VariableContext& context);
    static std::map<std::string, std::string> findPlaceholders(const std::string& text);

private:
    static std::string builtInValue(const std::string& name, const VariableContext& context);
    static std::string formatSerial(int value, int width);
};
