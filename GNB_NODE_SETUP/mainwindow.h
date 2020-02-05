#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QListWidget *listWidget;


private:
    void showList();


private slots:

    void on_createButton_clicked();
    void on_cleanButton_clicked();
    void on_exportButton_clicked();


    void onListItemClicked(QListWidgetItem* item);
    void onListItemDblClicked(QListWidgetItem* item);

};

#endif // MAINWINDOW_H


