#include "entrymodel.h"
#include "globals.h"
#include "typeenum.h"
#include <QtSql>

EntryModel::EntryModel(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent, db)
{
    connect(this, SIGNAL(beforeUpdate(int, QSqlRecord&)), this, SLOT(beforeUpdate(int, QSqlRecord&)));
}


void EntryModel::setTable(const QString &tableName) {
    QSqlTableModel::setTable(tableName);

    virtualValueColumns.clear();
    virtualValueColumns.push_back(fieldIndex(GlobalValues::SQL_COLUMNNAME_DAILY));
    virtualValueColumns.push_back(fieldIndex(GlobalValues::SQL_COLUMNNAME_WEEKLY));
    virtualValueColumns.push_back(fieldIndex(GlobalValues::SQL_COLUMNNAME_MONTHLY));
    virtualValueColumns.push_back(fieldIndex(GlobalValues::SQL_COLUMNNAME_YEARLY));
}

Qt::ItemFlags EntryModel::flags( const QModelIndex & index ) const {
    auto flags = QSqlTableModel::flags(index);
    bool isVirtualValueColumn = (std::find(virtualValueColumns.begin(), virtualValueColumns.end(), index.column()) != virtualValueColumns.end());

    if (isVirtualValueColumn) {
        return flags & (~Qt::ItemIsEditable);
     } else {
        return flags;
    }
}

void EntryModel::beforeUpdate(int, QSqlRecord &record) {
    auto schedulerIndex = record.value(GlobalValues::SQL_COLUMNNAME_TYPE);
    auto value = record.value(GlobalValues::SQL_COLUMNNAME_VALUE);

    auto dailyValue = TypeSchedulers.at(schedulerIndex.toInt()).convertToSmallestUnit(value.toDouble());

    record.setValue(GlobalValues::SQL_COLUMNNAME_DAILY, QVariant(TypeSchedulers.at(TypeSchedulerDaily).convertToSameUnit(dailyValue)));
    record.setGenerated(GlobalValues::SQL_COLUMNNAME_DAILY, true);

    record.setValue(GlobalValues::SQL_COLUMNNAME_WEEKLY, QVariant(TypeSchedulers.at(TypeSchedulerWeekly).convertToSameUnit(dailyValue)));
    record.setGenerated(GlobalValues::SQL_COLUMNNAME_WEEKLY, true);

    record.setValue(GlobalValues::SQL_COLUMNNAME_MONTHLY, QVariant(TypeSchedulers.at(TypeSchedulerMonthly).convertToSameUnit(dailyValue)));
    record.setGenerated(GlobalValues::SQL_COLUMNNAME_MONTHLY, true);

    record.setValue(GlobalValues::SQL_COLUMNNAME_YEARLY, QVariant(TypeSchedulers.at(TypeSchedulerYearly).convertToSameUnit(dailyValue)));
    record.setGenerated(GlobalValues::SQL_COLUMNNAME_YEARLY, true);
}

QVariant EntryModel::data(const QModelIndex &item, int role) const {
    if (role == Qt::DisplayRole) {
        bool isVirtualValueColumn = (std::find(virtualValueColumns.begin(), virtualValueColumns.end(), item.column()) != virtualValueColumns.end());


        if (isVirtualValueColumn) {
            auto value = QSqlTableModel::data(item, role);
            return QVariant(QLocale().toCurrencyString(value.toDouble(), " "));
        }
    }

    return QSqlTableModel::data(item, role);
}
