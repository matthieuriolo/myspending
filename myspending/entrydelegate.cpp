#include "entrydelegate.h"
#include "typeenum.h"
#include <QVariant>
#include <QApplication>
#include <QComboBox>
#include <QPainter>
#include <QMouseEvent>
#include <QAbstractItemView>
#include <QTimer>

QWidget* EntryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == indexTypeColumn) {
        QComboBox* comboBox = new QComboBox(parent);
        return comboBox;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void EntryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == indexTypeColumn) {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        for(auto const &val : TypeSchedule) {
            comboBox->addItem(QObject::tr(val.second.c_str()), QVariant(QVariant::LongLong, &val.first));
        }
        comboBox->setCurrentIndex(comboBox->findData(index.data()));
        QTimer::singleShot(0, comboBox, &QComboBox::showPopup);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

bool EntryDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (index.column() == indexTypeColumn) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QAbstractItemView* itemView = qobject_cast<QAbstractItemView*>( const_cast<QWidget*>(option.widget));
                if(itemView != nullptr) {
                    emit itemView->edit(index);
                }
                return true;
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
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
