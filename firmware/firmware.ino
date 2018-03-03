/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/tongue

   pin out attiny85
   RST     -1+----+8-   VCC
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
#define TXPIN     1 // tested on PB1/PB4
#define POTPIN    3  // Potentiometer on A3
#define RECVPIN   4 //ir reciver data pin , tested on PB1/PB4

#include "SoftwareServo.h"
SoftwareServo softServo;  // create servo object to control a servo

#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial softSerial (TXPIN);  // Tx pin

#include "tiny_IRremote.h"
IRrecv irrecv(RECVPIN);
decode_results results;
unsigned long  prevValue;

int pos_middle = 0;
int servoPos, potValue, servoSpeed;
int runningDelta, gestrue01;

void setup() {

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("reboot");

  EEPROM.get(0, runningDelta);
  softSerial.println(runningDelta);
  EEPROM.get(2, gestrue01 );
  softSerial.println(gestrue01);


  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {

  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA857:
        servoSpeed += 0.3;
        softSerial.print("[+ servoSpeed: ] "); softSerial.println(servoSpeed);
        printDebug();
        break;
      case 0xFFE01F:
        servoSpeed -= 0.3;
        softSerial.print("[- servoSpeed: ] "); softSerial.println(servoSpeed);
        printDebug();
        break;
      case 0xFF30CF:

        gestrue01 = potValue;
        EEPROM.put(2, gestrue01);
        softSerial.print("[1 learn gestrue01: ] "); softSerial.println(gestrue01);
        printDebug();
        break;

      case 0xFF6897:
        /*
          //potValue is the raw input from sensor
          // gestrue01 is the pre recorded "learning" value
          //
          //write the diff between natural middle and forced learning
          value dump [in forced learning stable]
          servoPos  runningDelta   potValue
          69        0               69

          value dump [naturla pos]
          servoPos  runningDelta    potValue
          129       0               129

          == example usage
          [EQ] - value dump 0xFF906F
          servoPos runningDelta potValue
          81       0             81

          [1 gestrue01] 79

          [0 runningDelta] 35

          [EQ] - value dump 0xFF906F
          servoPos runningDelta potValue
          80        35          115
        */

        runningDelta = potValue - gestrue01;
        EEPROM.put(0, runningDelta);
        softSerial.print("[0 new runningDelta: "); softSerial.println(runningDelta);
        printDebug();
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
  potValue = analogRead(POTPIN)/4 ;
  //potValue = map(potValue, 0, 1024, 0, 179) ;

  servoPos =  potValue - runningDelta ;
  /*
  if ( val < 350 ) {
    potValue = map(val, 0, 350, 20, 80); //the88p0 has less
  } else if (val > 650) {
    potValue = map(val, 1024, 650, 150, 90);
  } else {
    potValue = pos_middle ;// 82; //86 with the 880
  }
  */
  
  softServo.write(servoPos);
  //softSerial.print(potValue); softSerial.println("    ");
  //analogWrite(SERVO1PIN, potValue); //doesnt work, servo need PPM 0.5-2ms

  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position
}




void     printDebug() {
  //softSerial.println("potValue gestrue01 runningDelta servoPos vlt");
  softSerial.print(potValue); softSerial.print("    ");
  softSerial.print(gestrue01); softSerial.print("    ");
  softSerial.print(runningDelta); softSerial.print("      ");
  softSerial.print(servoPos); softSerial.print("      ");
  // softSerial.print(analogRead(POTPIN) * (5 / 1024)); softSerial.print("      ");

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
  //byte potValue;
  if ( val < 350 ) {
    potValue = map(val, 0, 350, 20, 80); //the88p0 has less
  } else if (val > 650) {
    potValue = map(val, 1024, 650, 150, 90);
  } else {
    potValue = pos_middle ;// 82; //86 with the 880
  }
  return potValue;
}
