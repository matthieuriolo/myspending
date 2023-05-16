#include "entrydelegate.h"
#include <QDebug>

EntryDelegate::EntryDelegate(QObject *parent) : QSqlRelationalDelegate(parent) {
}

void EntryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    // Since we draw the grid ourselves:
    opt.rect.adjust(0, 0, -1, -1);
    QSqlRelationalDelegate::paint(painter, opt, index);
}
