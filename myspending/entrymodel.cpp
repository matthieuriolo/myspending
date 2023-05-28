#include "entrymodel.h"

EntryModel::EntryModel(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent, db)
{}


int EntryModel::columnCount(const QModelIndex &parent) const {
    return QSqlTableModel::columnCount(parent) + 3;
}

/*
Qt::ItemFlags flags ( const QModelIndex & index ) const {
    if (index.column() == 1 || index.column() == 2)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
     else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
*/


QVariant EntryModel::data(const QModelIndex &item, int role) const {
qlonglong test = 1000;
    if (role == Qt::DisplayRole) {
        switch(item.column()) {
            case 5: return QVariant(QVariant::String, new QString("test 6"));
            case 6: return QVariant(QVariant::LongLong, &test);
            case 7: return QVariant(QVariant::String, new QString("test 8"));
        }
    }
    return QSqlTableModel::data(item, role);
}


QVariant EntryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        switch(section) {
            case 5: return QVariant(QVariant::String, new QString("name5"));
            case 6: return QVariant(QVariant::String, new QString("name6"));
            case 7: return QVariant(QVariant::String, new QString("name7"));
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

