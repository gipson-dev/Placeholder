#include <iostream>
#include <string>

#include "LabelPrinterApp/PrinterSettings.h"
#include "LabelPrinterApp/TemplateStorage.h"
#include "LabelPrinterApp/ZebraPrinter.h"
#include "LabelPrinterApp/ZplGenerator.h"

int main()
{
    ZebraPrinter printer;
    PrinterSettings settings;
    LabelTemplate labelTemplate = TemplateStorage::LoadDefaultTemplate();

    // CHANGE THIS to your exact Windows printer name
    printer.setPrinterName(L"ZDesigner ZD410-203dpi ZPL");

    std::string zpl = ZplGenerator::generate(labelTemplate, settings);
    std::string error;

    if (printer.printZpl(zpl, error))
    {
        std::cout << "Label sent to printer successfully.\n";
    }
    else
    {
        std::cout << "Print failed: " << error << "\n";
    }

    system("pause");
    return 0;
}
