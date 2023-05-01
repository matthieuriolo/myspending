#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DbManager.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    ui->splitter->setChildrenCollapsible(false);


//    model = new QSqlTableModel();
//    model->setTable("entries");

//    ui->tableView->setModel(model);

//    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
//    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
//    model->setHeaderData(2, Qt::Horizontal, QObject::tr("City"));
//    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Country"));



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

