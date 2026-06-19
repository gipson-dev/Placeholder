#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "LabelPrinterApp/LabelTemplate.h"

class PreviewWidget
{
public:
    bool RegisterWindowClass(HINSTANCE instance);
    HWND Create(HWND parent, const LabelTemplate& labelTemplate);
    void SetTemplate(const LabelTemplate& labelTemplate);

private:
    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    void Paint(HWND window);
    RECT GetLabelRect(const RECT& clientRect) const;
    int ScaleX(int value, const RECT& labelRect) const;
    int ScaleY(int value, const RECT& labelRect) const;

    HWND windowHandle = nullptr;
    LabelTemplate currentTemplate;
};
