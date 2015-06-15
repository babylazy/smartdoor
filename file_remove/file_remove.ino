// test how to delete information
#include <SD.h>
#include <SPI.h>

#define path1 "id2.txt"

File sdcard;

void setup(){
  Serial.begin(9600);
  if(!SD.begin(4)){
    Serial.println("Cannot connect SD card");
    return;
  }else{
    Serial.println("Connect SD card");
  }
  if(SD.exists(path1)){
    Serial.print("found ");
    Serial.println(path1);
    sdcard = SD.open(path1,FILE_WRITE);
    sdcard.println("123451234512345");
    sdcard.close();
  }else{
    Serial.print("cannot find ");
    Serial.println(path1);
    sdcard = SD.open(path1,FILE_WRITE);
    if(SD.exists(path1)){
      Serial.print("already create ");
      Serial.println(path1);
      sdcard.println("1234512345");
      sdcard.close();
    }
  }
  sdcard = SD.open(path1,FILE_WRITE);
  sdcard.seek(0);
  sdcard.print("7F",HEX);
  sdcard.prin("7F",HEX);
  sdcard.close();
  sdcard = SD.open(path1);
  while(sdcard.available()){
    Serial.print((char)sdcard.read());
  }
  Serial.println();
  sdcard.close();
}



void loop(){
}
