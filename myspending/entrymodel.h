#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <QSqlTableModel>

class EntryModel : public QSqlTableModel
{

private:
    int indexColumnDaily = -1;
    int indexColumnWeekly = -1;
    int indexColumnMonthly = -1;
    int indexColumnYearly = -1;

public:
    EntryModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    void setTable(const QString &tableName) override;
    Qt::ItemFlags flags( const QModelIndex & index ) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // ENTRYMODEL_H
