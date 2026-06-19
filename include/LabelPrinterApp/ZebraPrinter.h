#pragma once

#include <string>

class ZebraPrinter
{
public:
    ZebraPrinter();

    void setPrinterName(const std::wstring& printerName);
    std::wstring getPrinterName() const;

    bool printZpl(const std::string& zpl, std::string& errorMessage);

private:
    std::wstring printerName;
};