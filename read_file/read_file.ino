#include <SD.h>
#include <SPI.h>
#include <Wire.h>
File sdcard;
#define member 5

typedef struct{
    char member_id[12];
    char member_name[10];
}rfid;

rfid pupa[member];
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
   char tmp;
   int index=0;
   int num = 0;
   int mode = 0;    // MODE 0 for id and MODE 1 for name
   sdcard = SD.open("idaccess.txt");
   while(sdcard.available()){
     tmp = sdcard.read();
     if(tmp == '\r'){
         pupa[num].member_name[index+1] = '\0';
         num++;
         index = 0;
         mode = 0;
     }
     else if(tmp == ' '){
       pupa[num].member_id[index+1] = '\0';
       index = 0;
       mode = 1;
     } 
     else if((tmp >= 'A' && tmp <= 'z') || (tmp >= '0' && tmp <= '9')){
       if(mode == 0){
         pupa[num].member_id[index] = tmp;
         
         Serial.print("  id ");
         Serial.print(num);
         Serial.print("  Index ");
         Serial.print(index);
         Serial.print("  info:");
         Serial.println(pupa[num].member_id[index]);
         
       }else{
         pupa[num].member_name[index] = tmp;
         
         Serial.print("  Name ");
         Serial.print(num);
         Serial.print("  Index ");
         Serial.print(index);
         Serial.print("  info:");
         Serial.println(pupa[num].member_name[index]);
       }
       
       index++;
     }
         
   }
   sdcard.close();
   
    Serial.println("=========================");
    for(int i = 0 ; i < member; i++ ){
      if(strcmp(pupa[i].member_id,"")){
        Serial.print("  Num ");
        Serial.print(i);
        Serial.print("  ");
        Serial.print(pupa[i].member_id);
        Serial.print("  ");
        Serial.println(pupa[i].member_name);
      }
    }
    Serial.println("=========================");
}




void loop()
{   
}
