#pragma once

#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "LabelPrinterApp/LabelTemplate.h"
#include "LabelPrinterApp/PrinterSettings.h"
#include "PreviewWidget.h"

class MainWindow
{
public:
    MainWindow();

    int Show(HINSTANCE instance, int commandShow);

private:
    struct ElementInput
    {
        std::size_t elementIndex = 0;
        HWND labelHandle = nullptr;
        HWND editHandle = nullptr;
    };

    static constexpr int PrinterComboId = 1001;
    static constexpr int PrintButtonId = 1002;
    static constexpr int SaveButtonId = 1003;
    static constexpr int FirstInputId = 2000;

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    bool RegisterWindowClass(HINSTANCE instance);
    bool CreateMainWindow(HINSTANCE instance, int commandShow);

    void CreateControls();
    void LayoutControls(int width, int height);
    void PopulatePrinters();
    void CreateElementInputs();
    void SyncInputsToTemplate();
    void PrintTemplate();
    void SaveTemplate();
    void ShowMessage(const std::wstring& message, const std::wstring& title, UINT icon);

    static std::wstring ToWide(const std::string& value);
    static std::string ToNarrow(const std::wstring& value);
    static std::wstring GetWindowTextValue(HWND window);
    static std::string ResolveTemplatePath();

    HWND windowHandle = nullptr;
    HWND printerLabel = nullptr;
    HWND printerCombo = nullptr;
    HWND printButton = nullptr;
    HWND saveButton = nullptr;

    PreviewWidget previewWidget;
    HWND previewHandle = nullptr;

    LabelTemplate labelTemplate;
    PrinterSettings printerSettings;
    std::vector<ElementInput> elementInputs;
    std::string templatePath;
};
