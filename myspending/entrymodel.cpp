#include "entrymodel.h"
#include "globals.h"

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

    auto value = valueIndex.data(Qt::DisplayRole).toLongLong();
    auto type = typeIndex.data(Qt::DisplayRole).toInt();

    // TODO - thats nasty ...
    switch (type) {
        case 1: return value;
        case 2: return value / 7;
        case 3: return value / 7 / 4;
        case 4: return value / 7 / 4 / 12;
    }

    return 0;
}


QVariant EntryModel::data(const QModelIndex &item, int role) const {
    if (role == Qt::DisplayRole) {
        if (
                item.column() == indexColumnDaily ||
                item.column() == indexColumnWeekly ||
                item.column() == indexColumnMonthly ||
                item.column() == indexColumnYearly
         ) {
            auto daily = calculateDailyValue(item);
            if (item.column() == indexColumnDaily) {
                return QVariant(daily);
            } else if (item.column() == indexColumnWeekly) {
                return QVariant(daily * 7);
            } else if (item.column() == indexColumnMonthly) {
                return QVariant(daily * 7 * 4);
            } else if (item.column() == indexColumnYearly) {
                return QVariant(daily * 7 * 4 * 12);
            }
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
