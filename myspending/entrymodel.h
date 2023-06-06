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

    double calculateDailyValue(const QModelIndex &item) const;

public:
    EntryModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    void setTable(const QString &tableName) override;
    Qt::ItemFlags flags( const QModelIndex & index ) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;

    int getFieldIndexDaily() const;
    int getFieldIndexWeekly() const;
    int getFieldIndexMonthly() const;
    int getFieldIndexYearly() const;
};

#endif // ENTRYMODEL_H
