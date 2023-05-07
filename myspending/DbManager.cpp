#include "DbManager.h"
#include <QMessageBox>
#include <QtSql>
#include <vector>

bool DbManager::initialize() {
    if (!driverInstalled()) {
        return false;
    }

    db = QSqlDatabase::addDatabase(GlobalValues::SQL_CONNECTION_DEFAULT_NAME);
    db.setDatabaseName(":memory:");

    if (!db.open()) {
        return showError(db.lastError());
    }

    QSqlError q = createTables();
    if (q.type() != QSqlError::NoError) {
        return showError(q);
    }

    q = testData();
    if (q.type() != QSqlError::NoError) {
        return showError(q);
    }

    return true;
}


bool DbManager::driverInstalled() {
    if (!QSqlDatabase::drivers().contains(GlobalValues::SQL_CONNECTION_DEFAULT_NAME)) {
        return showError("Cannot launch " + GlobalValues::APP_NAME + " without SQLite driver");
    }

    return true;
}


QSqlError DbManager::testData() {
    QSqlQuery q;

    std::vector<QString> inserts = {
        "insert into " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES('category1')",
        "insert into " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES('category2')",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(1, 'entry1', 0, 10)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(1, 'entry2', 1, 100)"
    };

    for (auto queryString : inserts) {
        if (!q.exec(queryString)) {
            return q.lastError();
        }
    }

    return QSqlError();
}

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
