#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtSql>
#include <QString>
#include "entrymodel.h"
#include "dbmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(DbManager& dbManager, QWidget *parent = nullptr);
    ~MainWindow();

private:
    DbManager& dbManager;
    Ui::MainWindow *ui;
    QSqlTableModel* modelCategory;
    EntryModel* modelEntry;
    QToolBar* toolbar;
    QAction* toolbarActionNewCategory;
    QAction* toolbarActionDeleteCategory;
    QAction* toolbarActionNewEntry;
    QAction* toolbarActionDeleteEntry;

    void preselectFirstCategory();
    void setupEntryTableColumnStretching();
    void selectCategory(QModelIndex* index);
    void recalculateTotalSchedule();

private slots:
    void actionExit();

    void actionNewCategory();
    void actionDeleteCategory();
    void actionNewEntry();
    void actionDeleteEntry();

    void actionExport();
    void actionImport();

    void categorySelectionChanged(QItemSelection currentSelection);
    void entrySelectionChanged(QItemSelection currentSelection);
    void entryModelChanged();
};

#endif // MAINWINDOW_H
