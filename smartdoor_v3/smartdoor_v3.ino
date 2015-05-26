// include function to check access from text file

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <SD.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

tmElements_t tm;
File sdcard;

#define MEMBER 2
#define DOOR 5

byte code[12];  //10+2 XXXXXXXXXX\r\0)    // X = ASCII of char
int ci = 0;
char user[MEMBER][12];

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
  if(!SD.begin(4)){
    Serial.println("Failed to connect SD card");
    return;
  }
  if(SD.exists("idaccess.txt")){
    Serial.println("Founded idaccess");
    read_file("idaccess.txt"); // get ID from file "idaccess.txt"  to user[][]
  }
  else{
    Serial.println("Cannot find idaccess.txt");
  }
  
  //set pin for controlling RFID
  pinMode(DOOR , OUTPUT);
}

unsigned long rfid_stamp = 0;
unsigned long serial2_stamp = 0;

void loop(){  
  updateTime(tm.Minute);
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
      if(isUser()){
        LEDprint("Pass");
        Serial.println("Pass");
        unlock_door();
        LEDprint("reset");
      }else{
        LEDprint("Denied");
        Serial.println("Denied");
      }  
}

void read_file(char *path){
   char tmp;
   int index=0;
   int num = 0;
   sdcard = SD.open(path);
   while(sdcard.available()){
       tmp = sdcard.read();
       if(tmp == '\n'){
           user[num][index] = '\0';
           num++;
           index = 0;
       }else{
         user[num][index] = tmp;
         index++;
       }     
   }
   sdcard.close();
   Serial.println("Already read text file.");
   Serial.println("Text : ");
   for(int i = 0 ; i < MEMBER ; i++){
     Serial.println(user[i]);
   }
   Serial.println("==============================");
}

boolean isUser(){
  for(int i =  0 ; i < MEMBER ; i++){
      if(!strcmp(user[i],(char*)code)){
          return true;
      }
  }
  return false;
}

void unlock_door(){
  digitalWrite(DOOR,HIGH);
  Serial.println("The door is unlock.");
  delay(5000);
  digitalWrite(DOOR,LOW);
  Serial.println("The door is lock.");
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
  else{
    display.setCursor(30, 17);
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
  
