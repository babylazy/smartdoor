#ifdef __cplusplus
extern "C" 
{
#endif
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
#ifdef __cplusplus
}
#endif
#if ARDUINO >= 100
#include "Arduino.h"
#else   
#include "WProgram.h"
#endif


#include "mi_rfid.h"
unsigned char flag_read=0,pv_flag_read=1;
byte HEADER,LEN,CMD,STATUS,UID[4],TYPE,CHKSUM,BOX;
byte DATA[16];


RFID::RFID(int rx,int tx):SoftwareSerial(rx,tx)
{

}

void RFID::begin(long baudrate)
{
	SoftwareSerial::begin(baudrate);
}

bool RFID::WaitStatus(int found_pin){
   pinMode(found_pin,INPUT);  
   int input = digitalRead(found_pin);
   
   if(input==0)
   {
     if(pv_flag_read)
		flag_read=1;
   }
   if(input==1)
   {
     pv_flag_read=1;
   }
   
   if (flag_read == 1){
		pv_flag_read=0;
		flag_read=0; 
		return(1);
   }else	
	  return(0);
}

byte RFID::checksum(byte (*values),int length){
  byte result = values[0];
  for (int i = 1; i < length; i++)
    result ^= values[i];
  return result;
}
byte RFID::readTaginfo(byte *UID,byte *type){
    SoftwareSerial::write(0xBA);
    SoftwareSerial::write(0x02);
    SoftwareSerial::write(0x01);   
    SoftwareSerial::write(0xB9);
    while(1){
      if (SoftwareSerial::read() == 0xBD) break;
    }
    
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();
    
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();
    
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
       
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 4 ; i++) {      
          while (SoftwareSerial::available() == 0) ;
          UID[i] = SoftwareSerial::read();
        }  
      
        while (SoftwareSerial::available() == 0) ;    
          type[0] = SoftwareSerial::read();
       
        while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();	      
      }
	  return(STATUS);
}

byte RFID::LoginSector(byte sector,byte *key,byte type){
  byte *frame;

  frame = (byte *) malloc(12 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 10;   // Length 10
  frame[2] = LOGIN;
  frame[3] = sector;
  frame[4] = type;
  frame[5] = key[0];
  frame[6] = key[1];
  frame[7] = key[2];
  frame[8] = key[3];
  frame[9] = key[4];
  frame[10] = key[5];
  frame[11] = checksum(frame, 11);

  // Send frame to reader  
  SoftwareSerial::write(frame, 12);
  free(frame);
  frame = NULL;
        while (SoftwareSerial::available()==0);    
           HEADER = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);    
           LEN = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);   
           CMD = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0) ;   
           STATUS = SoftwareSerial::read();

        while (SoftwareSerial::available()==0) ;     
           CHKSUM = SoftwareSerial::read();
          
        if (STATUS == LOGIN_SUCCEED) { // Login Success
          //Serial.println("Login Success***");
          return(LOGIN_SUCCEED);
        }  
        if (STATUS == LOGIN_FAIL) { // Login Success
         // Serial.println("Login Fail---");
          return(LOGIN_FAIL);
        }    
}

byte RFID::readBlockToData(int block,byte *DATA){
  byte *frame;

  frame = (byte *) malloc(5 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 3;
  frame[2] = READ_BLOCK;
  frame[3] = block;
  frame[4] = checksum(frame, 4);

  // Send frame to reader  
  SoftwareSerial::write(frame, 5);
  free(frame);
  frame = NULL;
  
      while (1){    
		   while (SoftwareSerial::available()==0); 
			if(SoftwareSerial::read() == 0xBD) break;
      }     
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();
    
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();
    
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
                
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 16 ; i++) {  
          if(SoftwareSerial::available() > 0){    
            while (SoftwareSerial::available() == 0) ;
              DATA[i] = SoftwareSerial::read();
          }  
        }  
      }                     
      while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();
       
      if (STATUS == 0x00) // operation ok
		return (0x00);
	  else
		return (0x01);
      //else  Serial.println("Read UNcomplete");   
      
}

byte RFID::writeBlockFromData(int block,byte *DATAIN){
  byte *frame;
  
  frame = (byte *) malloc(21 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 19;
  frame[2] = 0x04;  // write block
  frame[3] = block;
  frame[4] = DATAIN[0];
  frame[5] = DATAIN[1];
  frame[6] = DATAIN[2];
  frame[7] = DATAIN[3];
  frame[8] = DATAIN[4];
  frame[9] = DATAIN[5];
  frame[10] = DATAIN[6];
  frame[11] = DATAIN[7];
  frame[12] = DATAIN[8];
  frame[13] = DATAIN[9];
  frame[14] = DATAIN[10];
  frame[15] = DATAIN[11];
  frame[16] = DATAIN[12];
  frame[17] = DATAIN[13];
  frame[18] = DATAIN[14];
  frame[19] = DATAIN[15];  
  frame[20] = checksum(frame, 20); 

  // Send frame to reader  
  SoftwareSerial::write(frame, 21);
  free(frame);
  frame = NULL;
 // while(Serial.available() == 0); // wait data from SL025M
  
      while (1){    
         while (SoftwareSerial::available()==0); 
         if(SoftwareSerial::read() == 0xBD) break;
      }      
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
       
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 16 ; i++) {      
          while (SoftwareSerial::available() == 0) ;
          DATAIN[i] = SoftwareSerial::read();
        }  
      }  
                   
      while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();    
           
      if (STATUS == 0x00) return (0x00);
}
byte RFID::writeBlockFromValue(byte block,byte *DATAIN){
  byte *frame;
  
  frame = (byte *) malloc(9 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 7;
  frame[2] = 0x06;  // write block
  frame[3] = block;
  frame[4] = DATAIN[0];
  frame[5] = DATAIN[1];
  frame[6] = DATAIN[2];
  frame[7] = DATAIN[3];
  frame[8] = checksum(frame, 8); 

  // Send frame to reader  
  SoftwareSerial::write(frame, 9);
  free(frame);
  frame = NULL;
 // while(Serial.available() == 0); // wait data from SL025M
  
      while (1){    
         while (SoftwareSerial::available()==0); 
         if(SoftwareSerial::read() == 0xBD) break;
      }      
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
       
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 4 ; i++) {      
          while (SoftwareSerial::available() == 0) ;
          DATAIN[i] = SoftwareSerial::read();
        }  
      }  
                   
      while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();    
           
      return(STATUS);
}
byte RFID::IncreasedBlockFromValue(byte block,byte *DATAIN,byte *STORE){
  byte *frame;
  
  frame = (byte *) malloc(9 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 7;
  frame[2] = 0x08;  // write block
  frame[3] = block;
  frame[4] = DATAIN[0];
  frame[5] = DATAIN[1];
  frame[6] = DATAIN[2];
  frame[7] = DATAIN[3];
  frame[8] = checksum(frame, 8); 

  // Send frame to reader  
  SoftwareSerial::write(frame, 9);
  free(frame);
  frame = NULL;
 // while(Serial.available() == 0); // wait data from SL025M
  
      while (1){    
         while (SoftwareSerial::available()==0); 
         if(SoftwareSerial::read() == 0xBD) break;
      }      
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
       
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 4 ; i++) {      
          while (SoftwareSerial::available() == 0) ;
          STORE[i] = SoftwareSerial::read();
        }  
      }  
                   
      while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();    
           
      if (STATUS == 0x00) return (0x00);
}
byte RFID::DecreasedBlockFromValue(byte block,byte *DATAIN,byte *STORE){

  byte *frame;
  frame = (byte *) malloc(9 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 7;
  frame[2] = 0x09;  // decresed value
  frame[3] = block;
  frame[4] = DATAIN[0];
  frame[5] = DATAIN[1];
  frame[6] = DATAIN[2];
  frame[7] = DATAIN[3];
  frame[8] = checksum(frame, 8); 

  // Send frame to reader  
  SoftwareSerial::write(frame, 9);
  free(frame);
  frame = NULL;
 // while(Serial.available() == 0); // wait data from SL025M
  
      while (1){    
         while (SoftwareSerial::available()==0); 
         if(SoftwareSerial::read() == 0xBD) break;
      }      
      while (SoftwareSerial::available()==0);    
         LEN = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0);   
         CMD = SoftwareSerial::read();  
         
      while (SoftwareSerial::available()==0) ;   
         STATUS = SoftwareSerial::read();
       
      if (STATUS == 0x00) { // Select Success
        for (int i = 0; i < 4 ; i++) {      
          while (SoftwareSerial::available() == 0) ;
          STORE[i] = SoftwareSerial::read();
        }  
      }  
                   
      while (SoftwareSerial::available() == 0) ;    
          CHKSUM = SoftwareSerial::read();    
           
      if (STATUS == 0x00) return (0x00);
}

byte RFID::SetMasterKeyA(byte sector,byte *skey){
  byte *frame;
  frame = (byte *) malloc(11 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 9;   // Length 10
  frame[2] = 0x07;
  frame[3] = sector;
  frame[4] = skey[0];
  frame[5] = skey[1];
  frame[6] = skey[2];
  frame[7] = skey[3];
  frame[8] = skey[4];
  frame[9] = skey[5];
  frame[10] = checksum(frame, 10);

  // Send frame to reader  
  SoftwareSerial::write(frame, 11);
  free(frame);
  frame = NULL;
        while (SoftwareSerial::available()==0);    
           HEADER = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);    
           LEN = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);   
           CMD = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0) ;   
           STATUS = SoftwareSerial::read(); 


          
		 if (STATUS == 0x00) { // Login Success
          //Serial.println("Setkey Success***");
         // return(SET_KEY_COMPLETE);
		  for (int i = 0; i < 6 ; i++) {      
			while (SoftwareSerial::available() == 0) ;
			skey[i] = SoftwareSerial::read();
          }  
         }   
		 
         while (SoftwareSerial::available()==0) ;     
           CHKSUM = SoftwareSerial::read();
		   
		return(STATUS);  
}
byte RFID::LED(byte led){
  byte *frame;
  frame = (byte *) malloc(5 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 3;   // Length 10
  frame[2] = 0x40;
  frame[3] = led;
  frame[4] = checksum(frame, 4);

  // Send frame to reader  
  SoftwareSerial::write(frame, 5);
  free(frame);
  frame = NULL;
        while (SoftwareSerial::available()==0);    
           HEADER = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);    
           LEN = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0);   
           CMD = SoftwareSerial::read();
    
        while (SoftwareSerial::available()==0) ;   
           STATUS = SoftwareSerial::read(); 
 
         while (SoftwareSerial::available()==0) ;     
           CHKSUM = SoftwareSerial::read();
		   
		return(STATUS);  
}









