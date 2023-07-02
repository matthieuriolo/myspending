#ifndef ENTRYDELEGATE_H
#define ENTRYDELEGATE_H

#include <QStyledItemDelegate>

class EntryDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    EntryDelegate(QObject *parent, int indexScheduleColumn) : QStyledItemDelegate(parent), indexTypeColumn(indexScheduleColumn) {};

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
private:
    int indexTypeColumn;

private slots:
    void currentIndexChanged(int selectedIndex);
};

#endif // ENTRYDELEGATE_H
