#include "core/ZebraPrinter.h"

#include <windows.h>
#include <winspool.h>

#include <vector>

namespace
{
    std::string windowsError(const std::string& message)
    {
        return message + " Windows error code: " + std::to_string(GetLastError()) + ".";
    }
}

std::vector<std::string> ZebraPrinter::installedPrinters(std::string& errorMessage)
{
    errorMessage.clear();
    DWORD bytesNeeded = 0;
    DWORD count = 0;
    EnumPrintersA(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2, nullptr, 0, &bytesNeeded, &count);
    if (bytesNeeded == 0)
    {
        return {};
    }

    std::vector<BYTE> buffer(bytesNeeded);
    if (!EnumPrintersA(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2, buffer.data(), bytesNeeded, &bytesNeeded, &count))
    {
        errorMessage = windowsError("Failed to enumerate printers.");
        return {};
    }

    std::vector<std::string> printers;
    auto* info = reinterpret_cast<PRINTER_INFO_2A*>(buffer.data());
    for (DWORD i = 0; i < count; ++i)
    {
        if (info[i].pPrinterName)
        {
            printers.emplace_back(info[i].pPrinterName);
        }
    }
    return printers;
}

bool ZebraPrinter::printRawZpl(const std::string& printerName, const std::string& zpl, std::string& errorMessage)
{
    errorMessage.clear();
    if (printerName.empty())
    {
        errorMessage = "Printer name is empty.";
        return false;
    }
    if (zpl.empty())
    {
        errorMessage = "ZPL data is empty.";
        return false;
    }

    HANDLE printer = nullptr;
    if (!OpenPrinterA(const_cast<LPSTR>(printerName.c_str()), &printer, nullptr))
    {
        errorMessage = windowsError("Failed to open printer.");
        return false;
    }

    DOC_INFO_1A docInfo{};
    docInfo.pDocName = const_cast<LPSTR>("LabelPrinterApp ZPL Job");
    docInfo.pDatatype = const_cast<LPSTR>("RAW");

    bool ok = false;
    if (StartDocPrinterA(printer, 1, reinterpret_cast<LPBYTE>(&docInfo)) != 0)
    {
        if (StartPagePrinter(printer))
        {
            DWORD written = 0;
            ok = WritePrinter(printer, const_cast<char*>(zpl.data()), static_cast<DWORD>(zpl.size()), &written) && written == zpl.size();
            EndPagePrinter(printer);
        }
        EndDocPrinter(printer);
    }

    if (!ok)
    {
        errorMessage = windowsError("Failed to write raw ZPL to printer.");
    }

    ClosePrinter(printer);
    return ok;
}
