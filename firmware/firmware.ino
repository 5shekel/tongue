/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/tongue

   pin out attiny85
   NC      -1+----+8-   VCC
   3/A3    -2|*   |7-   2/A1
   4/A4    -3|    |6-   1
   GND     -4+----+5-   0/PWM

  using:
  ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  sofServo http://playground.arduino.cc/ComponentLib/Servo
  SendOnlySoftwareSerial https://forum.arduino.cc/index.php?topic=112013.msg841582#msg841582
  tiny_ir https://gist.github.com/SeeJayDee/caa9b5cc29246df44e45b8e7d1b1cdc5
  ir_methods https://github.com/z3t0/Arduino-IRremote/wiki/Receiving-with-the-IRremote-library

  attiny85 pinout - https://camo.githubusercontent.com/081b569122da2244ff7de8bae15eb56947d05cc8/687474703a2f2f6472617a7a792e636f6d2f652f696d672f50696e6f7574543835612e6a7067
 *******************************************************************/

#include <EEPROM.h>

#define SERVO1PIN 0   // Servo control line (orange) on 0/PWM
#define POTPIN   A3  // Potentiometer on A3
#define TXPIN 1 // tested on PB1/PB4
#define RECVPIN  4 //ir reciver data pin , tested on PB1/PB4

#include "SoftwareServo.h"
SoftwareServo softServo;  // create servo object to control a servo
int prevpotValue;

#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial softSerial (TXPIN);  // Tx pin

#include "tiny_IRremote.h"
IRrecv irrecv(RECVPIN);
decode_results results;
unsigned long  prevValue;
long lastPressTime = 0;

int pos_middle = 82;
int runningDelta = 0; //soft calbrate cneter

int PROG = 1;
int outPos;

int servoPos, potValue, gestrue01;

void setup() {

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.print("reboot\n");

  EEPROM.get(0, runningDelta );
  softSerial.println(runningDelta);

  //EEPROM.get(1, runningDelta );
  softSerial.println(runningDelta);

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFF30CF:
        gestrue01 = potValue;
        softSerial.print("[1 forced learning gestrue01:] "); softSerial.println(gestrue01);
        break;

      case 0xFF6897:
        //potValue is the raw input from sensor
        // gestrue01 is the pre recorded "forced learning" value
        //
        /*
          //write the diff between natural middle and forced learning
          value dump [in forced learning stable]
          servoPos  runningDelta   potValue  potValue
          69        0         69            278

          value dump [naturla pos]
          servoPos  runningDelta   potValue  potValue
          129       0         129           516

          == example usage
          [EQ] - value dump 0xFF906F
          servoPos runningDelta potValue potValue
          81       0       81          324

          [1 gestrue01] 79

          [0 runningDelta] 35

          [EQ] - value dump 0xFF906F
          servoPos runningDelta potValue potValue
          80        35       115         461


        */
        runningDelta = potValue - gestrue01;
        EEPROM.put(0, runningDelta);
        softSerial.print("[0 runningDelta (potValue - gestrue01)"); softSerial.println(runningDelta);
        break;

      case 0xFF906F:
        softSerial.println("[EQ] - value dump 0xFF906F");
        printDebug();

      case 0xFFFFFFFF:
        break;
      default:
        softSerial.print(F("unknown = 0x"));
        softSerial.println(results.value, HEX);
    }
    irrecv.resume(); 
  }

  potValue = analogRead(POTPIN);              // Read voltage on potentiometer
  potValue = potValue / 4 ; //map(potValue, 0, 1024, 0, 179);
  servoPos =  potValue - runningDelta;

  softServo.write(servoPos);

  //analogWrite(SERVO1PIN, potValue); //doesnt work, servo need PPM 0.5-2ms

  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position
}




void     printDebug() {
  softSerial.println(" servoPos runningDelta potValue potValue");
  softSerial.print(servoPos); softSerial.print(" ");
  softSerial.print(runningDelta); softSerial.print(" ");
  softSerial.print(potValue); softSerial.print(" ");
  softSerial.println();
}

//tongueDiff(potValue);

// map the range of pot to limited map range on the servo.
// a narrow band of the full range.
// in both directions, with a stop space in middle
/*
  topSpeedL = 20;
  topSpeedR = 150;
  lowSpeedL = 80;
  lowSpeedR = 90;
*/
byte tongueDiff(int val, byte mid, int  ) {
  byte potValue;
  if ( val < 350 ) {
    potValue = map(val, 0, 350, 20, 80); //the88p0 has less
  } else if (val > 650) {
    potValue = map(val, 1024, 650, 150, 90);
  } else {
    potValue = pos_middle ;// 82; //86 with the 880
  }
  return potValue;
}
