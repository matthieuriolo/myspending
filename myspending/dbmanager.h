#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
#include <map>
#include "globals.h"
#include "dbmanagertraverser.h"

using namespace std;

class DbManager {
public:


    DbManager() {}

    bool initialize(bool inMemory);

    double sumDailyValues(int category_id);

    void traverse(DbManagerTraverser &traverser);

//    void iterateCategories(QDataStream &out, void (*callback)(QDataStream &out, QString name, int id));
//    void iterateEntries(int categoryId, void (*callback)(QString description, int type, double value));
private:
    QSqlDatabase db;

    map<QString, QString> sqlCreateTable = {
        // TODO unique constaint entry.name
        {
            GlobalValues::SQL_TABLENAME_CATEGORY,
            "create table category("
            + GlobalValues::SQL_COLUMNNAME_ID + " integer primary key, "
            + GlobalValues::SQL_COLUMNNAME_NAME + " varchar" +
            ");"
        },
        {
            GlobalValues::SQL_TABLENAME_ENTRY,
            "create table entry("
            + GlobalValues::SQL_COLUMNNAME_ID + " integer primary key, "
            + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " integer NOT NULL, "
            + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + " varchar, "
            + GlobalValues::SQL_COLUMNNAME_VALUE + " double, "
            + GlobalValues::SQL_COLUMNNAME_TYPE + " integer, "
            + GlobalValues::SQL_COLUMNNAME_DAILY + " double, "
            + GlobalValues::SQL_COLUMNNAME_WEEKLY + " double, "
            + GlobalValues::SQL_COLUMNNAME_MONTHLY + " double, "
            + GlobalValues::SQL_COLUMNNAME_YEARLY + " double, "

            + "FOREIGN KEY ("
                + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ") REFERENCES "
                + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_ID + ")"
                + "ON DELETE CASCADE"
            + ");"
        }
    };

    bool driverInstalled();
    QSqlError createTables();
    QSqlError testData();
};


#endif // DBMANAGER_H
