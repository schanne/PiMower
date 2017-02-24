/* Testing Resistive Touch Screen
*/

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x
 MISO:  12//Teensy3.x
 SCK:   13//Teensy3.x
 the rest of pin below:
 */

#define RA8875_INT 2 //any pin that have an INT
#define RA8875_CS 10 //see below...
/*Teensy 3.x can use: 2,6,9,10,15,20,21,22,23 */
#define RA8875_RESET 23//any pin or nothing!


static uint16_t tx, ty;//used as temp


RA8875 tft = RA8875(RA8875_CS);//Teensy3/arduino's

void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);//initialize RA8875
  /* Adafruit_480x272 Adafruit_800x480 RA8875_480x272 RA8875_800x480 */
  tft.useINT(RA8875_INT);//We use generic int helper for Internal Resistive Touch
  tft.touchBegin();//enable touch support for RA8875
  if (!tft.touchCalibrated()) {//already calibrated?
    Serial.println("Maybe better you calibrate first!");
  }
  //this enable an ISR on CPU and RA8875 INT
  tft.enableISR(true);
  //You can avoid ISR by simple ignore the line above
  //it will use the slower digitalRead(pin) alternative internally
}

void loop() {
  if (tft.touched()) {
    //if you are here means that you touched screen
    tft.touchReadPixel(&tx,&ty);//read directly in pixel
    tft.fillCircle(tx,ty,5,0xFFFF);
  }

}