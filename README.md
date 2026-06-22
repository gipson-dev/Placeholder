# LabelPrinterApp

LabelPrinterApp is a Windows C++17 Qt 6 desktop application for designing and printing Zebra ZPL labels. It targets 2.25 x 0.75 inch direct thermal removable adhesive labels on Zebra ZD410-class printers using the Windows RAW spooler path.

## Version 5 Features

- 203 DPI and 300 DPI label sizing with width, height, margins, gap, sensing mode, orientation, darkness, speed, and copies.
- Multiple editable label elements: text, Code 128, Code 39, and QR.
- Text formatting: font selection, font size, bold, italic, underline, rotation, alignment, multiline wrapping, fixed fields, variable fields, prefix/suffix, and auto-fit metadata.
- Qt GUI with printer selection, template load/save, element add/edit/delete, live preview, and drag-to-move element positioning.
- Placeholder replacement with `{ItemNumber}` syntax plus `{Date}`, `{Time}`, `{DateTime}`, `{Serial}`, and `{RecordIndex}`.
- Prompt-at-print values, serial ranges, CSV import, header detection, row preview, selected/all row printing, and quantity per row through `Quantity` or `Qty`.
- RAW ZPL printing through `OpenPrinterA`, `StartDocPrinterA`, `WritePrinter`, and `ClosePrinter`.

## Project Layout

```text
core/                 Label model, CSV import, variables, JSON storage, ZPL generation, RAW printing
ui/                   Qt MainWindow, PreviewWidget, and ElementEditorWidget
templates/            Default JSON template
examples/             Sample CSV data
docs/                 Architecture notes and example generated ZPL
tests/                Core model/ZPL/storage tests
.github/workflows/    Windows CI build
```

The full architecture is documented in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Requirements

- Windows
- Visual Studio 2022 or newer with Desktop development with C++
- CMake 3.20 or newer
- Qt 6 Widgets for MSVC 64-bit
- Zebra printer driver configured for ZPL/RAW output

`nlohmann/json` is resolved by CMake. If it is not installed, CMake fetches version 3.11.3 by default.

## Build In Visual Studio

Set `CMAKE_PREFIX_PATH` to your Qt install path, then configure and build:

```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.7.3\msvc2022_64"
.\scripts\build-and-test.ps1 -Config Release
```

Or run CMake directly:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.7.3\msvc2022_64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Package

```powershell
.\scripts\package-release.ps1 -Config Release
```

The package is written to `dist\LabelPrinterApp`. If `windeployqt.exe` is on `PATH` or under `CMAKE_PREFIX_PATH\bin`, the script copies the Qt runtime files.

## Print A Test Label

1. Connect the Zebra printer and confirm the Windows driver is installed.
2. Launch `build\Release\LabelPrinterApp.exe`.
3. Select the Zebra printer.
4. Confirm DPI, label width `2.25`, label height `0.75`, and media sensing mode.
5. Import `examples\sample_items.csv` or print the current template and enter prompted values.
6. Print one label and adjust X/Y, margins, darkness, or speed if the label is not aligned.

Example ZPL is available in [docs/example_generated.zpl](docs/example_generated.zpl).
