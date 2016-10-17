#include <winscard.h>
#include <string>
#include <vector>
#include <reader.h>

class ACR122U{
    public:
        //Constructors
        ACR122U();
        //Destructors
        ~ACR122U();

        //Important function for checking if connection process was SUCCESSFUL
        //Check the response for this function ALWAYS before using this instance
        bool getInitSuccessful();

        std::vector<BYTE> sendCardCommand(std::vector<BYTE> Command);
        std::vector<BYTE> sendReaderCommand(std::vector<BYTE> Command);

        //Basic functions
            //Connect to SmartCard
            bool SConnect();
            //Disconnect from SmartCard
            void SDisconnect();
            //waits for an smart card insertion
            void SWait();
            //Get the ATR
            std::vector<BYTE> getATR();
            //Return the Type of the connected Smartcard
            std::vector<BYTE> getType();
            //Load a Key into the reader
            std::vector<BYTE> loadSKey(int sector);
            //Authenticate a Block
            std::vector<BYTE> SAuthenticate(int block);

        //Top-Level functions
        //Read functions
            //get ID of the connected Card
            std::vector<BYTE> readCardID();
            //get ID of the connected Card
            std::string readCardIDString();
            //Version for accessing the block directly
            std::vector<BYTE> readBlock(int blockNumber);
	    #ifdef RFID_KEYS_HPP
            //Version for accessing a block directly with a non-standard key
            std::vector<BYTE> readBlockCrypt(int blockNumber);
	    #endif
            //Version for accessing the block of a sector
            std::vector<BYTE> readBlock(int sector, int blockNumber);
            //Version for returning a String of a block directly
            std::string readBlockString(int blockNumber);
            //Version for returning a String of a block of a sector 
            std::string readBlockString(int sector, int blockNumber);

        //Write functions (currently only implemented for 1k cards and data blocks)
            //version for writing to the block directly
            std::vector<BYTE> writeBlock(int blockNumber, std::vector<BYTE> data);    
            //Version for writing to block of sector
            std::vector<BYTE> writeBlock(int sector, int blockNumber, std::vector<BYTE> data);
            //version for writing a String to the block directly
            std::vector<BYTE> writeBlockString(int blockNumber, std::string block);    
            //Version for writing to block of sector
            std::vector<BYTE> writeBlockString(int sector, int blockNumber, std::string data);
            //Write the key A for the specified Sector
            std::vector<BYTE> writeKey(int sector);
            std::vector<BYTE> writeKey(int sector, std::vector<BYTE> KEY);

        //Readerspecific functions
            //Get the current firmware version as a byte vector
            std::vector<BYTE> getFirmware();
            //Get the current firmware version as a String
            std::string getFirmwareString();
            //set the LEDs
            std::vector<BYTE> setLEDs(bool green, bool blue, bool orange, bool red);

        //Variable declarations
        LONG rv;
        SCARDCONTEXT hContext;
        LPTSTR mszReaders;
        SCARDHANDLE hCard;
        DWORD dwReaders, dwActiveProtocol, dwRecvLength;
        SCARD_IO_REQUEST pioSendPci;
        BYTE pbRecvBuffer[258];

        //IMPORTANT: HOLDS THE VALUE IF THE INITIALIZATION WAS SUCCESSFUL
        bool SUCCESSFUL = true;
        bool cardConnected = false;

        //Assistance functions
            //Checks the response
            int Check(std::string f, long rv);

            //Shortens the response
            std::vector<BYTE> shorten(std::vector<BYTE> data);

            //converts a byte vector to a std::string
            std::string toString(std::vector<BYTE> data);
            //converts a std::string to a byteVector
            std::vector<BYTE> toBYTE(std::string data);


        //Values to compare with
        std::vector<BYTE> accessSuccessful = {0x90, 0x00};
        std::vector<BYTE> accessUnsuccessful = {0x62, 0x90};

        //LIST OF ADPU CODES

            //CODES FOR THE READER
            std::vector<BYTE> readerFirmware = {0xFF, 0x00, 0x48, 0x00, 0x00};
            std::vector<BYTE> readerSetLED = {0xFF, 0x00, 0x44, 0x00, 0x00};
            std::vector<BYTE> cardID = {0xFF, 0xCA, 0x00, 0x00, 0x05};
            //CODES FOR THE SMART CARD
            std::vector<BYTE> loadKey = { 0xFF, 0x82, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            std::vector<BYTE> authenticate = {0xFF, 0x88, 0x00, 0x00, 0x60, 0x00};
            std::vector<BYTE> read = {0xFF, 0xB0, 0x00, 0x00, 0x10};
            std::vector<BYTE> write = {0xFF, 0xD6, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            std::vector<BYTE> origTrail = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x80, 0x69, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
};
