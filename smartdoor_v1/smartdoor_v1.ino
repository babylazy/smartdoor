// include function to read rfid

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

void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600);
  //setup OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();  
  //date and time
  printDate();
  printTime();
  
  //SD card
  pinMode(SS,OUTPUT);
  if(!SD.begin(4)){
    Serial.println("Failed to connect SD card");
    return;
  }
  if(SD.exists("idaccess.txt")){
    Serial.println("Founded idaccess");
  }
  else{
    Serial.println("Cannot find IDACCESS.TXT");
  }
  //set pin for controlling RFID
  pinMode(5 , OUTPUT);
}

unsigned long rfid_stamp = 0;
unsigned long serial2_stamp = 0;
void loop(){ 
    
  
  //screen message
  LEDprint(2, 30, 17, "ATTACH");
  LEDprint(2, 30, 40, "RFCARD");  
  
  //update time
  updateTime(tm.Minute);
  
  //delay 1 minute and update time  
  delay(60000);
  display.clearDisplay();

}

byte code[12];  //12 + 4 (RF_XXXXXXXXXXXX\0)
int ci = 0;  
void serialEvent1(){
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
    code[11] = '\n';
    
    rfid_stamp = millis();
    //show RFID to monitor
    for(i = 0; i < 12 ; i++){
      Serial.println(code[i]);
    }
    //check access
    if(check_IDAccess((char*)code)){
      digitalWrite(5,HIGH);
      LEDprint(2,30,17,"Pass");
    }
    else{
      LEDprint(2,30,17,"Denied");
    }
  }
}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    display.write('0');
  }
  display.print(number);
  display.display();
}

void printDate(){
  RTC.read(tm);
  
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
}

void printTime(){
  RTC.read(tm);
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(90, 1);
  
  print2digits(tm.Hour);    
  display.write(':');    
  display.display();
  print2digits(tm.Minute);
}

void LEDprint(int textsize, int cursorX, int cursorY, char *text){
  display.setTextSize(textsize);
  display.setTextColor(WHITE);
  display.setCursor(cursorX, cursorY);
  display.println(text);
  display.display();
}

void updateTime(int currentMinute){
  RTC.read(tm);
  if(currentMinute != tm.Minute){
    display.clearDisplay();  
    printDate();
    printTime();
  }
}

boolean check_IDAccess(char* code){
    // redisplay prepare for show access
    display.clearDisplay();
    printDate();
    printTime(); 
    
    //check id to allow access
    while(sdcard.available()){
      char* id = read_id();
      if(!strcmp(id,code)){
        return true;
      } 
    }
    return false;
}

char* read_id(){
    char* id = (char*)malloc(sizeof(char)*12);
    int index = 0;
    if(sdcard.available()){
      for(index = 0 ; index < 12 ; index++){
         id[index] = sdcard.read();
      }
    }
    return id;
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
  
