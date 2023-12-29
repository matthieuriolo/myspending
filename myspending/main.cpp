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

    QCommandLineParser parser;
    QString flagM = "m";
    parser.addOptions({
           {flagM, QCoreApplication::translate("main", "Instead of using the physical the database the memory database with testdata is used")}
    });

    parser.process(application);

    DbManager dbManager;
    if (!dbManager.initialize(parser.isSet(flagM))) {
        return -1;
    }

    MainWindow mainWindow(dbManager);
    mainWindow.show();
    return application.exec();
}
