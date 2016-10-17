#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sqlite3.h>
#include <thread>
#include <chrono>
#include <wiringPi.h>
#include "ACR122U.hpp"

void LSleep();

std::vector<BYTE> result;

std::string selectedHASH;
std::string SCardString;

static int callback(void *data, int argc, char **argv, char **azColName){
  selectedHASH = argv[0];
  return 0;
}

int main(void)
{

  if(wiringPiSetup() == -1){
	std::cout << "Couldn't initialize wiringPi" << std::endl;
	return -4;
  }

  pinMode(15, OUTPUT);

  //Assistvariable Declaration
  int rv;
  bool run = true;
  char* data;

  //Connecto to the PCSC-Manager
  ACR122U ACR;

  //Open the Database connection
  sqlite3 *handle;
  rv = sqlite3_open("UID.sqlite", &handle);

  //Did something go wrong?
  if(rv != 0){
    std::cout << "Error opening the database!\nError Msg: " << sqlite3_errmsg16(handle) << std::endl;
    return (-2);  
  }

  //Main Loop
  do{
    selectedHASH = "";
    SCardString = "";
    //Wait for the insertion of a Smartcard
    ACR.SWait();

    //So, a Smartcard was found?
    rv = ACR.SConnect();
    if(rv == false){
      std::cout << "Couldn't connect to the Smartcard" << std::endl;
      ACR.SDisconnect();
      LSleep();
      continue;
    }

    //Get the type of the Smartcard
    result = ACR.getType();
    if(result[0] != 0x00 || result[1] != 0x01){
      std::cout << "A wrong Smartcard was connected" << std::endl;
      //TODO: Add Smartcard Disconnect
      ACR.SDisconnect();
      LSleep();
      continue;
    }

    //Get the UID of the Smartcard
    result = ACR.readCardID();
    if(result.size() != 4){
      std::cout << "A wrong Smartcard was connected(Wrong ID size)" << std::endl;
      //TODO:Add Smartcard Disconnect
      ACR.SDisconnect();
      LSleep();
      continue;
    }

    std::string ID = ACR.toString(result);

    //Extract the Hash

    for(size_t i = 0; i < 3; i++){
      result = ACR.readBlockCrypt(4+i);
      SCardString.append(ACR.toString(result));
    }
    result = ACR.readBlockCrypt(8);
    SCardString.append(ACR.toString(result).substr(0, 12));

    std::string sqlStatement = "SELECT HASH from UID where UID = \'" + ID + "\'";

    rv =  sqlite3_exec(handle, sqlStatement.c_str(), callback, (void*)data, NULL);


    std::cout << selectedHASH << "\t" << SCardString << std::endl;

    //Hash correct?
    if(!(SCardString == selectedHASH) || selectedHASH.size() == 0){
      std::cout << "A wrong Smartcard was connected(Wrong Hash)" << std::endl;
      std::cout << SCardString << "\t" << selectedHASH << std::endl; 
    }else{
      std::cout << "Worked!" << std::endl;
      digitalWrite(15, 1);
      std::this_thread::sleep_for (std::chrono::milliseconds(1200));
      digitalWrite(15, 0);
    }

    ACR.SDisconnect();

    LSleep();
  }while(run);


  return 0;
}

void LSleep(){
  std::this_thread::sleep_for (std::chrono::milliseconds(200));
}
