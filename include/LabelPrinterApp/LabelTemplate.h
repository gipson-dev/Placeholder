#pragma once

#include <string>
#include <vector>
#include "LabelElement.h"

struct LabelTemplate
{
    std::string name = "Default Label";

    int labelWidthDots = 609;
    int labelHeightDots = 203;

    std::vector<LabelElement> elements;

    void addElement(const LabelElement& element)
    {
        elements.push_back(element);
    }

    void clear()
    {
        elements.clear();
    }
};