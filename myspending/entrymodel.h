#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <QSqlTableModel>

class EntryModel : public QSqlTableModel
{
public:
    EntryModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // ENTRYMODEL_H
