#ifndef mi_RFID_h
#define mi_RFID_h

#include <Arduino.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include <String.h>
// Reader commands
#define GET_TAG 0x01
#define LOGIN 0x02
#define READ_BLOCK 0x03
#define RESET 0xFF
#define SEND_HEADER 0xBA

// Read status
#define OPERATION_OK 0x00
#define NO_TAG 0x01
#define READ_FAIL 0x04
#define WRITE_FAIL 0x05

// Login key types
#define USE_KEY_A 0xAA
#define USE_KEY_B 0xBB

// Login results
#define LOGIN_SUCCEED 0x02
#define LOGIN_FAIL 0x03

class RFID : public SoftwareSerial 
{
	public:
		RFID(int rx,int tx);
		bool  WaitStatus(int found_pin);
		void begin(long baudrate);
		byte readTaginfo(byte *UID,byte *type);
		byte LoginSector(byte sector,byte *key,byte type);
		byte readBlockToData(int block,byte *DATA);
		byte writeBlockFromData(int block,byte *DATAIN);
		byte writeBlockFromValue(byte block,byte *DATAIN);
		byte IncreasedBlockFromValue(byte block,byte *DATAIN,byte *STORE);
		byte DecreasedBlockFromValue(byte block,byte *DATAIN,byte *STORE);
		byte SetMasterKeyA(byte sector,byte *skey);
		byte LED(byte led);
    private:
		byte checksum(byte *value,int length);
};

#endif
