# Classic Label Designer UI Redesign

## Goal

Move LabelPrinterApp toward a classic LabelDirect/ZebraDesigner-style desktop layout while keeping Zebra ZPL generation, templates, Excel/CSV records, preview, and printing.

## Wireframe

```text
+--------------------------------------------------------------------------------+
| File Insert Edit Layout View Stock Database Printer Preferences Templates Help   |
+--------------------------------------------------------------------------------+
| New Open Save Print Cut Copy Paste Undo Redo Zoom +/- Fit Text Barcode QR ...    |
+----+-------------------------------------------------------------+---------------+
|SEL | inch ruler                                                  | LabelPrinterApp|
|A   |-------------------------------------------------------------| Zebra Designer |
|123 |                                                             | Text|Format... |
|||| |   gray workspace                                            | Element Name  |
|QR  |      +-----------------------------------------------+      | Position      |
|DT  |      | dashed printable boundary                     |      | Font          |
|#   |      | selected object with resize handles            |      | Text/Source   |
|-   |      +-----------------------------------------------+      | Barcode/Print |
|BOX |                                                             |               |
|IMG |                                                             |               |
+----+-------------------------------------------------------------+---------------+
| Align L C R | Align T M B | Equal Space | Front | Back | Lock | Unlock          |
+--------------------------------------------------------------------------------+
| For Help, press F1                         | Label 2.25 x 0.75 | DPI 203 | Zoom |
+--------------------------------------------------------------------------------+
```

## Qt Widgets

- `QMainWindow`: shell, menu bar, main toolbar, bottom toolbar, status bar.
- `QToolBar`: dense icon-style command strips.
- `QTabWidget`: compact workflow access for Design, Elements, Data, Templates, Print, Settings.
- `QFrame` + `QVBoxLayout`: compact left tool palette and right inspector.
- `PreviewWidget`: rulered canvas, white label, grid, dashed printable boundary, selection handles.
- `ElementEditorWidget`: right inspector editor embedded under Text/Formatting/Position/Data/Barcode/Print tabs.

## Visual Style

- App background: `#ececec`
- Toolbar/panels: `#eeeeee`
- Canvas workspace: `#d6d6d6`
- Label: white with dark outline
- Rulers: `#ebebeb`
- Grid: pale blue-gray dotted lines
- Selection: blue outline with gray resize handles
- Font: Segoe UI/Arial, compact 9 pt

## Behavior Notes

- The Design tab now visually behaves like the old professional design surface.
- Data, Templates, Print, and Settings stay available so no printing workflow is removed.
- Status bar reports help text, cursor placeholder, label size, DPI/dots, and zoom.
- Properties inspector tabs are in place for future per-type panels; the Text tab contains the current full editor.
