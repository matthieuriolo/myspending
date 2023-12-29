#include "dbmanager.h"
#include <QtSql>
#include <vector>
#include "messagebox.h"

#include<QDebug>
bool DbManager::initialize(bool inMemory) {
    if (!driverInstalled()) {
        return false;
    }

    db = QSqlDatabase::addDatabase(GlobalValues::SQL_CONNECTION_DEFAULT_NAME);
    if (inMemory) {
        qDebug() << "Storing data in memory";
        db.setDatabaseName(":memory:");
    } else {
        auto writeableDirectory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        if (writeableDirectory.isEmpty()) {
            MessageBox::errorNoWritableLocationFound();
            return false;
        }

        if (!QDir().mkpath(writeableDirectory)) {
            MessageBox::errorFailedToCreateWriteableDirectory(writeableDirectory);
            return false;
        }

        auto sqlFile = QDir(writeableDirectory).absoluteFilePath(GlobalValues::SQL_DB_PATH);

        qDebug() << "Storing data in: " + sqlFile;
        db.setDatabaseName(sqlFile);
    }


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


    if (inMemory) {
        q = testData();
        if (q.type() != QSqlError::NoError) {
            MessageBox::errorSQL(q);
            return false;
        }
    }

    return true;
}

double DbManager::sumDailyValues(int categoryId) {
    auto query = "SELECT SUM(" + GlobalValues::SQL_COLUMNNAME_DAILY + ") FROM " + GlobalValues::SQL_TABLENAME_ENTRY + " WHERE " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(categoryId) + ";";

    QSqlQuery q;
    if (!q.exec(query)) {
        MessageBox::errorSQL(q.lastError());
        return 0;
    }

    if (!q.next()) {
        MessageBox::errorSQL(q.lastError());
        return 0;
    }

    return q.value(0).toDouble();
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
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ", " + GlobalValues::SQL_COLUMNNAME_DAILY + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY + ") VALUES(1, 'entry1', 0, 10, 0, 0, 0, 0)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ", " + GlobalValues::SQL_COLUMNNAME_DAILY + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY + ") VALUES(1, 'entry2', 1, 100, 100, 700, 3000.00, 36000.00)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ", " + GlobalValues::SQL_COLUMNNAME_DAILY + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY + ") VALUES(1, 'entry3', 2, 1000, 142.86, 1000.00, 4285.71, 51428.57)",
        "insert into " + GlobalValues::SQL_TABLENAME_ENTRY + " (" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + ", " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + ", " + GlobalValues::SQL_COLUMNNAME_DAILY + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY + ") VALUES(2, 'entry4', 1, 100, 100, 700, 3000.00, 36000.00)",
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
