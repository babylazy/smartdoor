#include <SD.h>
#include <SPI.h>
#include <Wire.h>
File sdcard;

void setup()
{
  Serial.begin(9600);
    //SD card
  pinMode(SS,OUTPUT);
  if(!SD.begin(4)){
    Serial.println("Failed to connect SD card");
    return;
  }
  if(SD.exists("idaccess.txt")){
    Serial.println("Founded idaccess");
    Serial.println("=========================");
  }
  else{
    Serial.println("Cannot find IDACCESS.TXT");
  }
  
  char info[2][13];
   char tmp;
   int index=0;
   int num = 0;
   sdcard = SD.open("idaccess.txt");
   while(sdcard.available()){
     tmp = sdcard.read();
     if(tmp == '\n'){
       info[num][index] = '\0';
       Serial.print("  Num ");
       Serial.print(num);
       Serial.print("  Index ");
       Serial.print(index);
       Serial.print("  info:");
       Serial.println(info[num][index]);
       num++;
       index = 0;
     }
     else{
         info[num][index] = tmp;
       Serial.print("  Num ");
       Serial.print(num);
       Serial.print("  Index ");
       Serial.print(index);
       Serial.print("  info:");
       Serial.println(info[num][index],HEX);
       index++;
     }     
   }
   sdcard.close();
   
    Serial.println("=========================");
    for(int i = 0 ; i < 2 ; i++ ){
      Serial.print("  Num ");
      Serial.print(i);
      Serial.print("  ");
      Serial.print(info[i]);
      Serial.print("  ");
      Serial.println(strlen(info[i]));
    }
    Serial.println("=========================");
}




void loop()
{   
}
