#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DbManager.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->splitter->setChildrenCollapsible(false);


    model = new QSqlRelationalTableModel(ui->tableView);
    model->setTable(GlobalValues::SQL_TABLENAME_ENTRIES);

    ui->tableView->setColumnHidden(model->fieldIndex("id"), true);
    model->setHeaderData(model->fieldIndex("type"), Qt::Horizontal, QObject::tr("Interval"));

//    if (!model->select()) {
//        DbManger::showError(model->lastError());
//        return;
//    }

    ui->tableView->setModel(model);


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

