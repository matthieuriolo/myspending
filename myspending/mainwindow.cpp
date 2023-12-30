#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagebox.h"
#include "entrydelegate.h"
#include "typeenum.h"

MainWindow::MainWindow(DbManager &dbManager, QWidget *parent)
    : QMainWindow(parent)
    , dbManager(dbManager)
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

    // setup entry view
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

    // setup toolbar
    toolbar = addToolBar(GlobalValues::APP_NAME);
    toolbar->setMovable(false);
    toolbar->addAction(toolbarActionNewCategory = new QAction(QIcon(":/myspending/category-add.png"), QCoreApplication::translate("MainWindow", "New category", nullptr)));
    toolbar->addAction(toolbarActionDeleteCategory = new QAction(QIcon(":/myspending/category-remove.png"), QCoreApplication::translate("MainWindow", "Delete category", nullptr)));
    toolbar->addAction(toolbarActionNewEntry = new QAction(QIcon(":/myspending/entry-add.png"), QCoreApplication::translate("MainWindow", "New entry", nullptr)));
    toolbar->addAction(toolbarActionDeleteEntry = new QAction(QIcon(":/myspending/entry-remove.png"), QCoreApplication::translate("MainWindow", "delete entry", nullptr)));

    // pre initialize selection & CO
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

    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(actionExport()));


    connect(modelEntry, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(entryModelChanged()));

    connect(toolbarActionNewCategory, SIGNAL(triggered(bool)), this, SLOT(actionNewCategory()));
    connect(toolbarActionDeleteCategory, SIGNAL(triggered(bool)), this, SLOT(actionDeleteCategory()));
    connect(toolbarActionNewEntry, SIGNAL(triggered(bool)), this, SLOT(actionNewEntry()));
    connect(toolbarActionDeleteEntry, SIGNAL(triggered(bool)), this, SLOT(actionDeleteEntry()));
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
        toolbarActionDeleteCategory->setEnabled(true);
        toolbarActionNewEntry->setEnabled(true);
    } else {
        ui->actionDelete_category->setEnabled(false);
        ui->actionNew_entry->setEnabled(false);
        toolbarActionDeleteCategory->setEnabled(false);
        toolbarActionNewEntry->setEnabled(false);
    }

    ui->actionDelete_entry->setEnabled(false);
    toolbarActionDeleteEntry->setEnabled(false);
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
        toolbarActionDeleteEntry->setEnabled(false);
    } else {
        ui->actionDelete_entry->setEnabled(true);
        toolbarActionDeleteEntry->setEnabled(true);
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

void MainWindow::actionExport() {
    // TODO
    auto fileName = QFileDialog::getSaveFileName(this);

    if (!fileName.isEmpty()) {
        dbManager.exportTo(fileName);
    }
}


void MainWindow::actionImport() {
    // TODO
//    auto fileName = QFileDialog::getOpenFileName(this);

//    if (!fileName.isEmpty()) {
//        dbManager.exportTo(fileName);
//    }
}

void MainWindow::entryModelChanged() {
    recalculateTotalSchedule();
}

void MainWindow::recalculateTotalSchedule() {
    auto currentIndex = ui->categoryView->currentIndex();
    auto categoryIdIndex = modelCategory->index(currentIndex.row(), modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID));
    auto sumDaily = dbManager.sumDailyValues(categoryIdIndex.data().toInt());

    ui->labelDaily->setText(QLocale().toCurrencyString(TypeSchedulers.at(TypeSchedulerDaily).convertToSameUnit(sumDaily), " "));
    ui->labelWeekly->setText(QLocale().toCurrencyString(TypeSchedulers.at(TypeSchedulerWeekly).convertToSameUnit(sumDaily), " "));
    ui->labelMonthly->setText(QLocale().toCurrencyString(TypeSchedulers.at(TypeSchedulerMonthly).convertToSameUnit(sumDaily), " "));
    ui->labelYearly->setText(QLocale().toCurrencyString(TypeSchedulers.at(TypeSchedulerYearly).convertToSameUnit(sumDaily), " "));
}

MainWindow::~MainWindow()
{
    delete toolbar;
    delete toolbarActionNewCategory;
    delete toolbarActionDeleteCategory;
    delete toolbarActionNewEntry;
    delete toolbarActionDeleteEntry;

    delete modelCategory;
    delete modelEntry;
    delete ui;
}

