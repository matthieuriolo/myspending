#ifndef EXPORTER_H
#define EXPORTER_H

#include "dbmanager.h"

class Exporter : public DbManagerTraverser
{
public:
    Exporter();
    ~Exporter();

    void exportTo(DbManager &dbManager, QString fileName);

    void beforeTraversingCategories();
    void traverseCategory(QString name, int id);
    void afterTraversingCategories();

    void beforeTraversingEntries(int categoryId);
    void traverseEntry(QString description, int type, double value, int categoryId);
    void afterTraversingEntries(int categoryId);


    constexpr static auto HEADER_MIMETYPE = "application/myspending";
    constexpr static auto HEADER_VERSION = "1";
    constexpr static auto HEADER_SEPARATOR = "\n";
    constexpr static qint8 DATA_NEXT_OBJECT_EXISTS = 0x0;
    constexpr static qint8 DATA_NO_OTHER_OBJECT_EXISTS = 0x1;
private:

    QDataStream *dataStream = nullptr;
};

#endif // EXPORTER_H
