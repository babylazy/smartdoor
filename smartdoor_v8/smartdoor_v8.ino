// add set delay time function
// file name's max length is 8.3
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <Timer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <SD.h>

#define OLED_RESET 4
#define PREFIX_LEN 2
Adafruit_SSD1306 display(OLED_RESET);

 // the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
//the IP address for the shield:
byte ip[] = { 10, 0, 0, 177 };    
// the router's gateway address:
byte gateway[] = { 10, 0, 0, 1 };
// the subnet:
byte subnet[] = { 255, 255, 0, 0 };
// telnet defaults to port 23
EthernetServer server = EthernetServer(23);
EthernetClient activeClient;
Timer t;
tmElements_t tm;
File sdcard;
File activity;

#define MEMBER 30
#define DOOR 22
#define Button 24
#define FILE_ID "idaccess.txt"
#define FILE_LOG "activity.txt"


typedef struct{
    char member_id[12];
    char member_name[15];
}rfid;

rfid pupa[MEMBER]; // pathern -> "<member_id> <member_name>\r\n" 
byte code[12];  //10+2 XXXXXXXXXX\r\0)    // X = ASCII of char
int ci = 0;
boolean doorState = false;
int lockEvent;
unsigned long delayTime = 2000; // time in ms

void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600);
  
  //initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
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
  sdcard_process();
  
  //set pin for controlling RFID
  pinMode(DOOR , OUTPUT);
  pinMode(Button , INPUT);
}

unsigned long rfid_stamp = 0;

void loop(){
  t.update();
  updateTime(tm.Minute);
  unlockButton();
  process_server();
}  
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

void process_server(){
  EthernetClient client = server.available();
  
  if(client && activeClient != client){
    activeClient.stop();
    activeClient = client;
  }
  int done = 0;
  while(activeClient.available() > 0 && !done) {
    done = process_command(activeClient.read());
  }

  if(activeClient && !activeClient.connected()){
    activeClient.stop();
  } 
}
char command[256];
int cmd_index = 0;
int process_command(char tmp){
  if(tmp != '\n'){
      command[cmd_index++] = tmp;
      return 0;
  }else{
      command[cmd_index] = '\0';  
      cmd_index = 0;    
  }
  char* cmd = trimwhitespace(command);
  if(!strcmp(cmd,"help")){
    server_print_command();
    return 1;
  }
  if(cmd[1] != '_'){
    activeClient.println("Unknown Command!!!");
    activeClient.println("command ""help"" --> show how to command ");
    return 1;
  }
  switch(cmd[0]){
    case '1' : show_member();  return 1; break;
    case '2' : add_member(strcut(cmd,PREFIX_LEN,strlen(cmd)));  return 1; break;
    case '3' : remove_member(strcut(cmd,PREFIX_LEN,strlen(cmd)));  return 1; break;
    case '4' : set_delaytime(strcut(cmd,PREFIX_LEN,strlen(cmd))); return 1; break;
    case '5' : show_delaytime(); return 1; break;
    default  : activeClient.println("Unknown Command!!!");
               activeClient.println("command ""help"" --> show how to command ");
               return 1; break;
  }
}

void server_print_command(){
  activeClient.println("===================================");
  activeClient.println("======= How to use command ========");
  activeClient.println("===================================");
  activeClient.println("Show member   : 1_");
  activeClient.println("Add  member   : 2_<rfid_code> <name>");
  activeClient.println("Remove member : 3_<name> or 3_<rfid_code>");
  activeClient.println("Set delayTime : 4_<time>");
  activeClient.println("Show delayTime : 5_");
  activeClient.println("==========================================");
}

void show_member(){
    activeClient.println("Member : ");
    for(int i = 0 ; i < MEMBER; i++ ){
      if(strcmp(pupa[i].member_id,"")){
        activeClient.print("No.");
        activeClient.print(i);
        activeClient.print(": ");
        activeClient.print(trimwhitespace(pupa[i].member_id));  //remove '\r'
        activeClient.print(" ");
        activeClient.println(pupa[i].member_name);
      }
    }
    activeClient.println("=======================");
    activeClient.println("Show member Done!!");
}

void add_member(char* str){
  Serial.print("add : ");
  Serial.println(str);
  if(strlen(str) >= 10 ){
    sdcard = SD.open(FILE_ID,FILE_WRITE);
    sdcard.println(str);
    sdcard.close();
    Serial.println("Adding member Success!!");
    activeClient.println("Adding member Success!!");
    sdcard_process(); //update member list
  }else{
    Serial.println("Adding member Fail!!");
    activeClient.println("Adding member Fail!!");
  }
  activeClient.print("Adding  ");
  activeClient.print(trimwhitespace(str));
  activeClient.println("  Done!!");
}

void remove_member(char* str){
  int member_index = isUser(str);
  if(member_index < 0){
    activeClient.println("Fail to delete!!");
    return;
  }
  SD.remove(FILE_ID);
  for(int i = member_index; i+1 < MEMBER ; i++){
    if(strcmp(pupa[i+1].member_id, 0 )){
      strcpy(pupa[i].member_id, pupa[i+1].member_id);
      strcpy(pupa[i].member_name, pupa[i+1].member_name);
    }else{
      strcpy(pupa[i].member_id, 0);
      strcpy(pupa[i].member_name, 0);
    }
  }
  write_file(FILE_ID);
  activeClient.print("Deleting  ");
  activeClient.print(trimwhitespace(str));
  activeClient.println("  Done!!");
   
}

void set_delaytime(char* str){
  long time_tmp = str2long(str);
  if(time_tmp > 0){
    delayTime = time_tmp;
    activeClient.println("Change DelayTime Done!!");
    show_delaytime();
  }else{
    activeClient.println("DelayTime must be more than 0 !!");
  }
}

void show_delaytime(){
  activeClient.print("Delay Time : ");
  activeClient.println(delayTime);
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
    check_IDAccess();
  }
}

void check_IDAccess(){
      if(isUser((char*)code) >= 0){
        unlock_door("card");
      }else{
        LEDprint("Denied");
        Serial.println("ACCESS Denied");
        writeLOG("Unknown User",(char*)code);
        delay(1000);
        LEDprint((char*)code);
        delay(5000);
        LEDprint("reset");
      }  
}

void unlock_door(char* type){
  if(doorState){
    t.stop(lockEvent);
  }
  digitalWrite(DOOR,HIGH);
  doorState = true;
  if(!strcmp(type,"card")){
    Serial.print("Access accept : ");
    Serial.println(pupa[isUser((char*)code)].member_name);
    Serial.println();
    LEDprint(pupa[isUser((char*)code)].member_name);
    writeLOG(pupa[isUser((char*)code)].member_name,(char*)code);
  }else if(!strcmp(type,"button")){
    Serial.println("unlock by switch\n");
    LEDprint("UNLOCK by SWITCH");
    writeLOG("Member","unlock by switch");
  }
  Serial.println("The door is unlock.");
  lockEvent = t.after(delayTime , lock_door);
}
void lock_door(){
  digitalWrite(DOOR,LOW);
  doorState = false;
  Serial.println("The door is lock.");
  LEDprint("reset");
}

int isUser(char *str){
  char *user = trimwhitespace(str);
  for(int i =  0 ; i < MEMBER ; i++){
    char *pupa_name = trimwhitespace(pupa[i].member_name);
    char *pupa_id   = trimwhitespace(pupa[i].member_id);
      if((!strcmp(pupa_id,user) || !strcmp(pupa_name,user)) && strcmp(pupa_id,"")){
          return i;
      }
  }
  Serial.println("Cannot find id\n");
  return -1;
}
void unlockButton(){
  int buttonState = digitalRead(Button);
  if(buttonState == HIGH){
    unlock_door("button");
  }
}

void sdcard_process(){
  
    if(SD.exists(FILE_LOG)){
      Serial.print("Founded ");
      Serial.println(FILE_LOG);      
    }else{
      Serial.print("Didn't found ");
      Serial.println(FILE_LOG);
      activity = SD.open(FILE_LOG,FILE_WRITE);
      if(activity){
        Serial.print("Already create ");
        Serial.println(FILE_LOG);
        activity.close();
      }
    }
    
    if(SD.exists(FILE_ID)){
      Serial.print("Founded ");
      Serial.println(FILE_ID);
      read_file(FILE_ID); // get ID from file "idaccess.txt"  to user[][]
    } else{
      Serial.print("Didn't found ");
      Serial.println(FILE_ID);
      sdcard = SD.open(FILE_ID,FILE_WRITE);
      if(sdcard){
        Serial.print("Already create ");
        Serial.println(FILE_ID);
        sdcard.close();
      }
    }
}

void read_file(char *path){
   char tmp;
   int index=0;
   int num = 0;
   int mode = 0;    // MODE 0 for id and MODE 1 for name
   sdcard = SD.open(path);
   while(sdcard.available()){
     tmp = sdcard.read();
     if(tmp == '\r'){
         pupa[num].member_name[index] = '\r';
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

void write_file(char *path){
  sdcard = SD.open(path,FILE_WRITE);
  for(int i = 0 ; i < MEMBER ; i++){
    if(strcmp(trimwhitespace(pupa[i].member_id),"")){
      sdcard.print(trimwhitespace(pupa[i].member_id));
      sdcard.print(" ");
      sdcard.println(pupa[i].member_name);
    }
  }
  sdcard.close();
}

void writeLOG(char *username,char *id){
    activity = SD.open(FILE_LOG,FILE_WRITE);
    activity.print(tm.Day); 
    activity.write('/');
    activity.print(tm.Month); 
    activity.write('/'); 
    activity.print(tmYearToCalendar(tm.Year));
    activity.print("   ");
    activity.print(tm.Hour);  
    activity.write(':');
    activity.print(tm.Minute);
    activity.print("   ");
    activity.print(id);
    activity.print("   ");
    activity.println(username);
    activity.close();
}

char *num2digit(int num){
  char digit[2];
    if(num < 9){
      digit[0] = '0';
      digit[1] = (char)num;
      return digit;
    }
    return (char*)num;
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
  else if(isUser((char*)code) >= 0 && strcmp((char*)code,"")){
      display.setCursor(30, 17);
      display.println("Hello");
      display.display();
      display.setCursor(30, 40);
      display.println(text);
      display.display();
  }
  else{
      display.setCursor(0, 17);
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

char *strcut(char *str , int head , int tail){
  char *end = str + tail - 1;
  *(end+1) = '\0';
  while(head--) str++;
//  Serial.print("Str : ");
//  Serial.println(str);
  return str;
}

long str2long(char* str){
  long value = 0;
  int i,j;
  for(i = strlen(str)-1,j = 0 ; i >= 0 ; i-- ,j++){
      value += (str[i] - '0')* power10(j);
  }
  return value;
}
long power10(int n){
  long sum = 1 ;
  for(int i = 0 ; i < n ; i++){
    sum *= 10;
  }
  return sum;
}
