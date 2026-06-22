#include <QApplication>

#include "ui/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("LabelPrinterApp");
    QApplication::setOrganizationName("LabelPrinterApp");

    MainWindow window;
    window.resize(1280, 760);
    window.show();

    return app.exec();
}
