#include "entrydelegate.h"
#include "typeenum.h"
#include <QVariant>
#include <QApplication>
#include <QComboBox>
#include <QPainter>
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
        painter->save();

        QStyleOptionComboBox comboBoxOption;
        comboBoxOption.rect = option.rect;
        comboBoxOption.state = option.state;
        comboBoxOption.state |= QStyle::State_Enabled;
        comboBoxOption.editable = false;
        comboBoxOption.currentText = TypeSchedule.at(index.data().toInt()).c_str();

        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &comboBoxOption, painter, nullptr);
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter, nullptr);

        painter->restore();
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
