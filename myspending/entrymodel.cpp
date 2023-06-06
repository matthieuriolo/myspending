#include "entrymodel.h"

EntryModel::EntryModel(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent, db)
{}


int EntryModel::columnCount(const QModelIndex &parent) const {
    return QSqlTableModel::columnCount(parent) + 4;
}

void EntryModel::setTable(const QString &tableName) {
    QSqlTableModel::setTable(tableName);

    indexColumnDaily = QSqlTableModel::columnCount();
    indexColumnWeekly = indexColumnDaily + 1;
    indexColumnMonthly = indexColumnWeekly + 1;
    indexColumnYearly = indexColumnMonthly + 1;
}


Qt::ItemFlags EntryModel::flags( const QModelIndex & index ) const {
    if (
            index.column() == indexColumnDaily ||
            index.column() == indexColumnWeekly ||
            index.column() == indexColumnMonthly ||
            index.column() == indexColumnYearly
     ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
     } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}


QVariant EntryModel::data(const QModelIndex &item, int role) const {
    if (role == Qt::DisplayRole) {
        if (item.column() == indexColumnDaily) {
            return QVariant(QVariant::String, new QString("test 6"));
        } else if (item.column() == indexColumnWeekly) {
            return QVariant(QVariant::String, new QString("quickdirty"));
        } else if (item.column() == indexColumnMonthly) {
            return QVariant(QVariant::String, new QString("test 8"));
        } else if (item.column() == indexColumnYearly) {
            return QVariant(QVariant::String, new QString("test 9"));
        }
    }
    return QSqlTableModel::data(item, role);
}


QVariant EntryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (section == indexColumnDaily) {
            return QVariant(QVariant::String, new QString("daily"));
        } else if (section == indexColumnWeekly) {
            return QVariant(QVariant::String, new QString("weekly"));
        } else if (section == indexColumnMonthly) {
            return QVariant(QVariant::String, new QString("monthly"));
        } else if (section == indexColumnYearly) {
            return QVariant(QVariant::String, new QString("yearly"));
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

