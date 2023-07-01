#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtSql>
#include <QString>
#include "entrymodel.h"

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
    QSqlTableModel* modelCategory;
    EntryModel* modelEntry;

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
    void categorySelectionChanged(QItemSelection currentSelection);
    void entrySelectionChanged(QItemSelection currentSelection);
    void entryModelChanged();
};

#endif // MAINWINDOW_H
