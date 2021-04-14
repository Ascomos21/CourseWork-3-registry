#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <workerthread.h>
#include <QMainWindow>
#include <QSystemTrayIcon>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  WorkerThread *worker;
  void copyAllRegistry();

  void exportPartRegistry();

  void drawRegistry();

  void openFile();

  void deleteFile();

  void keyPressEvent(QKeyEvent *e);

private slots:


  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_openFileCopyReg_clicked();

  void on_ImportFile_triggered();

  void on_deleteFile_clicked();

  void on_drawTreeRegistry_clicked();

  void on_Exit_triggered();

  void on_SaveAllRegistry_triggered();

  void on_SavePartRegistry_triggered();

  void on_DrawTreeRegistry_triggered();

  void on_deleteFileFromMenu_triggered();

  void on_Help_triggered();

  void on_About_triggered();

private:
  Ui::MainWindow *ui;

public slots:
    void onValueChanged(int);
    void onSuccessMessage(QString);

};
#endif // MAINWINDOW_H
