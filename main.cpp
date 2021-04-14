#include "mainwindow.h"
#include <windows.h>
#include <string.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <QApplication>

#define _CRT_SECURE_NO_DEPRECATE
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;

int main(int argc, char *argv[])
{
 // startPointToExportRegistry("HKEY_CURRENT_USER", "SOFTWARE\\Radmin\\v3.0\\Parameters");
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}

