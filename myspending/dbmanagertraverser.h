#ifndef DBMANAGERTRAVERSER_H
#define DBMANAGERTRAVERSER_H

#include <QString>

class DbManagerTraverser {
public:
    virtual void beforeTraversingCategories() = 0;
    virtual void traverseCategory(QString name, int id) = 0;
    virtual void afterTraversingCategories() = 0;

    virtual void beforeTraversingEntries(int categoryId) = 0;
    virtual void traverseEntry(QString description, int type, double value, int categoryId) = 0;
    virtual void afterTraversingEntries(int categoryId) = 0;
};

#endif // DBMANAGERTRAVERSER_H
