#ifndef DBMANAGERINSERTER_H
#define DBMANAGERINSERTER_H
#include <QString>

class DbManagerInserter
{
public:
    virtual int addCategory(QString name) = 0;
    virtual int addEntry(QString description, int type, double value, int categoryId) = 0;
};

#endif // DBMANAGERINSERTER_H
