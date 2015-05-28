// include timer , activity log(inactive) , unlock button , reset button

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <SD.h>
#include <Timer.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Timer t;
tmElements_t tm;
File sdcard;
//File activity;

#define MEMBER 20
#define DOOR 5
#define Button 6
#define RESET 7
#define FILE_ID "idaccess.txt"
#define FILE_LOG "activitylog.txt"

typedef struct{
    char member_id[12];
    char member_name[10];
}rfid;

rfid pupa[MEMBER]; // pathern -> "<member_id> <member_name>\r\n" 
byte code[12];  //10+2 XXXXXXXXXX\r\0)    // X = ASCII of char
int ci = 0;
boolean doorState = false;
int lockEvent;
void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600);
  //setup OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay(); 
  LEDprint("reset");  
  
  //SD card
  pinMode(SS,OUTPUT);
  sdcard_process();
  
  //set pin for controlling RFID
  pinMode(DOOR , OUTPUT);
  pinMode(Button, INPUT);
  pinMode(RESET, INPUT);
}

unsigned long rfid_stamp = 0;
unsigned long serial2_stamp = 0;

void loop(){
  t.update();
  updateTime(tm.Minute);
  unlockButton();
  resetButton();
  //rfid
  byte bytesRead = 0;
  byte val = 0;
  while(Serial1.available() > 0 && bytesRead < 16) { //make sure while not run indefinitely
      val = Serial1.read();
      bytesRead++;        
      if(val == 0x02){
         ci = 0;
      }else if(val == 0x0D || val == 0x0A){ //ignore \r\n
         continue;
      }else if(val == 0x03){
         process_code();
         break;
      }else if(ci < 12) {  
         code[ci++] = val;
      }  
  }  
//  Serial.println("Active");
}  
      
void process_code() {
  byte checksum = hexstr2b(code[10], code[11]);
  byte test = hexstr2b(code[0], code[1]);
  int i;
  for(i = 1; i < 5; i++){
    test ^= hexstr2b(code[i * 2], code[i*2 + 1]);
  }
  if(test == checksum){ //valid tag code
    code[10] = '\r';
    code[11] = '\0';
    
    rfid_stamp = millis();
    Serial.print("Code : ");
    Serial.println((char*)code);
    Serial.println("Already read RFID");
    check_IDAccess();
  }
}

void check_IDAccess(){
      if(isUser() >= 0){
//        writeLOG(pupa[isUser()].member_name);
        
        unlock_door();
      }else{
        LEDprint("Denied");
//        writeLOG("Unknown");
        Serial.println("Denied");
        delay(1000);
        LEDprint("reset");
      }  
}

void unlock_door(){
  if(doorState){
    t.stop(lockEvent);
  }
  digitalWrite(DOOR,HIGH);
  doorState = true;
  LEDprint(pupa[isUser()].member_name);
  Serial.println("The door is unlock.");
  lockEvent = t.after(5000 , lock_door);
}
void lock_door(){
  digitalWrite(DOOR,LOW);
  doorState = false;
  Serial.println("The door is lock.");
  LEDprint("reset");
}

int isUser(){
  for(int i =  0 ; i < MEMBER ; i++){
      if(!strcmp(pupa[i].member_id,(char*)code) && strcmp(pupa[i].member_id,"")){
          return i;
      }
  }
  Serial.println("Cannot find id");
  return -1;
}
void unlockButton(){
  int buttonState = digitalRead(Button);
  if(buttonState == HIGH){
    unlock_door();
  }
}
void resetButton(){
  int resetState = digitalRead(RESET);
  if(resetState == HIGH){
    resetFunc();
  }
}


void sdcard_process(){
    if(!SD.begin(4)){
      Serial.println("Failed to connect SD card");
      return;
    }
    if(SD.exists(FILE_ID)){
      Serial.print("Founded ");
      Serial.println(FILE_ID);
      read_file(FILE_ID); // get ID from file "idaccess.txt"  to user[][]
    }
    else{
      Serial.print("Didn't found ");
      Serial.println(FILE_ID);
    }
    
//    if(SD.exists(FILE_LOG)){
//      Serial.print("Founded ");
//      Serial.println(FILE_LOG);
//      activity = SD.open(FILE_LOG,FILE_WRITE);
//    }
//    else{
//      Serial.print("Didn't found ");
//      Serial.println(FILE_LOG);
//    }
}
//void writeLOG(char *username){
//    activity.print(tm.Day); 
//    activity.write('/');
//    activity.print(tm.Month); 
//    activity.write('/'); 
//    activity.print(tmYearToCalendar(tm.Year));
//    activity.print("      ");
//    activity.print(num2digit(tm.Hour));  
//    activity.write(':');
//    activity.print(num2digit(tm.Minute));
//    activity.print("      ");
//    activity.print((char*)code);
//    activity.print("      ");
//    activity.println(username);
//}
//char *num2digit(int num){
//  char digit[2];
//    if(num < 9){
//      digit[0] = '0';
//      digit[1] = (char)num;
//      return digit;
//    }
//    return (char*)num;
//}

void read_file(char *path){
   char tmp;
   int index=0;
   int num = 0;
   int mode = 0;    // MODE 0 for id and MODE 1 for name
   sdcard = SD.open(path);
   while(sdcard.available()){
     tmp = sdcard.read();
     if(tmp == '\r'){
         pupa[num].member_name[index+1] = '\0';
         num++;
         index = 0;
         mode = 0;
     }
     else if(tmp == ' '){
       pupa[num].member_id[index] = '\r';
       pupa[num].member_id[index+1] = '\0';
       index = 0;
       mode = 1;
     } 
     else if((tmp >= 'A' && tmp <= 'z') || (tmp >= '0' && tmp <= '9')){
       if(mode == 0){
         pupa[num].member_id[index] = tmp;
       }else if(mode == 1){
         pupa[num].member_name[index] = tmp;
       }
       index++;
     }
         
   }
   sdcard.close();
   
   Serial.println("Already read text file.");
   Serial.println("Information : ");
    for(int i = 0 ; i < MEMBER; i++ ){
      if(strcmp(pupa[i].member_id,"")){
        Serial.print("No.");
        Serial.print(i);
        Serial.print("  ");
        Serial.print(pupa[i].member_id);
        Serial.print("  ");
        Serial.println(pupa[i].member_name);
      }
    }
    Serial.println("=========================");
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    display.write('0');
  }
  display.print(number);
  display.display();
}

void printDateAndTime(){
  RTC.read(tm);
  
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  
  display.print(tm.Day);    
  display.display();
  display.write('/');    
  display.display();
  display.print(tm.Month);    
  display.display();
  display.write('/');    
  display.display();
  display.print(tmYearToCalendar(tm.Year));    
  display.display();
  display.print("      ");
  display.display();
  
  display.setCursor(90, 1);
  
  print2digits(tm.Hour);    
  display.write(':');    
  display.display();
  print2digits(tm.Minute);
}

void LEDprint(char *text){
  //clear screen
  printDateAndTime();
  
  //print text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if(!strcmp(text,"reset")){
    display.setCursor(30, 17);
    display.println("ATTACH");
    display.display();
    display.setCursor(30, 40);
    display.println("RFCARD");
    display.display();
  }
  else if(!strcmp(text,"Denied")){
    display.setCursor(30, 17);
    display.println("Access");
    display.display();
    display.setCursor(30, 40);
    display.println("Denied");
    display.display();
  }
  else{
    display.setCursor(30, 17);
    display.println("Hello");
    display.display();
    display.setCursor(30, 40);
    display.println(text);
    display.display();
  }
}

void updateTime(int currentMinute){
  RTC.read(tm);
  if(currentMinute != tm.Minute){
    //date and time  
    printDateAndTime();
    //screen message
    LEDprint("reset");
  }
}

//------utilities functions-------------
byte hexstr2b(char a, char b){
  return (byte)((hexchar2b(a) * 16) + hexchar2b(b));
}

byte hexchar2b(char hex){
  if(hex >= '0' && hex <= '9')
    return hex -'0';
  if(hex >= 'A' && hex <= 'F'){     
    return 10 + (hex - 'A');  
  }
}

char *trimwhitespace(char *str) {
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
  
