#include "exporter.h"
#include "messagebox.h"

Exporter::Exporter()
{
}

Exporter::~Exporter()
{
    if (dataStream != nullptr) {
        delete dataStream;
    }
}

void Exporter::exportTo(DbManager &dbManager, QString fileName) {
    qDebug() << "Start exporting to " << fileName;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        MessageBox::errorFailedToOpenFile(fileName);
        return;
    }

    // write header
    file.write(HEADER_MIMETYPE);
    file.write(HEADER_SEPARATOR);
    file.write(HEADER_VERSION);
    file.write(HEADER_SEPARATOR);

    // write data
    dataStream = new QDataStream(&file);
    dataStream->setVersion(QDataStream::Qt_5_0);

    dbManager.traverse(*this);

    file.close();

    delete dataStream;
    dataStream = nullptr;

    qDebug() << "Finished exporting";
}


void Exporter::beforeTraversingCategories() {
    qDebug() << "Start exporting categories";
}

void Exporter::traverseCategory(QString name, int id) {
    qDebug() << "Add category " << name << "/" << id;
    *dataStream << DATA_NEXT_OBJECT_EXISTS;
    *dataStream << name;
}

void Exporter::afterTraversingCategories() {
    *dataStream << DATA_NO_OTHER_OBJECT_EXISTS;
    qDebug() << "finished exporting categories";
}

void Exporter::beforeTraversingEntries(int categoryId) {
    qDebug() << "Start exporting entries for category " << categoryId;
}

void Exporter::traverseEntry(QString description, int type, double value, int categoryId) {
    qDebug() << "Add entry " << description << " for category " << categoryId;
    *dataStream << DATA_NEXT_OBJECT_EXISTS;
    *dataStream << description;
    *dataStream << type;
    *dataStream << value;
}

void Exporter::afterTraversingEntries(int categoyrId) {
    *dataStream << DATA_NO_OTHER_OBJECT_EXISTS;
    qDebug() << "No more entries for category " << categoyrId;
}


//void Exporter::importTo(QString fileName) {
//    qDebug() << "Start importing from " << fileName;

//    QFile file(fileName);
//    if (!file.open(QIODevice::ReadOnly)) {
//        MessageBox::errorFailedToOpenFile(fileName);
//        return;
//    }

//    // read header
//    char buffer[64];
//    auto lengthMimeType =  QString(HEADER_MIMETYPE).size();
//    if (file.read(buffer, lengthMimeType) != lengthMimeType) {
//        MessageBox::errorFailedToOpenFile(fileName);
//        return;
//    }

//    buffer[lengthMimeType] = '\0';
//    if (strcmp(HEADER_MIMETYPE, buffer) != 0) {
//        MessageBox::errorFailedToOpenFile(fileName);
//        return;
//    }

//    file.skip(QString(HEADER_SEPARATOR).size());

//    auto lengthVersion =  QString(HEADER_VERSION).size();
//    if (file.read(buffer, lengthVersion) != lengthVersion) {
//        MessageBox::errorFailedToOpenFile(fileName);
//        return;
//    }

//    buffer[lengthVersion] = '\0';
//    if (strcmp(HEADER_VERSION, buffer) != 0) {
//        MessageBox::errorFailedToOpenFile(fileName);
//        return;
//    }

//    file.skip(QString(HEADER_SEPARATOR).size());


//    QDataStream in(&file);
//    in.setVersion(QDataStream::Qt_5_0);

//    QSqlQuery queryCategory;
//    if(!queryCategory.prepare("INSERT INTO " + GlobalValues::SQL_TABLENAME_CATEGORY + " (" + GlobalValues::SQL_COLUMNNAME_NAME + ") VALUES(:name);")) {
//        MessageBox::errorSQL(queryCategory.lastError());
//        return;
//    }

//    QSqlQuery queryEntry;
//    if(!queryEntry.prepare("INSERT INTO "+ GlobalValues::SQL_TABLENAME_ENTRY + " ("
//                       + GlobalValues::SQL_COLUMNNAME_DESCRIPTION + ", "
//                       + GlobalValues::SQL_COLUMNNAME_TYPE
//                       + ", " + GlobalValues::SQL_COLUMNNAME_VALUE
//                       + ", " + GlobalValues::SQL_COLUMNNAME_CATEGORY_ID
//                       + ", " + GlobalValues::SQL_COLUMNNAME_DAILY
//                       + ", " + GlobalValues::SQL_COLUMNNAME_WEEKLY
//                       + ", " + GlobalValues::SQL_COLUMNNAME_MONTHLY
//                       + ", " + GlobalValues::SQL_COLUMNNAME_YEARLY
//                           + ") VALUES(:description, :type, :value, :categoryId, :daily, :weekly, :monthly, :yearly);")) {
//        MessageBox::errorSQL(queryEntry.lastError());
//        return;
//    }

//    QSqlDatabase::database().transaction();

//    qint8 objectAvailable;
//    in >> objectAvailable;
//    while (objectAvailable == DATA_NEXT_OBJECT_EXISTS) {
//        QString categoryName;
//        in >> categoryName;

//        qDebug() << "Importing category: " << categoryName;

//        queryCategory.bindValue(":name", QVariant(categoryName));
//        if (!queryCategory.exec()) {
//            MessageBox::errorSQL(queryCategory.lastError());
//            QSqlDatabase::database().rollback();
//            return;
//        }

//        auto categoryIdVariant = queryCategory.lastInsertId();

//        if (!categoryIdVariant.isValid()) {
//            MessageBox::errorSQL(queryCategory.lastError());
//            QSqlDatabase::database().rollback();
//            return;
//        }

//        auto categoryId = categoryIdVariant.toInt();
//        qDebug() << "Added category: " << categoryName << " with id: " << categoryId;

//        in >> objectAvailable;
//        while (objectAvailable == DATA_NEXT_OBJECT_EXISTS) {
//            QString description;
//            int type;
//            double value;

//            in >> description;
//            in >> type;
//            in >> value;

//            qDebug() << "Importing entry: " << description << " for category with id: " << categoryId;

//            queryEntry.bindValue(":description", QVariant(description));
//            queryEntry.bindValue(":type", QVariant(type));
//            queryEntry.bindValue(":value", QVariant(value));
//            queryEntry.bindValue(":categoryId", QVariant(categoryId));

//            auto dailyValue = TypeSchedulers.at(type).convertToSmallestUnit(value);

//            queryEntry.bindValue(":daily", QVariant(TypeSchedulers.at(TypeSchedulerDaily).convertToSameUnit(dailyValue)));
//            queryEntry.bindValue(":weekly", QVariant(TypeSchedulers.at(TypeSchedulerWeekly).convertToSameUnit(dailyValue)));
//            queryEntry.bindValue(":monthly", QVariant(TypeSchedulers.at(TypeSchedulerMonthly).convertToSameUnit(dailyValue)));
//            queryEntry.bindValue(":yearly", QVariant(TypeSchedulers.at(TypeSchedulerYearly).convertToSameUnit(dailyValue)));

//            if (!queryEntry.exec()) {
//                MessageBox::errorSQL(queryEntry.lastError());
//                QSqlDatabase::database().rollback();
//                return;
//            }

//            qDebug() << "Added entry " << description << " for category with id: " << categoryId;
//            in >> objectAvailable;
//        }

//        in >> objectAvailable;
//    }

//    QSqlDatabase::database().commit();
//    qDebug() << "Finished importing";
//}
