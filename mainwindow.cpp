#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "info.h"
#include "help.h"
#include <QPixmap>
#include<QFile>
#include <QtDebug>
#include<QTextStream>
#include<QMessageBox>
#include<QStandardItemModel>
#include <QPalette>
#include <QVBoxLayout>
#include <registryShell.h>
#include <drawRegistryShall.h>
#include <workerthread.h>
#include <QKeyEvent>

string nameOpenedFileForDelete;
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  worker = new WorkerThread();
  connect(worker, SIGNAL(signalForChangeValue(int)),
              this, SLOT(onValueChanged(int)));

  connect(worker, SIGNAL(messageAboutOperation(QString)),
              this, SLOT(onSuccessMessage(QString)));
  ui->setupUi(this);
  ui->progressBar->setValue(0);
ui->statusbar->addPermanentWidget(ui->progressBar);
}

MainWindow::~MainWindow()
{
  delete ui;
}
// cout<<"А файл то пуст :D"<<endl;f.close();
//AlbumArt values
  //AppEvents package
  //\AppEvents\EventLabels\.Default  ; defualt values exist
  // SOFTWARE\\Radmin\\v3.0\\Parameters ; hex values
//S-1-5-21-2657195928-4124521378-2452857898-1001_Classes\\.cap\\OpenWithProgids // Data Type REG_NONE
//S-1-5-21-2657195928-4124521378-2452857898-1001_Classes\\.class_auto_file\\shell\\edit\\command // DataType REG_EXPAND_SZ

void MainWindow::keyPressEvent(QKeyEvent *e){
  qDebug() << e->key();
  if (e->type()== QKeyEvent::KeyPress){
      if(e->matches(QKeySequence::Save))
        copyAllRegistry();
      else if(e->matches(QKeySequence::Print))
        drawRegistry();
      else if(e->matches(QKeySequence::HelpContents)){
          help help;
          help.exec();
        }
      else if (e->matches(QKeySequence::Italic)){
          Info info;
          info.exec();
        }
      else if(e->matches(QKeySequence::Open)){
          openFile();
        }
      else if(e->matches(QKeySequence::Delete)){
          deleteFile();
        }

    }


}

void MainWindow::on_pushButton_3_clicked()
{
 copyAllRegistry();

}

void MainWindow::on_pushButton_4_clicked()
{
exportPartRegistry();
}

void MainWindow::on_pushButton_2_clicked()
{
  QFile::remove(nameOpenedFileForDelete.c_str());

}
void MainWindow::on_pushButton_5_clicked()
{
  Info info;
  info.setModal(true);
  info.exec();

}


void MainWindow::on_pushButton_6_clicked()
{
  help help;
  help.exec();
}

void MainWindow::on_deleteFile_clicked()
{
deleteFile();
}

void MainWindow::on_drawTreeRegistry_clicked()
{
 drawRegistry();
}

void MainWindow::on_openFileCopyReg_clicked()
{
  openFile();

}

void MainWindow::on_ImportFile_triggered()
{
  openFile();
}

void MainWindow::on_Exit_triggered()
{
    this->close();
}

void MainWindow::on_SaveAllRegistry_triggered()
{
  copyAllRegistry();
}

void MainWindow::on_SavePartRegistry_triggered()
{
    exportPartRegistry();
}

void MainWindow::on_DrawTreeRegistry_triggered()
{
  drawRegistry();
}

void MainWindow::on_deleteFileFromMenu_triggered()
{
 deleteFile();
}

void MainWindow::on_Help_triggered()
{
  help help;
  help.exec();
}

void MainWindow::on_About_triggered()
{
  Info info;
  info.exec();
}
void MainWindow::onValueChanged(int count)
{
    ui->progressBar->setValue(count);
}

void MainWindow::onSuccessMessage(QString message){
  QMessageBox::information(this, tr("Операція завершена"), tr(message.toStdString().c_str()));
}
void MainWindow::copyAllRegistry(){
  QString key=ui->lineEdit->text();
  string convertedKey = key.toStdString();
  string nameFile = "D://"+convertedKey+".reg";
  if(convertedKey.length()>2){
      int a=0;
      ui->progressBar->setValue(5);
      worker->value = a;
      worker->setNameFile(nameFile);

      worker->start();

      for(int i=0;i<1000;i++){
          qDebug() << i<<"Frm Main";
      }
    } else{
      QMessageBox::warning(this, tr("Помилка"), tr("Назва файлу повинна бути більша ніж 2 символи"));
    }


}
void MainWindow::exportPartRegistry(){
    ui->progressBar->setValue(10);

    QString key=ui->lineEdit->text();
    string convertedKey = key.toStdString();
    QString hkey=ui->comboBox->currentText();

    string convertedHkey = hkey.toStdString();
    string nameFile = "D://"+convertedKey+".reg";
    fstream myFile (nameFile, ios::out);
    ui->progressBar->setValue(40);

    startPointToExportRegistry(convertedHkey.c_str(),ui->namePartKey->text().toStdString().c_str(),myFile);
    ui->progressBar->setValue(0);

}

void MainWindow::drawRegistry(){
  ui->progressBar->setValue(10);
  QStandardItemModel* model = new QStandardItemModel();
  QStandardItem* HKEY_CLASSES_ROOTparentItem = new QStandardItem("HKEY_CLASSES_ROOT");
  QStandardItem* HKEY_CURRENT_USERparentItem = new QStandardItem("HKEY_CURRENT_USER");
  QStandardItem* HKEY_LOCAL_MACHINEparentItem = new QStandardItem("HKEY_LOCAL_MACHINE");
  QStandardItem* HKEY_USERSparentItem = new QStandardItem("HKEY_USERS");
  QStandardItem* HKEY_CURRENT_CONFIGparentItem = new QStandardItem("HKEY_CURRENT_CONFIG");

  startDraw("HKEY_CLASSES_ROOT"
                 ,HKEY_CLASSES_ROOTparentItem);
  model->appendRow(HKEY_CLASSES_ROOTparentItem);
  ui->progressBar->setValue(30);

  startDraw("HKEY_CURRENT_USER"
                 ,HKEY_CURRENT_USERparentItem);
  model->appendRow(HKEY_CURRENT_USERparentItem);
  startDraw("HKEY_LOCAL_MACHINE"
                 ,HKEY_LOCAL_MACHINEparentItem);
  model->appendRow(HKEY_LOCAL_MACHINEparentItem);
   ui->progressBar->setValue(70);
  startDraw("HKEY_USERS"
                 ,HKEY_USERSparentItem);
  model->appendRow(HKEY_USERSparentItem);
   ui->progressBar->setValue(90);
  startDraw("HKEY_CURRENT_CONFIG"
                 ,HKEY_CURRENT_CONFIGparentItem);
  model->appendRow(HKEY_CURRENT_CONFIGparentItem);
  ui->treeView->setModel(model);

 ui->progressBar->setValue(0);
}

void MainWindow::openFile(){
  ui->progressBar->setValue(10);
  QString nameFile=ui->lineEdit->text();
  string convertedKey = nameFile.toStdString();
  string lastNameFile ="D://"+convertedKey+".reg";
  nameOpenedFileForDelete = lastNameFile;
  QFile file(lastNameFile.c_str());
  if(!file.open(QIODevice::ReadOnly)){
      QMessageBox::information(0, "Error", file.errorString());//show msg if file dont open file
    }

  QTextStream in(&file);
  ui->progressBar->setValue(30);

  ui->textBrowser->setText(in.readAll());
  ui->progressBar->setValue(0);
}

void MainWindow::deleteFile(){
  ui->progressBar->setValue(20);
  QString nameFile = ui->lineEdit->text();
  string convertedKey = nameFile.toStdString();
  string convertedNameFile = "D://"+convertedKey+".reg";
  QFile::remove(convertedNameFile.c_str());
  ui->progressBar->setValue(0);
}


