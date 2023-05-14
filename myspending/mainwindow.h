#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtSql>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel* modelEntry;
    QSqlTableModel* modelCategory;



private slots:
    void actionExit();
    void categoryChanged(QItemSelection currentSelection);
};

#endif // MAINWINDOW_H
