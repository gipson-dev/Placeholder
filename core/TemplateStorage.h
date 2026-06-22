#pragma once

#include <string>

#include "core/LabelTemplate.h"

class TemplateStorage
{
public:
    static LabelTemplate load(const std::string& path);
    static bool save(const LabelTemplate& labelTemplate, const std::string& path, std::string& errorMessage);
};
