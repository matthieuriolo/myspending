#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include "messagebox.h"
#include "entrydelegate.h"
#include "typeenum.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // setup UI behaviour
    ui->setupUi(this);
    ui->splitter->setChildrenCollapsible(false);

    ui->actionDelete_category->setEnabled(false);
    ui->actionDelete_entry->setEnabled(false);
    ui->actionNew_entry->setEnabled(false);


    // setup category view
    modelCategory = new QSqlTableModel(ui->categoryView);
    modelCategory->setTable(GlobalValues::SQL_TABLENAME_CATEGORY);
    modelCategory->setEditStrategy(QSqlTableModel::OnFieldChange);

    ui->categoryView->setModel(modelCategory);
    ui->categoryView->setModelColumn(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));
    ui->categoryView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    if (!modelCategory->select()) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    modelEntry = new EntryModel(ui->entryView);
    modelEntry->setTable(GlobalValues::SQL_TABLENAME_ENTRY);
    modelEntry->setEditStrategy(QSqlTableModel::OnFieldChange);

    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION), Qt::Horizontal, QObject::tr("description"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE), Qt::Horizontal, QObject::tr("schedule"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE), Qt::Horizontal, QObject::tr("value"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DAILY), Qt::Horizontal, QObject::tr("daily"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_WEEKLY), Qt::Horizontal, QObject::tr("weekly"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_MONTHLY), Qt::Horizontal, QObject::tr("monthly"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_YEARLY), Qt::Horizontal, QObject::tr("yearly"));

    ui->entryView->setModel(modelEntry);
    ui->entryView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    ui->entryView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID), true);
    ui->entryView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), true);

    if (!modelEntry->select()) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }

    ui->entryView->setItemDelegate(new EntryDelegate(ui->entryView, modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE)));

    preselectFirstCategory();
    setupEntryTableColumnStretching();
    recalculateTotalSchedule();

    // setup slots
    connect(ui->categoryView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(categorySelectionChanged(QItemSelection)));
    connect(ui->entryView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(entrySelectionChanged(QItemSelection)));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit()));
    connect(ui->actionNew_category, SIGNAL(triggered()), this, SLOT(actionNewCategory()));
    connect(ui->actionDelete_category, SIGNAL(triggered()), this, SLOT(actionDeleteCategory()));
    connect(ui->actionNew_entry, SIGNAL(triggered()), this, SLOT(actionNewEntry()));
    connect(ui->actionDelete_entry, SIGNAL(triggered()), this, SLOT(actionDeleteEntry()));

    connect(modelEntry, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(entryModelChanged()));
}


void MainWindow::preselectFirstCategory()
{
    if (modelCategory->rowCount()) {
        ui->categoryView->setCurrentIndex(modelCategory->index(0, modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME)));
        auto idx = ui->categoryView->currentIndex();
        selectCategory(&idx);
    } else {
        selectCategory(nullptr);
    }
}

void MainWindow::setupEntryTableColumnStretching() {
    for (auto c = 0; c < ui->entryView->horizontalHeader()->count(); c++) {
        auto stretching = QHeaderView::ResizeToContents;
        if (c == modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION)) {
            stretching = QHeaderView::Stretch;
        }
        ui->entryView->horizontalHeader()->setSectionResizeMode(c, stretching);
    }
}

void MainWindow::selectCategory(QModelIndex* index)
{
    qlonglong pk = GlobalValues::SQL_VALUE_NONEXISTING_PK;
    if (index != nullptr) {
        auto record = modelCategory->record(index->row());
        pk = record.value(GlobalValues::SQL_COLUMNNAME_ID).toLongLong();
        ui->actionDelete_category->setEnabled(true);
        ui->actionNew_entry->setEnabled(true);
    } else {
        ui->actionDelete_category->setEnabled(false);
        ui->actionNew_entry->setEnabled(false);
    }

    ui->actionDelete_entry->setEnabled(false);
    modelEntry->setFilter(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(pk));
}

void MainWindow::categorySelectionChanged(QItemSelection currentSelection)
{
    if(currentSelection.isEmpty()) {
        selectCategory(nullptr);
    } else {
        selectCategory(&currentSelection.indexes().front());
    }

    recalculateTotalSchedule();
}

void MainWindow::entrySelectionChanged(QItemSelection currentSelection)
{
    if(currentSelection.isEmpty()) {
        ui->actionDelete_entry->setEnabled(false);
    } else {
        ui->actionDelete_entry->setEnabled(true);
    }
}

void MainWindow::actionExit() {
    QApplication::quit();
}

void MainWindow::actionNewCategory() {
    auto record = modelCategory->record();
    record.setValue(GlobalValues::SQL_COLUMNNAME_NAME, QVariant(QObject::tr("New Category")));
    if(!modelCategory->insertRecord(-1, record)) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    if(!modelCategory->select()) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    auto idx = modelCategory->index(modelCategory->rowCount() - 1, modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));
    ui->categoryView->setCurrentIndex(idx);
    selectCategory(&idx);
}

void MainWindow::actionDeleteCategory() {
    auto selection = ui->categoryView->selectionModel()->currentIndex();

    if (selection.isValid()) {
        if(!modelCategory->removeRow(selection.row())) {
            MessageBox::errorSQL(modelCategory->lastError(), this);
            return;
        }

        if(!modelCategory->select()) {
            MessageBox::errorSQL(modelCategory->lastError(), this);
            return;
        }

        if (modelCategory->rowCount() > 0) {
            auto newIndex = modelCategory->index(max(selection.row() - 1, 0), modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));

            if (newIndex.isValid()) {
                ui->categoryView->setCurrentIndex(newIndex);
                selectCategory(&newIndex);
            } else {
                preselectFirstCategory();
            }
        } else {
            preselectFirstCategory();
        }
    }
}

void MainWindow::actionNewEntry() {
    auto idx = ui->categoryView->currentIndex();
    auto categoryIndex = modelCategory->index(idx.row(), modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID));
    auto categoryId = modelCategory->data(categoryIndex);

    auto record = modelEntry->record();
    record.setValue(GlobalValues::SQL_COLUMNNAME_DESCRIPTION, QVariant(QObject::tr("New Entry")));
    record.setValue(GlobalValues::SQL_COLUMNNAME_VALUE, QVariant(0.0));
    record.setValue(GlobalValues::SQL_COLUMNNAME_TYPE, QVariant(0));
    record.setValue(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID, categoryId);

    if(!modelEntry->insertRecord(-1, record)) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }
    if(!modelEntry->select()) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }

    auto newIdx = modelEntry->index(modelEntry->rowCount() - 1, modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION));
    ui->entryView->setCurrentIndex(newIdx);
}

void MainWindow::actionDeleteEntry() {
    auto selection = ui->entryView->selectionModel()->currentIndex();

    if (selection.isValid()) {
        if (!modelEntry->removeRow(selection.row())) {
            MessageBox::errorSQL(modelEntry->lastError(), this);
            return;
        }

        if (!modelEntry->select()) {
            MessageBox::errorSQL(modelEntry->lastError(), this);
            return;
        }

        if (modelCategory->rowCount() > 0) {
            auto newIndex = modelEntry->index(max(selection.row() - 1, 0), modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION));

            if (newIndex.isValid()) {
                ui->entryView->setCurrentIndex(newIndex);
            }
        }
    }
}

void MainWindow::entryModelChanged() {
    recalculateTotalSchedule();
}

void MainWindow::recalculateTotalSchedule() {
// TODO
//    auto sumDailies = 0.0;
//    for (int rowIndex = 0; rowIndex < modelEntry->rowCount(); ++rowIndex) {
//        QModelIndex index = modelEntry->index(rowIndex, modelEntry->getFieldIndexDaily());
//        sumDailies += index.data().toDouble();
//    }

//    ui->labelDaily->setText(QLocale().toCurrencyString(TypeSchedulers.at(1).convertToSameUnit(sumDailies), " "));
//    ui->labelWeekly->setText(QLocale().toCurrencyString(TypeSchedulers.at(2).convertToSameUnit(sumDailies), " "));
//    ui->labelMonthly->setText(QLocale().toCurrencyString(TypeSchedulers.at(3).convertToSameUnit(sumDailies), " "));
//    ui->labelYearly->setText(QLocale().toCurrencyString(TypeSchedulers.at(4).convertToSameUnit(sumDailies), " "));
}

MainWindow::~MainWindow()
{
    delete modelCategory;
    delete modelEntry;
    delete ui;
}

