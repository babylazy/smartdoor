#include <SPI.h>
#include <Wire.h>

void(* resetFunc) (void) = 0;
int RESET = 7;
int ledpin = 13;
void setup() {
  Serial.begin(9600);
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin , HIGH);
  delay(5000);
}

void loop(){
  // put your main code here, to run repeatedly:
  digitalWrite(ledpin , LOW);
  resetButton();
}
void resetButton(){
  int resetState = digitalRead(RESET);
  if(resetState == HIGH){
    resetFunc();
  }
}
