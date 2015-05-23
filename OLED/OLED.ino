#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

tmElements_t tm;

void setup(){  
  //setup OLED
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay(); 
  //date and time
  printDate();
  printTime();  
}

void loop(){   
  //screen message
  LEDprint(2, 30, 17, "ATTACH");
  LEDprint(2, 30, 40, "RFCARD");  
  //update time
  updateTime(tm.Minute);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    display.write('0');
  }
  display.print(number);
  display.display();
}

void printDate(){
  //get date from RTC
  RTC.read(tm);
  
  //set up text to display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  
  //display dd/mm/yy
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
}

void printTime(){
  //get time from RTC
  RTC.read(tm);
  
  //set up text to display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(90, 1);
  
  //display hh:mm
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
  
  
  
