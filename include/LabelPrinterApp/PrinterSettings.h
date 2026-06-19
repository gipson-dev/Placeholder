#pragma once

#include <string>

struct PrinterSettings
{
    std::string printerName = "Zebra ZD410";

    int dpi = 203;

    int darkness = 15;
    int printSpeed = 4;

    int labelWidthDots = 609;
    int labelHeightDots = 203;
};