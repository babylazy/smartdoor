// A simple web server that always just says "Hello World"

#include "etherShield.h"
#include "ETHER_28J60.h"
#include <SD.h>
#define ENC28J60_CONTROL_CS     8
// set up variables using the SD utility library functions:

File myFile;
char tmp;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 9;  

static uint8_t mac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};   // this just needs to be unique for your network, 
                                                                // so unless you have more than one of these boards
                                                                // connected, you should be fine with this value.
                                                           
static uint8_t ip[4] = {192, 168, 1, 15};                       // the IP address for your board. Check your home hub
                                                                // to find an IP address not in use and pick that
                                                                // this or 10.0.0.15 are likely formats for an address
                                                                // that will work.

static uint16_t port = 80;                                      // Use port 80 - the standard for HTTP

ETHER_28J60 ethernet;

void setup()
{ 
  ethernet.setup(mac, ip, port);
  pinMode(10, OUTPUT);
}

void loop()
{
  if (ethernet.serviceRequest())
  {
    if (!SD.begin(9)) {
    ethernet.print("<H1>Hello World</H1>");
    }
    else
    {
      myFile = SD.open("web.txt");
      while(myFile.available())
      {
      tmp = myFile.read();
      ethernet.print(tmp);
      
    }
    }
    ethernet.respond();
  }
  delay(100);
}

