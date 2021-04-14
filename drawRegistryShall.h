#ifndef DRAWREGISTRYSHALL_H
#define DRAWREGISTRYSHALL_H
#include <string.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include<QStandardItemModel>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;

void drawRegistry(HKEY key,QStandardItem* parentItem);
void startDraw(LPCSTR parentHKEYNameLPCSTR,  LPCSTR subKeyName,
                QStandardItem *parentItem);
void drawValues(HKEY hKey, DWORD cValues,  QStandardItem* parentItem );

void startDraw(LPCSTR parentHKEYNameLPCSTR,
              QStandardItem *parentItem){

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

    drawRegistry(phkey, parentItem);

}

void drawRegistry(HKEY key, QStandardItem* parentItem)
{
    DWORD cSubKeys = 0;
    char arrayCharsNameKey[MAX_KEY_LENGTH];
    DWORD cbName;
    DWORD i;
    DWORD cValues = 1;
    DWORD lLongestKeyNameLen = 1;
    DWORD lLongestDataLen = 1;
    FILETIME ftLastWriteTime;

    long retCode = RegQueryInfoKey(

        key, // key handle

        NULL,

        NULL,

        NULL,

        &cSubKeys, // number of subkeys

        NULL,

        NULL,

        &cValues, // number of values for this key

        & lLongestKeyNameLen, // longest value name

        &lLongestDataLen, // longest value data

        NULL,

        & ftLastWriteTime); // last write time


    if (cValues)
    {
        drawValues(key, cValues, parentItem);
    }
    else{
        cout<<"=======No values=======";
    }

    if (cSubKeys)

    {

        for (i = 0; i < cSubKeys; i++){
            cbName = MAX_KEY_LENGTH;

            retCode = RegEnumKeyExA(
                key, // Handle to an open/predefined key
                i, // Index of the subkey to retrieve.
                arrayCharsNameKey, // buffer that receives the name of the subkey
                &cbName, // size of the buffer specified by the achKey
                NULL,
                NULL,
                NULL,
                NULL // variable that receives the time at which

                // the enumerated subkey was last written

            );
            if (retCode == ERROR_SUCCESS){
                LPCSTR stringToOpen = arrayCharsNameKey;
                string convertStringToOpen= stringToOpen;
                QStandardItem* item = new QStandardItem(convertStringToOpen.c_str());
                parentItem->appendRow(item);
                HKEY nextKey;
                RegOpenKeyExA(key, stringToOpen, 0, KEY_ALL_ACCESS, &nextKey);
                drawRegistry(nextKey, item);
            }
            else {
               cout<<"========== Can not read key=========";
            }
        }
    }

}
void drawValues(HKEY hKey, DWORD cValues,  QStandardItem* parentItem ) {

    string cstrFinalData, cstrTemp;

    DWORD i, retCode;

    char tcKeyName[MAX_VALUE_NAME];
        for (i = 0; i < cValues; i++)

        {

            DWORD cchValue = MAX_VALUE_NAME;

            retCode = RegEnumValueA(

                hKey, // Handle to an open key

                i, // Index of value

                tcKeyName, // Value name

                &cchValue, // Buffer for value name

                0, // Reserved

                NULL, // Value type

                NULL, // Value data

                NULL); // Buffer for value data

            if (retCode == ERROR_SUCCESS)
            {
                //// For (default) key names the tcKeyName is empty and dwKeyNameLen is zero ...in such case we need to
                //// have assignment like @ = "value"
                string result;
                LPCSTR stringToCout = tcKeyName;
                result = stringToCout;
                QStandardItem* item = new QStandardItem(QIcon(":/file_extension_bat.png"), stringToCout);
                parentItem->appendRow(item);
             }


        }

}
#endif // DRAWREGISTRYSHALL_H
