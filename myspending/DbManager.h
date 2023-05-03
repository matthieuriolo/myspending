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
    map<QLatin1String, QLatin1String> sqlCreateTable = {
        {GlobalValues::SQL_TABLENAME_ENTRIES, QLatin1String("create table entry(id integer primary key, type int);")}
    };

    bool showError(const QString &message);
    bool showError(const QSqlError &err, QWidget* parent = nullptr);

    bool driverInstalled();
    QSqlError createTables();
};


#endif // DBMANAGER_H
