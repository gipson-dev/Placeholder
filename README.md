# LabelPrinterApp

LabelPrinterApp is a small Windows C++ prototype for generating Zebra Programming Language (ZPL) labels and sending them to a Zebra printer through the WinSpool RAW print path.

The current sample builds a default label with text and barcode elements, generates ZPL, and sends it to the configured Windows printer name.

## Project Layout

```text
include/LabelPrinterApp/  Public app headers and label model types
src/                      Application entry point and printer implementation
src/ui/                   Placeholder UI classes for future interface work
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
cl.exe /Zi /EHsc /nologo /std:c++17 /Iinclude /FeLabelPrinterApp.exe src/main.cpp src/ZebraPrinter.cpp Winspool.lib
```

Or use CMake:

```powershell
cmake -S . -B build
cmake --build build
```

## Run

Update the printer name in `src/main.cpp` to match the installed Windows printer name:

```cpp
printer.setPrinterName(L"ZDesigner ZD410-203dpi ZPL");
```

Then run:

```powershell
.\LabelPrinterApp.exe
```

## Label Templates

The in-code default label lives in `include/LabelPrinterApp/TemplateStorage.h`.

`templates/default_label.json` mirrors the same structure for future file-backed template loading. Element types currently supported by the generator are:

- `Text`
- `Barcode`

## Notes

- Generated build artifacts such as `.exe`, `.obj`, `.pdb`, and `.ilk` files are ignored.
- Printer failures include the Windows error code where available.
