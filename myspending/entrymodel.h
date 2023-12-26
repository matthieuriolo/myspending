#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <list>
#include <QSqlTableModel>
#include "typescheduler.h"

class EntryModel : public QSqlTableModel
{

    Q_OBJECT

public:
    EntryModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    list<int> virtualValueColumns;
    void setTable(const QString &tableName) override;
    Qt::ItemFlags flags( const QModelIndex & index ) const override;
    QVariant data(const QModelIndex &item, int role) const override;
private slots:
    void beforeUpdate(int row, QSqlRecord &record);
};

#endif // ENTRYMODEL_H
