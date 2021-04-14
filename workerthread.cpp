#include "workerthread.h"
#include <QtDebug>
#include <string.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <regex>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;

fstream myFile;
WorkerThread::WorkerThread(){}

static long checkSum=0;

int WorkerThread::getSumAllRegistryInFile(string nameFile){
  long sumInFile=0;
  string item_name;
  ifstream nameFileout;
  nameFileout.open(nameFile);
  string line;
  if (nameFileout.is_open())
    {
      while (! nameFileout.eof() )
      {
        getline (nameFileout,line);
        sumInFile += line.length()+1;
      }
      nameFileout.close();
    }
  sumInFile--;
  qDebug()<<checkSum;
  qDebug()<<sumInFile;
  sumInFile -=checkSum;
  qDebug() <<checkSum;
  checkSum =0;
  return sumInFile;
}
void WorkerThread::setNameFile(string nameFile){
    WorkerThread::nameFile = nameFile;
    myFile.open(nameFile, ios::out);
}

void WorkerThread::run(){
  qDebug() << "start";
  qDebug() << value<<"VAAALUES";

  if(value==0){
  qDebug() <<"start export"<<1;
  startPointToExportRegistryRun("HKEY_CLASSES_ROOT", "",myFile);
    }
  value++;
   emit signalForChangeValue(value*20);
  if(value==1){
        qDebug() <<"start export"<<2;
  startPointToExportRegistryRun("HKEY_CURRENT_USER", "",myFile);
  }
  value++;

  emit signalForChangeValue(value*20);
  if(value==2)
  startPointToExportRegistryRun("HKEY_LOCAL_MACHINE", "",myFile);
  value++;

  emit signalForChangeValue(value*20);
  if(value==3)
  startPointToExportRegistryRun("HKEY_USERS", "",myFile);
  value++;

  emit signalForChangeValue(value*25-1);
  if(value==4)
  startPointToExportRegistryRun("HKEY_CURRENT_CONFIG", "",myFile);

      myFile.close();
      qDebug() <<"end export";


  for(int i=0;i<50;i++){
  qDebug() <<"end";
    }

  emit signalForChangeValue(0);
  int result = getSumAllRegistryInFile(nameFile);
  QString messageAboutOperation;
  if(result == 0){
    messageAboutOperation= "Копію успішно створено";
  } else{
   messageAboutOperation = "Копія не цілісна";
  }

  emit WorkerThread::messageAboutOperation(messageAboutOperation);

}

void WorkerThread::startPointToExportRegistryRun(LPCSTR parentHKEYNameLPCSTR,  LPCSTR subKeyName,  fstream &myFile){
  qDebug()<<"start";
  long file_size;
  string startFile = "Windows Registry Editor Version 5.00 \n" ;
  myFile.seekg(0, ios::end);
  file_size = myFile.tellg();
  if (file_size == 0){

    myFile << startFile ;
    checkSum +=startFile.length();

    }

  HKEY phkey = HKEY_CLASSES_ROOT;
  string parentHKEYName = parentHKEYNameLPCSTR;
  if( parentHKEYName == "HKEY_CLASSES_ROOT"){
      phkey = HKEY_CLASSES_ROOT;
    }
  if(parentHKEYName == "HKEY_CURRENT_USER"){
      phkey = HKEY_CURRENT_USER;
    }
  if(parentHKEYName == "HKEY_LOCAL_MACHINE"){
      phkey = HKEY_LOCAL_MACHINE;
    }
  if(parentHKEYName =="HKEY_USERS"){
      phkey = HKEY_USERS;
    }
  if(parentHKEYName == "HKEY_CURRENT_CONFIG"){
      phkey = HKEY_CURRENT_CONFIG;
    }


  LPCSTR subKey = subKeyName;
  long hkey = RegOpenKeyExA(phkey, //HKEY
                            subKey, //subKey
                            0, // operation (can be 0)
                            KEY_ALL_ACCESS, //registry secucity (righsts access)
                            &phkey);
  subKey = "";
  string subKeyNameString = subKeyName;
  if (subKeyNameString.length()>0){
  parentHKEYName +="\\"+subKeyNameString;
    }
  if (hkey == ERROR_SUCCESS){
    WorkerThread work;
    work.exportRegistryAndPrintRun(phkey, subKey, parentHKEYName,myFile);
   }
}

void WorkerThread::EnumerateValuesRun(HKEY hKey, DWORD cValues, DWORD lLongestDataLen,  fstream &myFile) {

    string cstrFinalData, cstrTemp;
    DWORD i, retCode;
    LPBYTE lpData = new BYTE[lLongestDataLen];
    char tcKeyName[MAX_VALUE_NAME];
        for (i = 0; i < cValues; i++)
        {
            DWORD dataType;
            DWORD dataSize =MAX_VALUE_NAME;
            DWORD cchValue = MAX_VALUE_NAME;

            retCode = RegEnumValueA(
                hKey, // Handle to an open key
                i, // Index of value
                tcKeyName, // Value name
                &cchValue, // Buffer for value name
                0, // Reserved
                &dataType, // Value type
                lpData, // Value data
                &dataSize); // Buffer for value data
            if (retCode == ERROR_SUCCESS)
            {
                FormatDataWithDataTypeRun(dataType, lpData, dataSize, cstrFinalData);
                char buffer[MAX_VALUE_NAME];
                string result;
                LPCSTR stringToCout = tcKeyName;

                result = stringToCout;
                if (result.empty())
                {
                    cstrTemp = "@ = ";
                }
                else
                {
                    sprintf(buffer, "\"%s\" = ", stringToCout);
                    cstrTemp = buffer;
                }
                cstrTemp += cstrFinalData;
                checkSum +=cstrTemp.length();
                myFile << cstrTemp;
             }
        }
}
void WorkerThread::FormatDataWithDataTypeRun(DWORD dwKeyType, LPBYTE pbbinKeyData, DWORD dwKeyDataLength, string& cstrOutput){
    string cstrTemp, cstrTemp1;
    char buffer[MAX_VALUE_NAME];
    switch (dwKeyType)
    {
    case REG_SZ:
    {
        sprintf(buffer, "\"%s\"\n", pbbinKeyData);
       // cstrTemp.Format(_T("\"%s\"\n"), pbbinKeyData);
        cstrOutput = buffer;
        break;
    }

    case REG_DWORD: /// same is for REG_DWORD_LITTLE_ENDIAN
    {
        DWORD dwValue;
        memcpy(&dwValue, pbbinKeyData, sizeof (DWORD));

        sprintf(buffer, "dword:%08x\n", dwValue);

        cstrOutput = buffer;
        break;
    }

    case REG_BINARY:
    case REG_MULTI_SZ:
      case REG_EXPAND_SZ:
    case REG_FULL_RESOURCE_DESCRIPTOR:
    case REG_RESOURCE_LIST:
    case REG_RESOURCE_REQUIREMENTS_LIST:
    {
        if (dwKeyType != REG_BINARY) {
            sprintf(buffer, "hex(%d):", dwKeyType);
            cstrOutput = buffer;
        }
        else {
            sprintf(buffer, "hex:");
            cstrOutput = buffer;
        }
        for (DWORD dwIndex = 0; dwIndex < dwKeyDataLength; dwIndex++)
        {
            sprintf(buffer, "%02x,", pbbinKeyData[dwIndex]);
            cstrTemp1 = buffer;
            cstrTemp += cstrTemp1;
            if (dwIndex != 0 && (dwIndex % 0x15 == 0)) {
                cstrTemp += ("\\\n");
            }
        }
        cstrTemp += ("\n");
        cstrOutput += cstrTemp;
        break;
    }
      case REG_NONE:{
          sprintf(buffer, "hex(%d):", dwKeyType);
          cstrOutput = buffer;
        }
    case REG_DWORD_BIG_ENDIAN:
        //todo do for this type
    case REG_LINK:
        // todo do for this type
        break;

    }

}

void WorkerThread::exportRegistryAndPrintRun(HKEY key, LPCSTR keyName, string parentHKEYName,  fstream &myFile){
    string firstPartfOfKey = "["+parentHKEYName;
    string secondPartOfKey = keyName;
    string lastPartOfKey = "]\n";
    string fullKey = firstPartfOfKey + secondPartOfKey + lastPartOfKey;
    myFile << fullKey;
    checkSum +=fullKey.length();
    DWORD cSubKeys = 0;

    DWORD i;
    DWORD cValues = 1;
    DWORD lLongestKeyNameLen = 1;
    DWORD lLongestDataLen = 1;
   // FILETIME ftLastWriteTime;
    long resCode = RegQueryInfoKey(key, NULL,NULL, NULL, &cSubKeys,NULL, NULL, &cValues, &lLongestKeyNameLen,&lLongestDataLen, NULL,NULL);
  if(resCode==ERROR_SUCCESS){
    if (cValues)
    {
        EnumerateValuesRun(key, cValues, lLongestDataLen, myFile);
    }
    else{
        qDebug()<<"=======No values=======";
    }

    if (cSubKeys)
    {

        for (i = 0; i < cSubKeys; i++){
            DWORD buffForName;
            buffForName = MAX_KEY_LENGTH;
            char arrayCharsForName[MAX_KEY_LENGTH];

            resCode = RegEnumKeyExA(
                key, // Handle to an open/predefined key
                i, // Index of the subkey to retrieve.
                arrayCharsForName, // buffer that receives the name of the subkey
                &buffForName, // size of the buffer specified by the arrayCharsForName
                NULL, // Reserved; must be NULL
                NULL,
                NULL,
                NULL
            );
            if (resCode == ERROR_SUCCESS){
                HKEY nextKey;
                LPCSTR stringToOpen = arrayCharsForName;
                RegOpenKeyExA(key, stringToOpen, 0, KEY_ALL_ACCESS, &nextKey);
                string convertStringToOpen = stringToOpen;
                string converStringName = keyName;
                string concatStr = converStringName + "\\"+ convertStringToOpen;
                LPCSTR resultString = concatStr.c_str();
                exportRegistryAndPrintRun(nextKey, resultString, parentHKEYName, myFile);
            }
            else {
               qDebug()<<"========== Can not read key=========";
            }
        }
        myFile << "\n";
        checkSum +=1;
    }
  }
}
