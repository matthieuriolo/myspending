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

void DbManager::traverse(DbManagerTraverser &traverser) {
    QSqlQuery queryCategories;
    queryCategories.setForwardOnly(true);
    if (!queryCategories.exec("SELECT " + GlobalValues::SQL_COLUMNNAME_ID + ", " + GlobalValues::SQL_COLUMNNAME_NAME + " FROM " + GlobalValues::SQL_TABLENAME_CATEGORY)) {
        MessageBox::errorSQL(queryCategories.lastError());
        return;
    }

    traverser.beforeTraversingCategories();

    while(queryCategories.next()) {
        auto categoryId = queryCategories.value(GlobalValues::SQL_COLUMNNAME_ID).toInt();
        traverser.traverseCategory(
            queryCategories.value(GlobalValues::SQL_COLUMNNAME_NAME).toString(),
            categoryId
        );

        QSqlQuery queryEntries;
        queryEntries.setForwardOnly(true);

        if (!queryEntries.exec("SELECT " + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", " + GlobalValues::SQL_COLUMNNAME_TYPE + ", " + GlobalValues::SQL_COLUMNNAME_VALUE + "  FROM " + GlobalValues::SQL_TABLENAME_ENTRY + " WHERE " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(categoryId))) {
            MessageBox::errorSQL(queryEntries.lastError());
            return;
        }

        traverser.beforeTraversingEntries(categoryId);

        while (queryEntries.next()) {
            traverser.traverseEntry(
                queryEntries.value(GlobalValues::SQL_COLUMNNAME_DESCRIPTION).toString(),
                queryEntries.value(GlobalValues::SQL_COLUMNNAME_TYPE).toInt(),
                queryEntries.value(GlobalValues::SQL_COLUMNNAME_VALUE).toDouble(),
                categoryId
            );
        }

        traverser.afterTraversingEntries(categoryId);
    }

    traverser.afterTraversingCategories();
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

void DbManager::process(std::function<void(DbManagerInserter&)> processor) {
    db.transaction();

    try {

        QSqlQuery queryCategory;
        if(!queryCategory.prepare("INSERT INTO " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES(:" + GlobalValues::SQL_COLUMNNAME_NAME + ");")) {
            throw QSqlErrorException(queryCategory.lastError());
        }

        QSqlQuery queryEntry;
        if(!queryEntry.prepare("INSERT INTO "+ GlobalValues::SQL_TABLENAME_ENTRY + " ("
                           + GlobalValues::SQL_COLUMNNAME_DESCRIPTION
                           + ", " + GlobalValues::SQL_COLUMNNAME_TYPE
                           + ", " + GlobalValues::SQL_COLUMNNAME_VALUE
                           + ", " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID
                           + ", " + GlobalValues::SQL_COLUMNNAME_DAILY
                           + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY
                           + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY
                           + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY

                           + ") VALUES("

                           + ":" + GlobalValues::SQL_COLUMNNAME_DESCRIPTION
                           + ", :" + GlobalValues::SQL_COLUMNNAME_TYPE
                           + ", :" + GlobalValues::SQL_COLUMNNAME_VALUE
                           + ", :" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID
                           + ", :" + GlobalValues::SQL_COLUMNNAME_DAILY
                           + ", :" + GlobalValues::SQL_COLUMNNAME_WEEKLY
                           + ", :" + GlobalValues::SQL_COLUMNNAME_MONTHLY
                           + ", :" + GlobalValues::SQL_COLUMNNAME_YEARLY
                           + ");")) {
            throw QSqlErrorException(queryEntry.lastError());
        }

        Inserter api = Inserter(queryCategory, queryEntry);
        processor(api);
    } catch(QSqlErrorException &error) {
        MessageBox::errorSQL(error.error());
        db.rollback();
    }catch(std::exception &error) {
        MessageBox::errorException(error);
        db.rollback();
    }

    db.commit();
}


DbManager::Inserter::Inserter(QSqlQuery &queryCategory, QSqlQuery &queryEntry) : queryCategory(queryCategory), queryEntry(queryEntry) {}

int DbManager::Inserter::addCategory(QString name) {
    queryCategory.bindValue(":" + GlobalValues::SQL_COLUMNNAME_NAME, QVariant(name));
    if (!queryCategory.exec()) {
        throw QSqlErrorException(queryCategory.lastError());
    }

    auto categoryId = queryCategory.lastInsertId();
    if (!categoryId.isValid()) {
        throw QSqlErrorException(queryCategory.lastError());
    }

    return categoryId.toInt();
}

int DbManager::Inserter::addEntry(QString description, int type, double value, int categoryId) {
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_DESCRIPTION, QVariant(description));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_TYPE, QVariant(type));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_VALUE, QVariant(value));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID, QVariant(categoryId));

    auto dailyValue = TypeSchedulers.at(type).convertToSmallestUnit(value);

    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_DAILY, QVariant(TypeSchedulers.at(TypeSchedulerDaily).convertToSameUnit(dailyValue)));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_WEEKLY, QVariant(TypeSchedulers.at(TypeSchedulerWeekly).convertToSameUnit(dailyValue)));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_MONTHLY, QVariant(TypeSchedulers.at(TypeSchedulerMonthly).convertToSameUnit(dailyValue)));
    queryEntry.bindValue(":" + GlobalValues::SQL_COLUMNNAME_YEARLY, QVariant(TypeSchedulers.at(TypeSchedulerYearly).convertToSameUnit(dailyValue)));

    if (!queryEntry.exec()) {
        throw QSqlErrorException(queryEntry.lastError());
    }

    auto entryId = queryEntry.lastInsertId();
    if (!entryId.isValid()) {
        QSqlErrorException(queryEntry.lastError());
    }

    return entryId.toInt();
}
