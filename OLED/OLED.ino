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
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();    
  
  LEDprint(2, WHITE, 10, 17, "PUPASOFT");
  delay(800);
  display.clearDisplay();  
}

void loop(){  
  printDate();
  printTime();  
  LEDprint(2, WHITE, 10, 17, "ATTACH RFID TO UNLOCK"); 
  delay(60000);
  display.clearDisplay();
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
  display.setCursor(1, 1);
  
  print2digits(tm.Hour);    
  display.write(':');    
  display.display();
  print2digits(tm.Minute);
}

void LEDprint(int textsize, byte *textcolor, int cursorX, int cursorY, char *text){
  display.setTextSize(textsize);
  display.setTextColor(textcolor);
  display.setCursor(cursorX, cursorY);
  display.println("%s", text);
  display.display();
}
  
  
  

  
  
