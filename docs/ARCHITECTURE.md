# LabelPrinterApp Architecture

LabelPrinterApp is a Windows C++17 Qt 6 Widgets application that designs and prints Zebra ZPL labels for 2.25 x 0.75 inch direct thermal removable adhesive media. The default printer target is a Zebra ZD410-compatible ZPL printer using the Windows RAW spooler path.

## Project Structure

```text
LabelPrinterApp/
├── CMakeLists.txt
├── main.cpp
├── core/
│   ├── PrinterSettings.h
│   ├── LabelElement.h
│   ├── LabelTemplate.h
│   ├── ZplGenerator.h/.cpp
│   ├── ZebraPrinter.h/.cpp
│   ├── TemplateStorage.h/.cpp
│   ├── CsvImporter.h/.cpp
│   └── VariableResolver.h/.cpp
├── ui/
│   ├── MainWindow.h/.cpp
│   ├── PreviewWidget.h/.cpp
│   └── ElementEditorWidget.h/.cpp
├── templates/default_label.json
├── examples/sample_items.csv
└── tests/model_zpl_tests.cpp
```

## Core Classes

- `PrinterSettings` stores printer, DPI, label size, margins, gap, sensing mode, orientation, darkness, speed, and copies.
- `LabelElement` models text, Code 128, Code 39, and QR fields with fixed, variable, prompt-at-print, and serial-number sources.
- `LabelTemplate` owns the printer settings and ordered element list.
- `VariableResolver` replaces `{Placeholders}`, built-in date/time tokens, and serial values before printing.
- `ZplGenerator` converts a resolved template into ZPL with `^PW`, `^LL`, `^LH`, `^MN`, `^FW`, text, barcode, and QR commands.
- `ZebraPrinter` enumerates Windows printers and sends raw ZPL using `OpenPrinterA`, `StartDocPrinterA`, `WritePrinter`, and `ClosePrinter`.
- `TemplateStorage` saves and loads JSON templates with nlohmann/json.
- `CsvImporter` parses quoted CSV data, detects headers, and maps columns to template placeholders.

## UI Classes

- `MainWindow` wires printer settings, element editing, template load/save, CSV import, serial ranges, and print commands.
- `ElementEditorWidget` edits element content, source mode, position, formatting, barcode settings, QR settings, and wrapping.
- `PreviewWidget` paints a scaled label preview and supports drag-to-move positioning for future designer work.

## Version Coverage

- Version 1: basic fields, font size, X/Y position, copies, valid ZPL, RAW printing.
- Version 2: Qt GUI, preview, multiple elements, add/edit/delete, JSON templates, printer selection.
- Version 3: Code 128, Code 39, QR, barcode height, module width, human-readable toggle.
- Version 4: placeholders, prompt-at-print values, date/time tokens, serial numbers, prefix/suffix, serial ranges.
- Version 5: CSV import, header detection, placeholder mapping, row preview, selected/all row printing, quantity per row.

## Build In Visual Studio

1. Install Visual Studio 2022 or newer with "Desktop development with C++".
2. Install Qt 6 for MSVC 64-bit and make sure `CMAKE_PREFIX_PATH` points to the Qt install, for example `C:\Qt\6.7.3\msvc2022_64`.
3. Open the folder in Visual Studio with "Open a local folder", or run:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.7.3\msvc2022_64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The helper script can find Visual Studio's bundled CMake:

```powershell
.\scripts\build-and-test.ps1
```

## Test Printing

1. Install the Zebra Windows driver and configure the printer for ZPL mode.
2. Load `templates/default_label.json`.
3. Select the Zebra printer in the Printer dropdown.
4. Set DPI to match the printer, usually 203 or 300.
5. Use `examples/sample_items.csv` to test CSV printing, or use Print Current and enter prompted values.
6. Print one label first and verify alignment before printing a batch.

The default label is 2.25 x 0.75 inches with gap sensing. Use black mark or continuous mode only when the loaded media requires it.
