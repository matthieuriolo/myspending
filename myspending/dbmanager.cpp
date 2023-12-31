#include "dbmanager.h"
#include <QtSql>
#include <vector>
#include "messagebox.h"
#include <QDebug>
#include "typeenum.h"


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

// TODO
const auto MIMETYPE = "application/myspending";
const auto VERSION = "1";
const auto SEPARATOR = "\n";
const qint8 NEXT_OBJECT_EXISTS = 0x0;
const qint8 NO_OTHER_OBJECT_EXISTS = 0x1;

void DbManager::exportTo(QString fileName) {
    qDebug() << "Start exporting to " << fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    // write header
    file.write(MIMETYPE);
    file.write(SEPARATOR);
    file.write(VERSION);
    file.write(SEPARATOR);

    // write data
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);

    QSqlQuery queryCategories;
    queryCategories.setForwardOnly(true);
    if (!queryCategories.exec("SELECT " + GlobalValues::SQL_COLUMNNAME_ID + ", " + GlobalValues::SQL_COLUMNNAME_NAME + " FROM " + GlobalValues::SQL_TABLENAME_CATEGORY)) {
        MessageBox::errorSQL(queryCategories.lastError());
        return;
    }

    while(queryCategories.next()) {
        qDebug() << "Add category " << queryCategories.value(GlobalValues::SQL_COLUMNNAME_NAME) << "/" << queryCategories.value(GlobalValues::SQL_COLUMNNAME_ID);
        out << NEXT_OBJECT_EXISTS;
        out << queryCategories.value(GlobalValues::SQL_COLUMNNAME_NAME).toString();

        QSqlQuery queryEntries;
        queryEntries.setForwardOnly(true);

        if (!queryEntries.exec("SELECT " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + "  FROM " + GlobalValues::SQL_TABLENAME_ENTRY + " WHERE " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + queryCategories.value(GlobalValues::SQL_COLUMNNAME_ID).toString())) {
            MessageBox::errorSQL(queryEntries.lastError());
            return;
        }

        while (queryEntries.next()) {
            qDebug() << "Add entry " << queryEntries.value(GlobalValues::SQL_COLUMNNAME_DESCRIPTION) << " for category " << queryCategories.value(GlobalValues::SQL_COLUMNNAME_NAME) << "/" << queryCategories.value(GlobalValues::SQL_COLUMNNAME_ID);
            out << NEXT_OBJECT_EXISTS;
            out << queryEntries.value(GlobalValues::SQL_COLUMNNAME_DESCRIPTION).toString();
            out << queryEntries.value(GlobalValues::SQL_COLUMNNAME_TYPE).toInt();
            out << queryEntries.value(GlobalValues::SQL_COLUMNNAME_VALUE).toDouble();
        }

        qDebug() << "No more entries for category " << queryCategories.value(GlobalValues::SQL_COLUMNNAME_NAME) << "/" << queryCategories.value(GlobalValues::SQL_COLUMNNAME_ID);
        out << NO_OTHER_OBJECT_EXISTS;
    }
    qDebug() << "No more categories";
    out << NO_OTHER_OBJECT_EXISTS;
    file.close();

    qDebug() << "Finished exporting";
}

void DbManager::importTo(QString fileName) {
    qDebug() << "Start importing from " << fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    // read header
    char buffer[64];
    auto lengthMimeType =  QString(MIMETYPE).size();
    if (file.read(buffer, lengthMimeType) != lengthMimeType) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    buffer[lengthMimeType] = '\0';
    if (strcmp(MIMETYPE, buffer) != 0) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    file.skip(QString(SEPARATOR).size());

    auto lengthVersion =  QString(VERSION).size();
    if (file.read(buffer, lengthVersion) != lengthVersion) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    buffer[lengthVersion] = '\0';
    if (strcmp(VERSION, buffer) != 0) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    file.skip(QString(SEPARATOR).size());


    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0);

    QSqlQuery queryCategory;
    if(!queryCategory.prepare("INSERT INTO " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES(:name);")) {
        MessageBox::errorSQL(queryCategory.lastError());
        return;
    }

    QSqlQuery queryEntry;
    if(!queryEntry.prepare("INSERT INTO "+ GlobalValues::SQL_TABLENAME_ENTRY + " ("
                       + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", "
                       + GlobalValues::SQL_COLUMNNAME_TYPE
                       + ", " + GlobalValues::SQL_COLUMNNAME_VALUE
                       + ", " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID
                       + ", " + GlobalValues::SQL_COLUMNNAME_DAILY
                       + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY
                       + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY
                       + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY
                           + ") VALUES(:description, :type, :value, :categoryId, :daily, :weekly, :monthly, :yearly);")) {
        MessageBox::errorSQL(queryEntry.lastError());
        return;
    }

    QSqlDatabase::database().transaction();

    qint8 objectAvailable;
    in >> objectAvailable;
    while (objectAvailable == NEXT_OBJECT_EXISTS) {
        QString categoryName;
        in >> categoryName;

        qDebug() << "Importing category: " << categoryName;

        queryCategory.bindValue(":name", QVariant(categoryName));
        if (!queryCategory.exec()) {
            MessageBox::errorSQL(queryCategory.lastError());
            QSqlDatabase::database().rollback();
            return;
        }

        auto categoryIdVariant = queryCategory.lastInsertId();

        if (!categoryIdVariant.isValid()) {
            MessageBox::errorSQL(queryCategory.lastError());
            QSqlDatabase::database().rollback();
            return;
        }

        auto categoryId = categoryIdVariant.toInt();
        qDebug() << "Added category: " << categoryName << " with id: " << categoryId;

        in >> objectAvailable;
        while (objectAvailable == NEXT_OBJECT_EXISTS) {
            QString description;
            int type;
            double value;

            in >> description;
            in >> type;
            in >> value;

            qDebug() << "Importing entry: " << description << " for category with id: " << categoryId;

            queryEntry.bindValue(":description", QVariant(description));
            queryEntry.bindValue(":type", QVariant(type));
            queryEntry.bindValue(":value", QVariant(value));
            queryEntry.bindValue(":categoryId", QVariant(categoryId));

            auto dailyValue = TypeSchedulers.at(type).convertToSmallestUnit(value);

            queryEntry.bindValue(":daily", QVariant(TypeSchedulers.at(TypeSchedulerDaily).convertToSameUnit(dailyValue)));
            queryEntry.bindValue(":weekly", QVariant(TypeSchedulers.at(TypeSchedulerWeekly).convertToSameUnit(dailyValue)));
            queryEntry.bindValue(":monthly", QVariant(TypeSchedulers.at(TypeSchedulerMonthly).convertToSameUnit(dailyValue)));
            queryEntry.bindValue(":yearly", QVariant(TypeSchedulers.at(TypeSchedulerYearly).convertToSameUnit(dailyValue)));

            if (!queryEntry.exec()) {
                MessageBox::errorSQL(queryEntry.lastError());
                QSqlDatabase::database().rollback();
                return;
            }

            qDebug() << "Added entry " << description << " for category with id: " << categoryId;
            in >> objectAvailable;
        }

        in >> objectAvailable;
    }

    QSqlDatabase::database().commit();
    qDebug() << "Finished importing";
}
