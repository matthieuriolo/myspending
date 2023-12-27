#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
#include "dbmanager.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "myspending_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            application.installTranslator(&translator);
            break;
        }
    }


    DbManager dbManager;
    if (!dbManager.initialize()) {
        return -1;
    }

    MainWindow mainWindow(dbManager);
    mainWindow.show();
    return application.exec();
}
