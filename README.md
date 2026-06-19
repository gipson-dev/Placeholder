# LabelPrinterApp

LabelPrinterApp is a small Windows C++ app for editing Zebra Programming Language (ZPL) label values, previewing the label, saving the template, and sending it to a Zebra printer through the WinSpool RAW print path.

The current UI loads the default JSON template, creates input fields from each element name, draws a live preview, lists installed Windows printers, and prints the generated ZPL to the selected printer.

## Project Layout

```text
include/LabelPrinterApp/  Public app headers and label model types
src/                      Application entry point and printer implementation
src/ui/                   Win32 main window and preview widget
templates/                JSON template examples
.github/workflows/        GitHub Actions build checks
```

## Requirements

- Windows
- Visual Studio 2022 or newer with MSVC C++ tools
- CMake 3.20 or newer, if using the CMake build path
- Installed Zebra printer driver configured for ZPL/RAW printing

## Build

From a Developer PowerShell or Developer Command Prompt with MSVC on `PATH`:

```powershell
cl.exe /Zi /EHsc /nologo /std:c++17 /Iinclude /FeLabelPrinterApp.exe src/main.cpp src/ZebraPrinter.cpp src/ui/MainWindow.cpp src/ui/PreviewWidget.cpp Gdi32.lib User32.lib Winspool.lib /link /SUBSYSTEM:WINDOWS
```

Or use CMake:

```powershell
cmake -S . -B build
cmake --build build
```

## Run

Run the app and select the installed Windows printer from the printer list:

```powershell
.\LabelPrinterApp.exe
```

## Current UI

- Printer selection from installed Windows printers
- Dynamic input fields generated from `templates/default_label.json`
- Live label preview for text and barcode elements
- Print button that sends generated ZPL to the selected printer
- Save Template button that writes edited values back to JSON

## Label Templates

The in-code fallback label lives in `include/LabelPrinterApp/TemplateStorage.h`.

`templates/default_label.json` is loaded at startup and saved when the Save Template button is clicked. Element names in that JSON file become the editable input labels in the UI. Element types currently supported by the generator are:

- `Text`
- `Barcode`

## Notes

- Generated build artifacts such as `.exe`, `.obj`, `.pdb`, and `.ilk` files are ignored.
- Printer failures include the Windows error code where available.
