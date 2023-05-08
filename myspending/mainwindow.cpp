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


    modelCategory = new QSqlRelationalTableModel(ui->listView);
    modelCategory->setTable(GlobalValues::SQL_TABLENAME_CATEGORY);

    ui->listView->setModel(modelCategory);
    ui->listView->setModelColumn(modelCategory->fieldIndex(GlobalValues::SQL_COLUMNNAME_NAME));

    if (!modelCategory->select()) {
        MessageBox::errorSQL(modelCategory->lastError(), this);
        return;
    }

    modelEntry = new QSqlRelationalTableModel(ui->tableView);
    modelEntry->setTable(GlobalValues::SQL_TABLENAME_ENTRY);

    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION), Qt::Horizontal, QObject::tr("description"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE), Qt::Horizontal, QObject::tr("schedule"));
    modelEntry->setHeaderData(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE), Qt::Horizontal, QObject::tr("value"));

    ui->tableView->setModel(modelEntry);

    ui->tableView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID), true);
    ui->tableView->setColumnHidden(modelEntry->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), true);

    if (!modelEntry->select()) {
        MessageBox::errorSQL(modelEntry->lastError(), this);
        return;
    }


    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit()));
}


void MainWindow::actionExit() {
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete modelEntry;
    delete ui;
}

