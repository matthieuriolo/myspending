#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
#include <map>
#include "globals.h"

using namespace std;

class DbManager {
public:
    DbManager() {}

    bool initialize();

    QSqlDatabase db;

private:
    map<QString, QString> sqlCreateTable = {
        // TODO unique constaint entry.name
        {GlobalValues::SQL_TABLENAME_CATEGORY, "create table category(" + GlobalValues::SQL_COLUMNNAME_ID + " integer primary key, " + GlobalValues::SQL_COLUMNNAME_NAME + " varchar);"},
        {
            GlobalValues::SQL_TABLENAME_ENTRY, "create table entry("
            + GlobalValues::SQL_COLUMNNAME_ID + " integer primary key, "
            + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " integer NOT NULL, "
            + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + " varchar, "
            + GlobalValues::SQL_COLUMNNAME_VALUE + " double, "
            + GlobalValues::SQL_COLUMNNAME_TYPE + " integer, "
            + "FOREIGN KEY (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ") REFERENCES " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_ID + ") );"
        }
    };

    bool driverInstalled();
    QSqlError createTables();
    QSqlError testData();
};


#endif // DBMANAGER_H
