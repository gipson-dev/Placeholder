#pragma once

#include <string>
#include <vector>

#include "core/LabelElement.h"
#include "core/PrinterSettings.h"

struct LabelTemplate
{
    std::string name = "Blank 2.25 x 0.75 Label";
    PrinterSettings settings;
    std::vector<LabelElement> elements;

    static LabelTemplate defaultTemplate()
    {
        LabelTemplate label;
        label.settings.dpi = 203;
        label.settings.labelWidthInches = 2.25;
        label.settings.labelHeightInches = 0.75;
        label.settings.gapInches = 0.125;
        label.settings.orientation = LabelOrientation::Landscape;

        return label;
    }
};
