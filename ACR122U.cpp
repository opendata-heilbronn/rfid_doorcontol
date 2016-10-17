#include <stdio.h>
#include <iostream>
#include <string.h>
#include "ACR122U.hpp"
#ifdef RFID_KEYS_HPP
	#include "rfid_keys.hpp"
#else
	std::vector<BYTE> KEY_VECTOR = {};
#endif

//Assistance functions

//Checks the result of a call to a pcsc_lite function
int ACR122U::Check(std::string f, long rv){
    if (SCARD_S_SUCCESS != rv) 
    { 
        printf("%s : %s\n", f.c_str(), pcsc_stringify_error(rv)); 
        return -1;
    }
    return 0;     
}

std::vector<BYTE> ACR122U::shorten(std::vector<BYTE> data){

    if(data == accessUnsuccessful){
        return accessSuccessful;
    }else if(data.size() >= 2 && data.at(data.size()-2) == accessSuccessful.at(0) && data.back() == accessSuccessful.at(1)){
        data.pop_back();
        data.pop_back();
    }

    return data;
}

std::string ACR122U::toString(std::vector<BYTE> data){
    std::string result;

    for(size_t i = 0; i < data.size(); i++)
        result.push_back((char)data.at(i));

    return result;
}

std::vector<BYTE> ACR122U::toBYTE(std::string data){
    std::vector<BYTE> result;

    for(size_t i = 0; i < data.size(); i++)
        result.push_back((BYTE)data.at(i));

    return result;
}

//Returns if the initialization was successful
bool ACR122U::getInitSuccessful(){
    return SUCCESSFUL;
}

//Wait for the insertion of a smart card
void ACR122U::SWait(){

    SCARD_READERSTATE rgReaderState[1];

    do{
	   
	    rgReaderState[0].cbAtr = 0;

	    rgReaderState[0].szReader = mszReaders;

	    rgReaderState[0].dwCurrentState = SCARD_STATE_UNAWARE;

	    rv = SCardGetStatusChange(hContext, -1, &rgReaderState[0], 1);


	    rgReaderState[0].dwCurrentState = rgReaderState[0].dwEventState;


	    rv = SCardGetStatusChange(hContext, -1, &rgReaderState[0], 1);

    }while(!(rgReaderState[0].dwCurrentState & SCARD_STATE_EMPTY));

    
    if(rv != SCARD_S_SUCCESS){
        std::cout << "Error! Something went wrong with the connection to the Reader!" << std::endl;
        exit(-3);
    }
}


//Constructors
//Standardconstructor
ACR122U::ACR122U(){
 dwRecvLength = sizeof(pbRecvBuffer);

 rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
 Check("SCardEstablishContext", rv);

 if(rv != SCARD_S_SUCCESS){
     std::cout << "Driver error. Couldn't connect to the PCSC Daemon!" << std::endl;
     exit(-1);
 }

#ifdef SCARD_AUTOALLOCATE
 dwReaders = SCARD_AUTOALLOCATE;

 rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
#else
 rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);

 mszReaders = calloc(dwReaders, sizeof(char));
 rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
#endif
 if(Check("SCardListReaders", rv) == -1){
     SUCCESSFUL = false;
     std::cout << "Could not find any readers to connect to" << std::endl;
     exit(-1);
 }else{
    printf("reader name: %s\n", mszReaders);

    rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
    
    if(!(rv == SCARD_S_SUCCESS || rv == SCARD_E_NO_SMARTCARD)){
        std::cout << "Couldn't connect to the specified Reader! Error Code: " << std::hex << rv << std::endl;
        exit(-1);
    }

    SUCCESSFUL = true;

    pioSendPci.dwProtocol = dwActiveProtocol;
    pioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

 }
}

//Deconstructors
//Standarddeconstructor
ACR122U::~ACR122U(){
if(SUCCESSFUL == true){
    rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    Check("SCardDisconnect", rv);

    #ifdef SCARD_AUTOALLOCATE
    rv = SCardFreeMemory(hContext, mszReaders);
    Check("SCardFreeMemory", rv);

    #else
    free(mszReaders);
    #endif

    rv = SCardReleaseContext(hContext);

    Check("SCardReleaseContext", rv);
 }
}

//Standard Function Definitions
//connect to the Smart Card in the Reader
bool ACR122U::SConnect(){
    rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);

    if(rv != SCARD_S_SUCCESS){
        cardConnected = false;
        return false;
    }
    cardConnected = true;
    return true;
}

void ACR122U::SDisconnect(){
    rv = SCardDisconnect(hCard, SCARD_EJECT_CARD);
    cardConnected = false;
}

//Sends the specified ADPU to the Smartcard
std::vector<BYTE> ACR122U::sendCardCommand(std::vector<BYTE> Command){

    BYTE TEMP[Command.size()];

    for(std::size_t i = 0; i < Command.size(); i++){
        TEMP[i] = Command.at(i);
    }

    rv = SCardTransmit(hCard, SCARD_PCI_T1, TEMP, sizeof(TEMP), NULL, pbRecvBuffer, &dwRecvLength);

    std::vector<BYTE> result;

    //Send an "operation unsuccessful" if something failed while sending the command
    if(Check("SCardTransmit", rv) == -1){
        result.push_back(accessUnsuccessful[0]);
        result.push_back(accessSuccessful[1]);
        
        return result;
    }

    for(std::size_t i = 0; i < dwRecvLength; i++){
        result.push_back(pbRecvBuffer[i]);
    }

    dwRecvLength = sizeof(pbRecvBuffer);

    return shorten(result);
}


//Sends the specified ADPU to the Smart Card Reader
std::vector<BYTE> ACR122U::sendReaderCommand(std::vector<BYTE> Command){

    BYTE TEMP[Command.size()];

    for(std::size_t i = 0; i < Command.size(); i++){
        TEMP[i] = Command.at(i);
    }

    DWORD responseLength;

    rv = SCardControl(hCard, (0x42000000 + 3500), TEMP, sizeof(TEMP), &pbRecvBuffer, dwRecvLength, &responseLength); 

    std::vector<BYTE> result;

    //Send an "operation unsuccessful" if something failed while sending the command
    if(Check("SControlTransmit", rv) == -1){
        result = accessUnsuccessful;

        return result;
    }

    for(size_t i = 0; i < responseLength; i++){
        result.push_back(pbRecvBuffer[i]);
    }
    
    return result;
}


//Top-level functions

std::vector<BYTE> ACR122U::getATR(){
    std::vector<BYTE> result;

    unsigned char *pbAttr;
    DWORD dwAttrLen = SCARD_AUTOALLOCATE;
    rv = SCardGetAttrib(hCard, SCARD_ATTR_ATR_STRING, (unsigned char *)&pbAttr, &dwAttrLen);

    for(size_t i = 0; i < dwAttrLen; i++){
        result.push_back(pbAttr[i]);
    }

    return result;
}

std::vector<BYTE> ACR122U::getType(){
    std::vector<BYTE> result;

    result = getATR();
    result.assign(result.begin()+13, result.begin()+15);

    return result;
}


std::vector<BYTE> ACR122U::readCardID(){
    std::vector<BYTE> result;

    result = readBlock(0);
    result.assign(result.begin(), result.begin()+4);

    return result;
}


std::string ACR122U::readCardIDString(){
    std::vector<BYTE> result;

    result = readBlock(0);
    result.assign(result.begin(), result.begin()+4);

    return toString(result);
}

std::vector<BYTE> ACR122U::loadSKey(int sector){
    std::vector<BYTE> result;

    if((sector*6+6) <= KEY_VECTOR.size()){
        result.assign(KEY_VECTOR.begin()+sector*6, KEY_VECTOR.begin()+6+sector*6);
    }else{
        result.assign({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    }

    for(size_t i = 0; i < 6; i++){
        loadKey.at(5+i) = result.at(i);
    }

    result = sendCardCommand(loadKey);

    return result;
}

std::vector<BYTE> ACR122U::SAuthenticate(int block){
    std::vector<BYTE> result;

    authenticate.at(3) = block;

    result = sendCardCommand(authenticate);

    return result;
}

#ifdef RFID_KEYS_HPP
std::vector<BYTE> ACR122U::readBlockCrypt(int blockNumber){
    std::vector<BYTE> result;  

    loadSKey(blockNumber/4);
    result = SAuthenticate(blockNumber);

    for(size_t i = 0; i < result.size(); i++){
      printf("%02x", result.at(i));
    }

    if(result == accessUnsuccessful){
        return result;
    }

    read.at(3) = blockNumber;

    result = sendCardCommand(read);

    return shorten(result);
}  
#endif

std::vector<BYTE> ACR122U::readBlock(int blockNumber){
    std::vector<BYTE> result;  

    loadSKey(10000);
    result = SAuthenticate(blockNumber);

    if(result == accessUnsuccessful){
        return result;
    }

    read.at(3) = blockNumber;

    result = sendCardCommand(read);

    return shorten(result);
}  

std::vector<BYTE> ACR122U::writeKey(int sector, std::vector<BYTE> KEY){
    std::vector<BYTE> result;  

    loadSKey(0);
    result = SAuthenticate(sector*4+3);

    if(result == accessUnsuccessful){
        return result;
    }

    write.at(3) = sector*4+3;

    for(size_t i = 0; i < 16; i++){
        if(i < 6)
            write.at(5+i) = KEY.at(i);
        else
            write.at(5+i) = origTrail.at(i);    
    }

    std::cout << write.size() << std::endl;

    for(size_t i = 0; i < write.size(); i++){
        printf("%02x", write.at(i));
    }

    result = sendCardCommand(write);

    return shorten(result);
}

std::vector<BYTE> ACR122U::writeKey(int sector){
    std::vector<BYTE> result;  

    loadSKey(0);
    result = SAuthenticate(sector*4+3);

    if(result == accessUnsuccessful){
        return result;
    }

    write.at(3) = sector*4+3;

    for(size_t i = 0; i < 16; i++){
        if(i < 6)
            write.at(5+i) = KEY_VECTOR.at(sector*6+i);
        else
            write.at(5+i) = origTrail.at(i);    
    }

    std::cout << write.size() << std::endl;

    for(size_t i = 0; i < write.size(); i++){
        printf("%02x", write.at(i));
    }

    result = sendCardCommand(write);

    return shorten(result);
}

std::vector<BYTE> ACR122U::readBlock(int sector, int blockNumber){
    int block = sector * 4 + blockNumber;
    std::vector<BYTE> result = readBlock(block);

    return result;
}    

std::string ACR122U::readBlockString(int blockNumber){
    std::vector<BYTE> result = readBlock(blockNumber);

    return toString(result);
}

std::string ACR122U::readBlockString(int sector, int blockNumber){
    int block = sector * 4 + blockNumber;
    std::vector<BYTE> result = readBlock(block);

    return toString(result);
}

std::vector<BYTE> ACR122U::writeBlock(int blockNumber, std::vector<BYTE> data){
    std::vector<BYTE> result;
    //Checks if someone is trying to write to the trailer block of the sector
    if((blockNumber+1)%4 == 0){
        return accessUnsuccessful;
    }

    authenticate[3] = blockNumber;
    write[3] = blockNumber;

    dwRecvLength = sizeof(pbRecvBuffer);

    BYTE TEMP[authenticate.size()];

    for(std::size_t i = 0; i < authenticate.size(); i++){
        TEMP[i] = authenticate.at(i);
    }

    rv = SCardTransmit(hCard, &pioSendPci, TEMP, sizeof(TEMP), NULL, pbRecvBuffer, &dwRecvLength);
    if(Check("SCardTransmit", rv) == -1){
        return accessUnsuccessful;
    }


    unsigned int data_size = data.size();
    if(data_size < 16){
        for(std::size_t i = 0; i < (16-data_size); i++){
            data.push_back(0x00);
        }
    }else if(data_size > 16){
        for(std::size_t i = 0; i < (data_size-16); i++){
            data.pop_back();
        }    
    }

    for(std::size_t i = 0; i < 16; i++){
        write[5+i] = data.at(i);
    }

    dwRecvLength = sizeof(pbRecvBuffer);

    BYTE TEMP2[write.size()];

    for(std::size_t i = 0; i < write.size(); i++){
        TEMP2[i] = write.at(i);
    }

    rv = SCardTransmit(hCard, &pioSendPci, TEMP2, sizeof(TEMP2), NULL, pbRecvBuffer, &dwRecvLength);
    if(Check("SCardTransmit", rv) == -1){
        return accessUnsuccessful;
    }

    for(std::size_t i = 0; i < dwRecvLength-2; i++){
        result.push_back(pbRecvBuffer[i]);
    }

    dwRecvLength = sizeof(pbRecvBuffer);

    return result;
}

std::vector<BYTE> ACR122U::writeBlock(int sector, int blockNumber, std::vector<BYTE> data){
    int block = sector * 4 + blockNumber;

    std::vector<BYTE> result = writeBlock(block, data);

    return result;
}

std::vector<BYTE> ACR122U::writeBlockString(int blockNumber, std::string data){
    std::vector<BYTE> result;

    result = writeBlock(blockNumber, toBYTE(data));    

    return result;
}

std::vector<BYTE> ACR122U::writeBlockString(int sector, int blockNumber, std::string data){
    std::vector<BYTE> result;

    result = writeBlock((sector*4 + blockNumber), toBYTE(data));    

    return result;
}


std::vector<BYTE> ACR122U::getFirmware(){
    std::vector<BYTE> result = sendReaderCommand(readerFirmware);

    result.erase(result.begin(), result.begin() +4);

    return result;
}

std::string ACR122U::getFirmwareString(){
    std::vector<BYTE> result = sendReaderCommand(readerFirmware);

    //result.erase(result.begin(), result.begin() +4 );

    return toString(result);
}

std::vector<BYTE> ACR122U::setLEDs(bool green, bool blue, bool orange, bool red){ 
    readerSetLED[3] = (int)green + 2* (int)blue + 4*(int)orange + 8 * (int)red;

    std::vector<BYTE> result = sendReaderCommand(readerSetLED);

    return result;
}
