#include "Timer.h"


Timer t;
int pin = 13;
int pushButton = 6;
boolean state = false;

int ledEvent;
void setup()
{
  Serial.begin(9600);
  pinMode(pushButton, INPUT);
  pinMode(13, OUTPUT);
  
  
}
int unlockEvent;

void loop()
{
  t.update();
  check();
}


void check(){
  int buttonState = digitalRead(pushButton);
  if(buttonState == HIGH){
    if(state){
      t.stop(unlockEvent);
    }
    digitalWrite(13,HIGH);
    state = true;
    unlockEvent = t.after(5000, doo);
    state = false;
  }
}
void doo(){
    digitalWrite(13,LOW);
}
