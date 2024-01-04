#include "importer.h"
#include <QDebug>

#include "exporter.h"
#include "datatraverser.h"
#include "messagebox.h"

Importer::Importer(QString fileName) : fileName(fileName)
{

}


void Importer::operator()(DbManagerInserter &inserter) {
        qDebug() << "Start importing from " << fileName;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            MessageBox::errorFailedToOpenFile(fileName);
            return;
        }

        if(checkHeader(file, fileName)) {
            QDataStream dataStream(&file);
            processData(dataStream, inserter);
        }

        qDebug() << "Finished importing";
}

bool Importer::checkHeader(QFile &file, QString fileName) {
    char buffer[64];
    auto lengthMimeType =  QString(Exporter::HEADER_MIMETYPE).size();
    if (file.read(buffer, lengthMimeType) != lengthMimeType) {
        qDebug() << "CheckHeader failed because of missing mimeType";
        MessageBox::errorFailedToOpenFile(fileName);
        return false;
    }

    buffer[lengthMimeType] = '\0';
    if (strcmp(Exporter::HEADER_MIMETYPE, buffer) != 0) {
        qDebug() << "CheckHeader failed because of wrong mimeType";
        MessageBox::errorFailedToOpenFile(fileName);
        return false;
    }

    file.skip(QString(Exporter::HEADER_SEPARATOR).size());

    auto lengthVersion =  QString(Exporter::HEADER_VERSION).size();
    if (file.read(buffer, lengthVersion) != lengthVersion) {
        qDebug() << "CheckHeader failed because of missing version";
        MessageBox::errorFailedToOpenFile(fileName);
        return false;
    }

    buffer[lengthVersion] = '\0';
    if (strcmp(Exporter::HEADER_VERSION, buffer) != 0) {
        qDebug() << "CheckHeader failed because of wrong version";
        MessageBox::errorFailedToOpenFile(fileName);
        return false;
    }

    file.skip(QString(Exporter::HEADER_SEPARATOR).size());
    return true;
}

void Importer::processData(QDataStream &dataStream, DbManagerInserter &inserter) {
    dataStream.setVersion(QDataStream::Qt_5_0);


    qint8 objectAvailable;
    dataStream >> objectAvailable;
    while (objectAvailable == Exporter::DATA_NEXT_OBJECT_EXISTS) {
        QString name;
        dataStream >> name;

        qDebug() << "Importing category: " << name;

        auto categoryId = inserter.addCategory(name);

        qDebug() << "Added category: " << name << " with id: " << categoryId;

        dataStream >> objectAvailable;
        while (objectAvailable == Exporter::DATA_NEXT_OBJECT_EXISTS) {
            QString description;
            int type;
            double value;

            dataStream >> description;
            dataStream >> type;
            dataStream >> value;

            qDebug() << "Importing entry: " << description << " for category with id: " << categoryId;

            auto entryId = inserter.addEntry(description, type, value, categoryId);

            qDebug() << "Added entry " << description << " with id " << entryId << " for category with id: " << categoryId;

            dataStream >> objectAvailable;
        }

        dataStream >> objectAvailable;
    }
}
