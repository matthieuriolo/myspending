#include "DbManager.h"
#include <QMessageBox>
#include <QtSql>

bool DbManager::initialize() {
    if (!driverInstalled()) {
        return false;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open()) {
        return showError(db.lastError());
    }

    QSqlError q = createTables();
    if (q.type() != QSqlError::NoError) {
        return showError(q);
    }

    return true;
}


bool DbManager::driverInstalled() {
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        return showError("Cannot launch " + GlobalValues::APP_NAME + " without SQLite driver");
    }

    return true;
}

#include <iostream>

QSqlError DbManager::createTables() {
    QStringList tables = db.tables();
    QSqlQuery q;

    for(auto const &entry : sqlCreateTable) {
        if (!tables.contains(entry.first)) {
            if (!q.exec(entry.second)) {
                return q.lastError();
            }
        }
    }

    return QSqlError();
}

bool DbManager::showError(const QString &message) {
    QMessageBox::critical(nullptr, GlobalValues::APP_NAME, message);
    return false;
}

bool DbManager::showError(const QSqlError &err, QWidget* parent) {
    QMessageBox::critical(parent, GlobalValues::APP_NAME, "Error in database with message: " + err.text());
    return false;
}
