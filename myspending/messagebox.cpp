#include "messagebox.h"
#include <QMessageBox>
#include "globals.h"

void MessageBox::errorMissingSQLDriver() {
    QMessageBox::critical(nullptr, GlobalValues::APP_NAME, "Cannot launch " + GlobalValues::APP_NAME + " without SQLite driver");
}

void MessageBox::errorNoWritableLocationFound() {
    QMessageBox::critical(nullptr, GlobalValues::APP_NAME, GlobalValues::APP_NAME + " cannot find the default writeable application data folder");
}

void MessageBox::errorFailedToCreateWriteableDirectory(QString &path) {
    QMessageBox::critical(nullptr, GlobalValues::APP_NAME, "Cannot create directory: " + path);
}

void MessageBox::errorSQL(const QSqlError &err, QWidget* parent) {
    QMessageBox::critical(parent, GlobalValues::APP_NAME, "Error in database with message: " + err.text());
}

