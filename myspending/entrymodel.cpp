#include "entrymodel.h"
#include "globals.h"
#include "typeenum.h"

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
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
     } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}


double EntryModel::calculateDailyValue(const QModelIndex &item) const {
    auto valueIndex = createIndex(item.row(), this->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE));
    auto typeIndex = createIndex(item.row(), this->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE));

    auto value = valueIndex.data(Qt::DisplayRole).toDouble();
    auto type = typeIndex.data(Qt::DisplayRole).toInt();

    return TypeSchedulers.at(type).convertToSmallestUnit(value);
}

TypeScheduler EntryModel::mapIndexColumnToTypeScheduler(int column) const {
    if(column == indexColumnDaily) {
        return TypeSchedulers.at(1);
    } else if(column == indexColumnWeekly) {
        return TypeSchedulers.at(2);
    } else if(column == indexColumnMonthly) {
        return TypeSchedulers.at(3);
    } else if(column == indexColumnYearly) {
        return TypeSchedulers.at(4);
    }

    // fallback to NONE
    return TypeSchedulers.at(0);
}

QVariant EntryModel::data(const QModelIndex &item, int role) const {
    if (role == Qt::DisplayRole) {
        if (
                item.column() == indexColumnDaily ||
                item.column() == indexColumnWeekly ||
                item.column() == indexColumnMonthly ||
                item.column() == indexColumnYearly
         ) {
            auto scheduler = mapIndexColumnToTypeScheduler(item.column());
            auto daily = calculateDailyValue(item);
            return QVariant(scheduler.convertToSmallestUnit(daily));
        }
    }
    return QSqlTableModel::data(item, role);
}

int EntryModel::getFieldIndexDaily() const {
    return indexColumnDaily;
}

int EntryModel::getFieldIndexWeekly() const {
    return indexColumnWeekly;
}

int EntryModel::getFieldIndexMonthly() const {
    return indexColumnMonthly;
}

int EntryModel::getFieldIndexYearly() const {
    return indexColumnYearly;
}
