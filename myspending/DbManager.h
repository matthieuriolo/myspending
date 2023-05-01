#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
#include <map>

using namespace std;

class DbManager {
public:
    DbManager() {}

    bool initialize();
private:
    map<QLatin1String, QLatin1String> sqlCreateTable = {
        {QLatin1String("entry"), QLatin1String("create table entry(id integer primary key, type int)")}
    };
    QSqlDatabase db;

    bool showError(const QString &message);
    bool showError(const QSqlError &err, QWidget* parent = nullptr);

    bool driverInstalled();
    QSqlError createTables();
};


#endif // DBMANAGER_H
