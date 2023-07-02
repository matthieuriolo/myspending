#include "dbmanager.h"
#include <QtSql>
#include <vector>
#include "messagebox.h"


bool DbManager::initialize() {
    if (!driverInstalled()) {
        return false;
    }

    db = QSqlDatabase::addDatabase(GlobalValues::SQL_CONNECTION_DEFAULT_NAME);
    db.setDatabaseName(":memory:");

    if (!db.open()) {
        MessageBox::errorSQL(db.lastError());
        return false;
    }

    QSqlError q = db.exec("PRAGMA foreign_keys = ON").lastError();
    if (q.type() != QSqlError::NoError) {
        MessageBox::errorSQL(q);
        return false;
    }

    q = createTables();
    if (q.type() != QSqlError::NoError) {
        MessageBox::errorSQL(q);
        return false;
    }

    q = testData();
    if (q.type() != QSqlError::NoError) {
        MessageBox::errorSQL(q);
        return false;
    }

    return true;
}


bool DbManager::driverInstalled() {
    if (!QSqlDatabase::drivers().contains(GlobalValues::SQL_CONNECTION_DEFAULT_NAME)) {
        MessageBox::errorMissingSQLDriver();
        return false;
    }

    return true;
}


QSqlError DbManager::testData() {
    QSqlQuery q;

    std::vector<QString> inserts = {
        "insert into " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES('category1')",
        "insert into " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES('category2')",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(1, 'entry1', 0, 10)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(1, 'entry2', 1, 100)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(1, 'entry3', 2, 1000)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ") VALUES(2, 'entry4', 1, 100)",
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
