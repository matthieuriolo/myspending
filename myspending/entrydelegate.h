#ifndef ENTRYDELEGATE_H
#define ENTRYDELEGATE_H

#include <QSqlRelationalDelegate>

class EntryDelegate : public QSqlRelationalDelegate
{
public:
    EntryDelegate(QObject *parent);

   void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ENTRYDELEGATE_H
