#include "LabelPrinterApp/ZebraPrinter.h"

#include <windows.h>
#include <winspool.h>
#include <string>

#pragma comment(lib, "Winspool.lib")

ZebraPrinter::ZebraPrinter()
{
}

namespace
{
    std::string formatLastError(const std::string& message)
    {
        return message + " Windows error code: " + std::to_string(GetLastError()) + ".";
    }
}

void ZebraPrinter::setPrinterName(const std::wstring& printerName)
{
    this->printerName = printerName;
}

std::wstring ZebraPrinter::getPrinterName() const
{
    return printerName;
}

bool ZebraPrinter::printZpl(const std::string& zpl, std::string& errorMessage)
{
    if (printerName.empty())
    {
        errorMessage = "Printer name is empty.";
        return false;
    }

    HANDLE printerHandle = nullptr;

    if (!OpenPrinterW(
            const_cast<LPWSTR>(printerName.c_str()),
            &printerHandle,
            nullptr))
    {
        errorMessage = formatLastError("Failed to open printer.");
        return false;
    }

    DOC_INFO_1W docInfo;
    docInfo.pDocName = const_cast<LPWSTR>(L"ZPL Label Print Job");
    docInfo.pOutputFile = nullptr;
    docInfo.pDatatype = const_cast<LPWSTR>(L"RAW");

    DWORD jobId = StartDocPrinterW(printerHandle, 1, reinterpret_cast<LPBYTE>(&docInfo));

    if (jobId == 0)
    {
        errorMessage = formatLastError("Failed to start print document.");
        ClosePrinter(printerHandle);
        return false;
    }

    if (!StartPagePrinter(printerHandle))
    {
        errorMessage = formatLastError("Failed to start printer page.");
        EndDocPrinter(printerHandle);
        ClosePrinter(printerHandle);
        return false;
    }

    DWORD bytesWritten = 0;
    DWORD bytesToWrite = static_cast<DWORD>(zpl.size());

    BOOL writeResult = WritePrinter(
        printerHandle,
        const_cast<char*>(zpl.c_str()),
        bytesToWrite,
        &bytesWritten);
    DWORD writeError = writeResult ? ERROR_SUCCESS : GetLastError();

    EndPagePrinter(printerHandle);
    EndDocPrinter(printerHandle);
    ClosePrinter(printerHandle);

    if (!writeResult || bytesWritten != bytesToWrite)
    {
        errorMessage = "Failed to write ZPL data to printer.";
        if (writeError != ERROR_SUCCESS)
        {
            errorMessage += " Windows error code: " + std::to_string(writeError) + ".";
        }
        return false;
    }

    return true;
} 
