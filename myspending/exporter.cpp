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
