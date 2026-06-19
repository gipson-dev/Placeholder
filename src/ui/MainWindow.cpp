#include "MainWindow.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <winspool.h>

#include "LabelPrinterApp/TemplateStorage.h"
#include "LabelPrinterApp/ZebraPrinter.h"
#include "LabelPrinterApp/ZplGenerator.h"

MainWindow::MainWindow()
    : templatePath(ResolveTemplatePath()),
      labelTemplate(TemplateStorage::LoadFromFile(templatePath))
{
}

int MainWindow::Show(HINSTANCE instance, int commandShow)
{
    if (!RegisterWindowClass(instance) || !CreateMainWindow(instance, commandShow))
    {
        return 1;
    }

    MSG message = {};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}

bool MainWindow::RegisterWindowClass(HINSTANCE instance)
{
    previewWidget.RegisterWindowClass(instance);

    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = MainWindow::WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = L"LabelPrinterMainWindow";
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    return RegisterClassW(&windowClass) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool MainWindow::CreateMainWindow(HINSTANCE instance, int commandShow)
{
    windowHandle = CreateWindowExW(
        0,
        L"LabelPrinterMainWindow",
        L"Label Printer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        940,
        640,
        nullptr,
        nullptr,
        instance,
        this);

    if (!windowHandle)
    {
        return false;
    }

    ShowWindow(windowHandle, commandShow);
    UpdateWindow(windowHandle);
    return true;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    MainWindow* mainWindow = nullptr;

    if (message == WM_NCCREATE)
    {
        auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        mainWindow = static_cast<MainWindow*>(createStruct->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mainWindow));
        mainWindow->windowHandle = window;
    }
    else
    {
        mainWindow = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    }

    if (!mainWindow)
    {
        return DefWindowProcW(window, message, wParam, lParam);
    }

    switch (message)
    {
    case WM_CREATE:
        mainWindow->CreateControls();
        return 0;

    case WM_SIZE:
        mainWindow->LayoutControls(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == PrintButtonId)
        {
            mainWindow->PrintTemplate();
            return 0;
        }
        if (LOWORD(wParam) == SaveButtonId)
        {
            mainWindow->SaveTemplate();
            return 0;
        }
        if (LOWORD(wParam) >= FirstInputId && HIWORD(wParam) == EN_CHANGE)
        {
            mainWindow->SyncInputsToTemplate();
            return 0;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(window, message, wParam, lParam);
    }
}

void MainWindow::CreateControls()
{
    printerLabel = CreateWindowExW(0, L"STATIC", L"Printer", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, windowHandle, nullptr, nullptr, nullptr);
    printerCombo = CreateWindowExW(0, L"COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 0, 0, 0, 0, windowHandle, reinterpret_cast<HMENU>(static_cast<INT_PTR>(PrinterComboId)), nullptr, nullptr);
    printButton = CreateWindowExW(0, L"BUTTON", L"Print", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, windowHandle, reinterpret_cast<HMENU>(static_cast<INT_PTR>(PrintButtonId)), nullptr, nullptr);
    saveButton = CreateWindowExW(0, L"BUTTON", L"Save Template", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, windowHandle, reinterpret_cast<HMENU>(static_cast<INT_PTR>(SaveButtonId)), nullptr, nullptr);

    previewHandle = previewWidget.Create(windowHandle, labelTemplate);

    PopulatePrinters();
    CreateElementInputs();

    RECT rect = {};
    GetClientRect(windowHandle, &rect);
    LayoutControls(rect.right - rect.left, rect.bottom - rect.top);
}

void MainWindow::LayoutControls(int width, int height)
{
    const int margin = 18;
    const int labelWidth = 110;
    const int editWidth = 300;
    const int rowHeight = 28;
    const int rowGap = 10;
    const int leftWidth = margin + labelWidth + editWidth + margin;
    const int previewX = leftWidth + margin;
    const int previewWidth = std::max(260, width - previewX - margin);
    const int previewHeight = std::max(220, height - (margin * 2));

    MoveWindow(printerLabel, margin, margin + 4, labelWidth, rowHeight, TRUE);
    MoveWindow(printerCombo, margin + labelWidth, margin, editWidth, 200, TRUE);

    int y = margin + rowHeight + 18;
    for (const ElementInput& input : elementInputs)
    {
        MoveWindow(input.labelHandle, margin, y + 4, labelWidth, rowHeight, TRUE);
        MoveWindow(input.editHandle, margin + labelWidth, y, editWidth, rowHeight, TRUE);
        y += rowHeight + rowGap;
    }

    MoveWindow(printButton, margin + labelWidth, height - margin - 34, 120, 34, TRUE);
    MoveWindow(saveButton, margin + labelWidth + 132, height - margin - 34, 168, 34, TRUE);
    MoveWindow(previewHandle, previewX, margin, previewWidth, previewHeight, TRUE);
}

void MainWindow::PopulatePrinters()
{
    DWORD bytesNeeded = 0;
    DWORD printerCount = 0;
    EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2, nullptr, 0, &bytesNeeded, &printerCount);

    if (bytesNeeded == 0)
    {
        SendMessageW(printerCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"No printers found"));
        SendMessageW(printerCombo, CB_SETCURSEL, 0, 0);
        return;
    }

    std::vector<BYTE> buffer(bytesNeeded);
    if (!EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2, buffer.data(), bytesNeeded, &bytesNeeded, &printerCount))
    {
        SendMessageW(printerCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Unable to load printers"));
        SendMessageW(printerCombo, CB_SETCURSEL, 0, 0);
        return;
    }

    auto* printers = reinterpret_cast<PRINTER_INFO_2W*>(buffer.data());
    for (DWORD i = 0; i < printerCount; ++i)
    {
        SendMessageW(printerCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(printers[i].pPrinterName));
    }

    if (printerCount > 0)
    {
        SendMessageW(printerCombo, CB_SETCURSEL, 0, 0);
    }
}

void MainWindow::CreateElementInputs()
{
    elementInputs.clear();

    for (std::size_t i = 0; i < labelTemplate.elements.size(); ++i)
    {
        const LabelElement& element = labelTemplate.elements[i];
        std::wstring label = ToWide(element.name.empty() ? "Element" : element.name);
        std::wstring value = ToWide(element.text);

        ElementInput input;
        input.elementIndex = i;
        input.labelHandle = CreateWindowExW(0, L"STATIC", label.c_str(), WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, windowHandle, nullptr, nullptr, nullptr);
        input.editHandle = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            value.c_str(),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            0,
            0,
            0,
            0,
            windowHandle,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(FirstInputId + i)),
            nullptr,
            nullptr);

        elementInputs.push_back(input);
    }
}

void MainWindow::SyncInputsToTemplate()
{
    for (const ElementInput& input : elementInputs)
    {
        if (input.elementIndex < labelTemplate.elements.size())
        {
            labelTemplate.elements[input.elementIndex].text = ToNarrow(GetWindowTextValue(input.editHandle));
        }
    }

    previewWidget.SetTemplate(labelTemplate);
}

void MainWindow::PrintTemplate()
{
    SyncInputsToTemplate();

    int selected = static_cast<int>(SendMessageW(printerCombo, CB_GETCURSEL, 0, 0));
    if (selected == CB_ERR)
    {
        ShowMessage(L"Select a printer before printing.", L"Printer Required", MB_ICONWARNING);
        return;
    }

    wchar_t printerName[512] = {};
    SendMessageW(printerCombo, CB_GETLBTEXT, selected, reinterpret_cast<LPARAM>(printerName));

    ZebraPrinter printer;
    printer.setPrinterName(printerName);

    std::string error;
    std::string zpl = ZplGenerator::generate(labelTemplate, printerSettings);
    if (printer.printZpl(zpl, error))
    {
        ShowMessage(L"Label sent to printer.", L"Print Complete", MB_ICONINFORMATION);
    }
    else
    {
        ShowMessage(ToWide(error), L"Print Failed", MB_ICONERROR);
    }
}

void MainWindow::SaveTemplate()
{
    SyncInputsToTemplate();
    if (TemplateStorage::SaveToFile(labelTemplate, templatePath))
    {
        ShowMessage(L"Template saved.", L"Save Complete", MB_ICONINFORMATION);
    }
    else
    {
        ShowMessage(L"Template could not be saved.", L"Save Failed", MB_ICONERROR);
    }
}

void MainWindow::ShowMessage(const std::wstring& message, const std::wstring& title, UINT icon)
{
    MessageBoxW(windowHandle, message.c_str(), title.c_str(), MB_OK | icon);
}

std::wstring MainWindow::ToWide(const std::string& value)
{
    if (value.empty())
    {
        return L"";
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    std::wstring output(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, output.data(), length);
    output.resize(length - 1);
    return output;
}

std::string MainWindow::ToNarrow(const std::wstring& value)
{
    if (value.empty())
    {
        return "";
    }

    int length = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string output(length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, output.data(), length, nullptr, nullptr);
    output.resize(length - 1);
    return output;
}

std::wstring MainWindow::GetWindowTextValue(HWND window)
{
    int length = GetWindowTextLengthW(window);
    std::wstring value(length + 1, L'\0');
    GetWindowTextW(window, value.data(), length + 1);
    value.resize(length);
    return value;
}

std::string MainWindow::ResolveTemplatePath()
{
    namespace fs = std::filesystem;

    std::vector<fs::path> searchRoots;
    searchRoots.push_back(fs::current_path());

    wchar_t modulePath[MAX_PATH] = {};
    if (GetModuleFileNameW(nullptr, modulePath, MAX_PATH) > 0)
    {
        searchRoots.push_back(fs::path(modulePath).parent_path());
    }

    for (fs::path root : searchRoots)
    {
        for (int depth = 0; depth < 5 && !root.empty(); ++depth)
        {
            fs::path candidate = root / "templates" / "default_label.json";
            if (fs::exists(candidate))
            {
                return candidate.string();
            }

            root = root.parent_path();
        }
    }

    return "templates/default_label.json";
}
