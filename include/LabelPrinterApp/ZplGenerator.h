#pragma once

#include <sstream>
#include <string>
#include "LabelTemplate.h"
#include "PrinterSettings.h"

class ZplGenerator
{
public:
    static std::string generate(
        const LabelTemplate& labelTemplate,
        const PrinterSettings& settings
    )
    {
        std::ostringstream zpl;

        zpl << "^XA\n";

        const int widthDots = labelTemplate.labelWidthDots > 0
            ? labelTemplate.labelWidthDots
            : settings.labelWidthDots;
        const int heightDots = labelTemplate.labelHeightDots > 0
            ? labelTemplate.labelHeightDots
            : settings.labelHeightDots;

        zpl << "^PW" << widthDots << "\n";
        zpl << "^LL" << heightDots << "\n";

        zpl << "^MD" << settings.darkness << "\n";
        zpl << "^PR" << settings.printSpeed << "\n";

        for (const auto& element : labelTemplate.elements)
        {
            zpl << "^FO" << element.x << "," << element.y << "\n";

            if (element.type == LabelElementType::Barcode)
            {
                zpl << "^BY2\n";
                zpl << "^BCN," << element.barcodeHeight << ",Y,N,N\n";
                zpl << "^FD" << sanitizeText(element.text) << "^FS\n";
            }
            else if (element.bold)
            {
                // Fake bold by printing the same text twice with slight offset
                zpl << "^A0N," << element.fontHeight << "," << element.fontWidth << "\n";
                zpl << "^FD" << sanitizeText(element.text) << "^FS\n";

                zpl << "^FO" << element.x + 2 << "," << element.y << "\n";
                zpl << "^A0N," << element.fontHeight << "," << element.fontWidth << "\n";
                zpl << "^FD" << sanitizeText(element.text) << "^FS\n";
            }
            else
            {
                zpl << "^A0N," << element.fontHeight << "," << element.fontWidth << "\n";
                zpl << "^FD" << sanitizeText(element.text) << "^FS\n";
            }
        }

        zpl << "^XZ\n";

        return zpl.str();
    }

private:
    static std::string sanitizeText(const std::string& input)
    {
        std::string output;

        for (char c : input)
        {
            // Avoid breaking ZPL field data
            if (c == '^' || c == '~')
            {
                output += ' ';
            }
            else
            {
                output += c;
            }
        }

        return output;
    }
}; 
