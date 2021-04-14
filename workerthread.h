#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QtCore>
#include <windows.h>
using namespace std;


class WorkerThread: public QThread
{
  Q_OBJECT
public:
  WorkerThread();
  void run();
  int value;
  string nameFile;
  void setNameFile(string name);
 void exportRegistryAndPrintRun(HKEY key, LPCSTR keyName, string parentHKEYName, fstream &myFile);
 void EnumerateValuesRun(HKEY hKey, DWORD cValues, DWORD lLongestDataLen,  fstream &myFile);
 void startPointToExportRegistryRun(LPCSTR parentHKEYName,  LPCSTR subKeyName, fstream &myFile);
 int getSumAllRegistryInFile(string nameFile);
 void FormatDataWithDataTypeRun(DWORD dwKeyType, LPBYTE pbbinKeyData, DWORD dwKeyDataLength, string& cstrOutput);

signals:
  void signalForChangeValue(int);
  void messageAboutOperation(QString);

};

#endif // WORKERTHREAD_H
