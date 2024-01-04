#ifndef IMPORTER_H
#define IMPORTER_H

#include <QString>
#include <QFile>
#include "dbmanagerinserter.h"
class Importer
{
public:
    Importer(QString fileName);
    void operator()(DbManagerInserter &inserter);
private:
    QString fileName;
    bool checkHeader(QFile &file, QString fileName);
    void processData(QDataStream &dataStream, DbManagerInserter &inserter);
};

#endif // IMPORTER_H
