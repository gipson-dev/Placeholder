# Changelog

## 2026-06-19

- Added typed label elements for text and barcode output.
- Updated ZPL generation to emit barcode fields.
- Reworked the default label template to use the current C++ model.
- Removed implementation-file includes from `main.cpp`.
- Added a CMake build target and updated the VS Code build task.
- Moved source files into a cleaner `include/` and `src/` layout.
- Added GitHub Actions CI for Windows builds.
- Updated README content with layout, build, run, and template notes.
- Expanded `.gitignore` for MSVC and build output files.
- Improved printer error messages with Windows error codes.

## 2026-06-19 - Phase 5 UI

- Added a real Win32 `MainWindow`.
- Added printer selection from installed Windows printers.
- Added dynamic input fields generated from JSON element names.
- Added a `PreviewWidget` that draws text and barcode elements.
- Added Print and Save Template button behavior.
- Changed the application entry point from console output to the Windows UI.
