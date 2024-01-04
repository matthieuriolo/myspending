#ifndef DATATRAVERSER_H
#define DATATRAVERSER_H

#include <QString>

class DataTraverser {
    virtual void beforeTraversingCategories() = 0;
    virtual void traverseCategory(QString name) = 0;
    virtual void afterTraversingCategories() = 0;

    virtual void traverseEntry(QString description, int type, double value) = 0;
};

#endif // DATATRAVERSER_H
