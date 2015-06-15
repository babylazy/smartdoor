#include <SoftwareSerial.h>
#include <mi_rfid.h>

RFID RFID(8,9);  // rx tx
byte keyA[6] ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA};//create key A
byte keyB[6] ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};//create key 

void setup()
{
  RFID.begin(9600);
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Hello RFID READER");
}
void read_Taginfo(void){
  byte Uid[4];
  byte Type[1];
  if (RFID.readTaginfo(Uid,Type)==0x00){
	Serial.println("Operation Success");
        Serial.print ("UID  IS : "); 
        for(int i = 0 ;i<4;i++){
          String c = String(Uid[i],HEX);
          Serial.print(" ");
          Serial.print(c);
        }
        Serial.println("");        
        Serial.print ("TYPE IS :  "); //Serial.println(Type[0],HEX);
        switch(Type[0]){
          case 0x01:
            Serial.println("MIFARE_STANDARD_1K");  
          break;
          case 0x03:
            Serial.println("MIFARE_ULTRA_LIGHT");  
          break;  
          case 0x04:
            Serial.println("MIFARE_STANDARD_4K");  
          break;   
          case 0x06:
            Serial.println("MIFARE_DES_FIRE");  
          break;  
          case 0x0A:
            Serial.println("OTHER TYPE");  
          break;            
        }     
  }     
}


void test_readDATA(byte block){
        byte STORE_DATA[16]; 
        //   Test Read Block Data   
        //                       block is a number of block
        //                       DAT is Array for stored data
        if (RFID.readBlockToData(block,STORE_DATA) == OPERATION_OK){
           Serial.println("Read DATA Complete");     
           Serial.print ("DATA Block "); 
           Serial.print (block,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<16;i++){
              String c = String(STORE_DATA[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
        }
}
void test_writeDATA(byte block){
     byte STORE_DATA[16]; 
     byte DAT_to_Write[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
                             ,0x10,0x11,0x12,0x13,0x14,0x15,0x16};
     // Test Write Block Data ** block is a number of block
       //                          DAT_to_Write is a Data where you want to write
      
        if (RFID.writeBlockFromData(block,DAT_to_Write) == OPERATION_OK){
           Serial.println("Read DATA Complete");     
           Serial.print ("DATA Block "); 
           Serial.print (block,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<16;i++){
              String c = String(DAT_to_Write[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
        }
}

void test_writeValue(byte block){
      byte VALUE[4] = {0x00,0x00,0x00,0x01}; // variable for increse value 
       if (RFID.writeBlockFromValue(block,VALUE) == 0x00){
           Serial.println("Write Value Complete");     
           Serial.print ("Value Block "); 
           Serial.print (block,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<4;i++){
              String c = String(VALUE[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
       }
}
void test_IncreasedValue(byte block){
      byte STORE[4]; // variable for store value
      byte VALUE[4] = {0x00,0x00,0x00,0x01}; // variable for Increased value
      
       if (RFID.IncreasedBlockFromValue(block,VALUE,STORE) == 0x00){
           Serial.println("Increse Value Complete");     
           Serial.print ("Value Block "); 
           Serial.print (block,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<4;i++){
              String c = String(STORE[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
       }
}
void test_DecreasedValue(byte block){
      byte STORE[4]; // variable for store value
      byte VALUE[4] = {0x00,0x00,0x00,0x01}; // variable for dencreased value
      
       if (RFID.DecreasedBlockFromValue(block,VALUE,STORE) == 0x00){
           Serial.println("Decrese Value Complete");     
           Serial.print ("Value Block "); 
           Serial.print (block,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<4;i++){
              String c = String(STORE[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
       }
}
void test_SetKeyA(byte sector){
      byte KeyA[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFA}; // variable for dencreased value
       if (RFID.SetMasterKeyA(sector,KeyA) == 0x00){
           Serial.println("Set Key A Complete");     
           Serial.print ("Key of Sector is  "); 
           Serial.print (sector,DEC); 
           Serial.print (" : "); 
           for(int i = 0 ;i<6;i++){
              String c = String(KeyA[i],HEX);
              Serial.print(" ");
              Serial.print(c);
          } 
       }
}
void loop() 
{ char a;
  if (RFID.WaitStatus(10)){  // found card == 1
      Serial.println("");
      Serial.println("Found Card !!!");
      read_Taginfo();
      int sector = 0;
      
       if(RFID.LoginSector(0,keyA,USE_KEY_A) == LOGIN_SUCCEED){
        Serial.println("Login Succeed ");
        Serial.println("Press keyboard for next Operation");
        int block = 0;   
          while(1){
            if (Serial.available() > 0){
              a = Serial.read();
              Serial.println("");
              
              switch(a){
                case 'w':
                  Serial.println("To Test Write DATA");
                  test_writeDATA(2);
                break;
                case 'r':
                  Serial.println("To Test Read DATA");
                  test_readDATA(2);
                break;
                case 'v':
                  Serial.println("To Test Write Value");
                  test_writeValue(2);
                break;
                case 'i':
                  Serial.println("To Test Incresed Value");
                  test_IncreasedValue(2);
                break;
                case 'd':
                  Serial.println("To Test Decresed Value");
                  test_DecreasedValue(2);
                break;
                case 's':
                  Serial.println("To Test Set Key A");
                  test_SetKeyA(0); // set for sector 0
                break;
                case 'L':
                  Serial.println("To Test LED ON");
                  RFID.LED(0x01);
                break;
                case 'l':
                  Serial.println("To Test LED OFF");
                  RFID.LED(0x00);
                break;
              }
            }
           if (a == 'e') {
             Serial.println("Exit already ");
             Serial.println("");
             break; 
           }
          }
    } 
    
   else{
        Serial.println("Login Fail");
       }  
    
  }
}
