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


    model = new QSqlRelationalTableModel(ui->tableView);
    model->setTable(GlobalValues::SQL_TABLENAME_ENTRY);

    model->setHeaderData(model->fieldIndex(GlobalValues::SQL_COLUMNNAME_DESCRIPTION), Qt::Horizontal, QObject::tr("description"));
    model->setHeaderData(model->fieldIndex(GlobalValues::SQL_COLUMNNAME_TYPE), Qt::Horizontal, QObject::tr("schedule"));
    model->setHeaderData(model->fieldIndex(GlobalValues::SQL_COLUMNNAME_VALUE), Qt::Horizontal, QObject::tr("value"));

    ui->tableView->setModel(model);

    ui->tableView->setColumnHidden(model->fieldIndex(GlobalValues::SQL_COLUMNNAME_ID), true);
    ui->tableView->setColumnHidden(model->fieldIndex(GlobalValues::SQL_COLUMNNAME_CATEGORY_ID), true);

    if (!model->select()) {
        MessageBox::errorSQL(model->lastError(), this);
        return;
    }



    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit()));
}


void MainWindow::actionExit() {
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete model;
    delete ui;
}

