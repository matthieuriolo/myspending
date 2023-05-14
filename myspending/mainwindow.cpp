#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DbManager.h"
#include "messagebox.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->splitter->setChildrenCollapsible(false);


    modelCategory = new QSqlTableModel(ui->listView);
    modelCategory->setTable(GlobalValues::SQL_TABLENAME_CATEGORY);

    ui->listView->setModel(modelCategory);
    ui->listView->setModelColumn(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));
    ui->listView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    if (!modelCategory->select()) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    modelEntry = new QSqlRelationalTableModel(ui->tableView);
    modelEntry->setTable(GlobalValues::SQL_TABLENAME_ENTRY);

    modelEntry->setRelation(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), QSqlRelation(GlobalValues::SQL_TABLENAME_CATEGORY, GlobalValues::SQL_COLUMNNAME_ID, GlobalValues::SQL_COLUMNNAME_NAME));

    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION), Qt::Horizontal, QObject::tr("description"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE), Qt::Horizontal, QObject::tr("schedule"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE), Qt::Horizontal, QObject::tr("value"));

    ui->tableView->setModel(modelEntry);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    ui->tableView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID), true);
    ui->tableView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), true);

    if (!modelEntry->select()) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }

    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(categoryChanged(QItemSelection)));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit()));
}

void MainWindow::categoryChanged(QItemSelection currentSelection)
{
    if(currentSelection.isEmpty()) {
        modelEntry->setFilter(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(GlobalValues::SQL_VALUE_NONEXISTING_PK));
        return;
    }

    auto currentIndex = currentSelection.indexes().front();
    auto record = modelCategory->record(currentIndex.row());
    auto pk = record.value(GlobalValues::SQL_COLUMNNAME_ID).toLongLong();
    modelEntry->setFilter(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID + " = " + QString::number(pk));
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

