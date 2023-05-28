#include "entrydelegate.h"
#include "typeenum.h"
#include <QVariant>
#include <QApplication>
#include <QComboBox>
#include <QDebug>

QWidget* EntryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == indexTypeColumn) {
        QComboBox* comboBox = new QComboBox(parent);

        for(auto const &val : TypeSchedule) {
            comboBox->addItem(QObject::tr(val.second.c_str()), QVariant(QVariant::LongLong, &val.first));
        }

        // TODO
        //comboBox->setGeometry(option.rect);
        //comboBox->showPopup();
        return comboBox;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void EntryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == indexTypeColumn) {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findData(index.data()));
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}


void EntryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == indexTypeColumn) {
        QStyleOptionViewItem myOption = option;
        QString text = TypeSchedule.at(index.data().toInt()).c_str();
        myOption.text = text;
        // TODO
        //QApplication::style()->drawComplexControl(CC_ComboBox, )
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
    }else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void EntryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    if (index.column() == indexTypeColumn) {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentData());
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
