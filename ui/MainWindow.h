#pragma once

#include <QMainWindow>
#include <QTableWidget>

#include "core/CsvImporter.h"
#include "core/LabelTemplate.h"
#include "core/VariableResolver.h"

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;

class ElementEditorWidget;
class PreviewWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildUi();
    void refreshPrinterList();
    void refreshElementList();
    void refreshSettingsControls();
    void refreshPreview();
    void loadDefaultTemplate();
    void addElement(LabelElementType type);
    void deleteSelectedElement();
    void saveTemplate();
    void loadTemplate();
    void importCsv();
    void configureCsvMapping();
    void printCurrent();
    void printSelectedCsvRows();
    void printAllCsvRows();
    void updateTemplateFromSettings();
    void selectElement(int index);
    VariableContext contextForRow(int rowIndex) const;
    VariableContext promptContext() const;
    std::map<std::string, std::string> csvMapping() const;
    int quantityForRow(int rowIndex) const;
    void printContexts(const std::vector<VariableContext>& contexts, int quantityPerContext);

    LabelTemplate labelTemplate_;
    CsvData csvData_;
    std::map<std::string, std::string> csvMappingOverrides_;
    int selectedElement_ = -1;

    PreviewWidget* preview_ = nullptr;
    ElementEditorWidget* editor_ = nullptr;
    QListWidget* elementList_ = nullptr;
    QComboBox* printerCombo_ = nullptr;
    QSpinBox* dpiSpin_ = nullptr;
    QDoubleSpinBox* widthSpin_ = nullptr;
    QDoubleSpinBox* heightSpin_ = nullptr;
    QDoubleSpinBox* marginLeftSpin_ = nullptr;
    QDoubleSpinBox* marginTopSpin_ = nullptr;
    QDoubleSpinBox* gapSpin_ = nullptr;
    QComboBox* mediaCombo_ = nullptr;
    QComboBox* orientationCombo_ = nullptr;
    QSpinBox* speedSpin_ = nullptr;
    QSpinBox* darknessSpin_ = nullptr;
    QSpinBox* copiesSpin_ = nullptr;
    QSpinBox* serialStartSpin_ = nullptr;
    QSpinBox* serialEndSpin_ = nullptr;
    QTableWidget* csvTable_ = nullptr;
};
