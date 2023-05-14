#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DbManager.h"
#include "messagebox.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // setup UI behaviour
    ui->setupUi(this);
    ui->splitter->setChildrenCollapsible(false);

    // setup category view
    modelCategory = new QSqlTableModel(ui->categoryView);
    modelCategory->setTable(GlobalValues::SQL_TABLENAME_CATEGORY);

    ui->categoryView->setModel(modelCategory);
    ui->categoryView->setModelColumn(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));
    ui->categoryView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    if (!modelCategory->select()) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    // setup entry view
    modelEntry = new QSqlRelationalTableModel(ui->entryView);
    modelEntry->setTable(GlobalValues::SQL_TABLENAME_ENTRY);

    modelEntry->setRelation(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), QSqlRelation(GlobalValues::SQL_TABLENAME_CATEGORY, GlobalValues::SQL_COLUMNNAME_ID, GlobalValues::SQL_COLUMNNAME_NAME));

    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION), Qt::Horizontal, QObject::tr("description"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE), Qt::Horizontal, QObject::tr("schedule"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE), Qt::Horizontal, QObject::tr("value"));

    ui->entryView->setModel(modelEntry);
    ui->entryView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    ui->entryView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID), true);
    ui->entryView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), true);

    if (!modelEntry->select()) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }

    preselectFirstCategory();

    // setup slots
    connect(ui->categoryView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(categorySelectionChanged(QItemSelection)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit()));
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

void MainWindow::selectCategory(QModelIndex* index)
{
    qlonglong pk = GlobalValues::SQL_VALUE_NONEXISTING_PK;
    if (index != nullptr) {
        auto record = modelCategory->record(index->row());
        pk = record.value(GlobalValues::SQL_COLUMNNAME_ID).toLongLong();
    }
    modelEntry->setFilter(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(pk));
}

void MainWindow::categorySelectionChanged(QItemSelection currentSelection)
{
    if(currentSelection.isEmpty()) {
        selectCategory(nullptr);
    } else {
        selectCategory(&currentSelection.indexes().front());
    }
}

void MainWindow::actionExit() {
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete modelCategory;
    delete modelEntry;
    delete ui;
}

