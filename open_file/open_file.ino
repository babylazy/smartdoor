// test how to use multi files
// file name's max length s 8.3
#include <SD.h>
#include <Wire.h>
#include <SPI.h>

#define path1 "idaccess.txt"
#define path2 "activity.txt"

File sdcard;
File activity;

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
    sdcard = SD.open(path1);
  }else{
    Serial.print("cannot find ");
    Serial.println(path1);
    sdcard = SD.open(path1,FILE_WRITE);
    if(SD.exists(path1)){
      Serial.print("already create ");
      Serial.println(path1);
    }
  }
  sdcard.close();
  if(SD.exists(path2)){
    Serial.print("found ");
    Serial.println(path2);
    activity = SD.open(path2);
  }else{
    Serial.print("cannot find ");
    Serial.println(path2);
    activity = SD.open(path2,FILE_WRITE);
    if(SD.exists(path2)){
      Serial.print("already create ");
      Serial.println(path2);
    }
  }
  activity.close();
}



void loop(){
}
