#ifndef REGISTRYSHELL_H
#define REGISTRYSHELL_H
#include <string.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <regex>
#include <QtDebug>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
using namespace std;

void FormatDataWithDataType(DWORD dwKeyType, LPBYTE pbbinKeyData, DWORD dwKeyDataLength, string& cstrOutput);
void EnumerateValues(HKEY hKey, DWORD cValues, DWORD lLongestKeyNameLen, DWORD lLongestDataLen, fstream &myFile);
void exportRegistryAndPrint(HKEY key, LPCSTR keyName, string parentHKEYName, fstream &myFile);
void startPointToExportRegistry(LPCSTR parentHKEYName,  LPCSTR subKeyName, fstream &myFile);
int getSumFile(string nameFile);

static long checkSum=0;

int getSumFile(string nameFile){
  long sumInFile=0;
  string item_name;
  ifstream nameFileout;
  nameFileout.open(nameFile);
  string line;
  while(std::getline(nameFileout, line))
  {
      if(regex_match(line, regex("\".*(\"|\\d|\\w)"))){
          line +="\n";
      sumInFile += line.length();
        }
      //std::cout << "line:" << line << std::endl;
      // TODO: assign item_name based on line (or if the entire line is
      // the item name, replace line with item_name in the code above)
  }
  cout<<sumInFile;
  sumInFile -=checkSum;
  qDebug() <<checkSum;
  checkSum =0;
  return sumInFile;
}
void exportAllRegistry(int a, fstream &myFile){

  if(a==0){
      cout<<"start export";

  startPointToExportRegistry("HKEY_CLASSES_ROOT", "",myFile);
    }
  if(a==1)
  startPointToExportRegistry("HKEY_CURRENT_USER", "",myFile);
  if(a==2)
  startPointToExportRegistry("HKEY_LOCAL_MACHINE", "",myFile);
  if(a==3)
  startPointToExportRegistry("HKEY_USERS", "",myFile);
  if(a==4)
  startPointToExportRegistry("HKEY_CURRENT_CONFIG", "",myFile);

  if(a==4){
  myFile.close();
cout<<"end export";
    }
}
void startPointToExportRegistry(LPCSTR parentHKEYNameLPCSTR,  LPCSTR subKeyName,  fstream &myFile){
  cout<<"start";
  long file_size;
  myFile.seekg(0, ios::end);
  file_size = myFile.tellg();
  if (file_size == 0)
    myFile << "Windows Registry Editor Version 5.00 \n";

    // cout<<"А файл то пуст :D"<<endl;f.close();
  //AlbumArt values
      //AppEvents package
      //\AppEvents\EventLabels\.Default  ; defualt values exist
      // SOFTWARE\\Radmin\\v3.0\\Parameters ; hex values
  //S-1-5-21-2657195928-4124521378-2452857898-1001_Classes\\.cap\\OpenWithProgids // Data Type REG_NONE
  //S-1-5-21-2657195928-4124521378-2452857898-1001_Classes\\.class_auto_file\\shell\\edit\\command // DataType REG_EXPAND_SZ

  HKEY phkey;
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
  if (hkey == ERROR_SUCCESS)
    exportRegistryAndPrint(phkey, subKey, parentHKEYName,myFile);
  cout<< "end";;
}

void EnumerateValues(HKEY hKey, DWORD cValues, DWORD lLongestDataLen,  fstream &myFile) {

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
                FormatDataWithDataType(dataType, lpData, dataSize, cstrFinalData);

                //// For (default) key names the tcKeyName is empty and dwKeyNameLen is zero ...in such case we need to
                //// have assignment like @ = "value"
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
                //cout << cstrTemp;

             }


        }

}
void FormatDataWithDataType(DWORD dwKeyType, LPBYTE pbbinKeyData, DWORD dwKeyDataLength, string& cstrOutput)
{
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

void exportRegistryAndPrint(HKEY key, LPCSTR keyName, string parentHKEYName,  fstream &myFile)
{
  string newString = "\n";
    string firstPartfOfKey = "["+parentHKEYName;
    string secondPartOfKey = keyName;
    string lastPartOfKey = "]\n";
    string fullKey = firstPartfOfKey + secondPartOfKey + lastPartOfKey;

    myFile << fullKey;
   // cout << fullKey;

    DWORD cSubKeys = 0;
    char achKey[MAX_VALUE_NAME];
    DWORD cbName;
    DWORD i;
    DWORD cValues = 1;
    DWORD lLongestKeyNameLen = 1;
    DWORD lLongestDataLen = 1;
    FILETIME ftLastWriteTime;

    long retCode = RegQueryInfoKey(

        key, // key handle

        NULL, // buffer for class name

        NULL, // size of class string

        NULL, // reserved

        &cSubKeys, // number of subkeys

        NULL, // longest subkey size

        NULL, // longest class string

        &cValues, // number of values for this key

        & lLongestKeyNameLen, // longest value name

        &lLongestDataLen, // longest value data

        NULL, // security descriptor

        & ftLastWriteTime); // last write time


    if (cValues)
    {
        EnumerateValues(key, cValues, lLongestDataLen, myFile);

        myFile<<newString;
    }
    else{
        //cout<<"=======No values=======";
    }

    if (cSubKeys)

    {

        for (i = 0; i < cSubKeys; i++){
            cbName = MAX_VALUE_NAME;

            retCode = RegEnumKeyExA(

                key, // Handle to an open/predefined key

                i, // Index of the subkey to retrieve.

                achKey, // buffer that receives the name of the subkey

                &cbName, // size of the buffer specified by the achKey

                NULL, // Reserved; must be NULL

                NULL, // buffer that receives the class string

                // of the enumerated subkey

                NULL, // size of the buffer specified by the previous parameter

                &ftLastWriteTime // variable that receives the time at which

                // the enumerated subkey was last written

            );

            if (retCode == ERROR_SUCCESS){
                HKEY nextKey;
                LPCSTR stringToOpen = achKey;
                RegOpenKeyExA(key, stringToOpen, 0, KEY_ALL_ACCESS, &nextKey);
                string convertStringToOpen = stringToOpen;
                string converStringName = keyName;
                string concatStr = converStringName + "\\"+ convertStringToOpen;
                LPCSTR resultString = concatStr.c_str();
                exportRegistryAndPrint(nextKey, resultString, parentHKEYName, myFile);
            }
            else {
               cout<<"========== Can not read key=========";
            }
        }
        myFile << newString;
    }

}

#endif // REGISTRYSHELL_H
