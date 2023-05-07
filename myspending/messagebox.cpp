#include "messagebox.h"
#include <QMessageBox>
#include "globals.h"

void MessageBox::errorMissingSQLDriver() {
    QMessageBox::critical(nullptr, GlobalValues::APP_NAME, "Cannot launch " + GlobalValues::APP_NAME + " without SQLite driver");
}

void MessageBox::errorSQL(const QSqlError &err, QWidget* parent) {
    QMessageBox::critical(parent, GlobalValues::APP_NAME, "Error in database with message: " + err.text());
}

