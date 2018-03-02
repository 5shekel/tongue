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
int prevServoPos;

#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial softSerial (TXPIN);  // Tx pin

#include "tiny_IRremote.h"
IRrecv irrecv(RECVPIN);
decode_results results;
unsigned long  prevValue;
long lastPressTime = 0;

int pos_middle = 82;
int trimVal = 0; //soft calbrate cneter

int PROG = 1;
int outPos;

int servoPos;
  int potValue;
  
void setup() {

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.print("reboot\n");

  EEPROM.get(0, pos_middle );
  softSerial.println(pos_middle);

  //EEPROM.get(1, trimVal );
  softSerial.println(trimVal);

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D:
        softSerial.print("[CH-] pos_mid "); softSerial.println(trimVal);
        trimVal--;

        softSerial.print(servoPos); softSerial.print(" ");
        softSerial.print(trimVal); softSerial.print(" ");
        softSerial.print(outPos); softSerial.print(" ");
        softSerial.print(potValue); softSerial.print(" ");
        softSerial.println();

        break;
      case 0xFFE21D:
        softSerial.print("[CH+] posmid  "); softSerial.println(trimVal);
        trimVal++;

        softSerial.print(servoPos); softSerial.print(" ");
        softSerial.print(trimVal); softSerial.print(" ");
        softSerial.print(outPos); softSerial.print(" ");
        softSerial.print(potValue); softSerial.print(" ");
        softSerial.println();

        break;
      case 0xFF629D:
        EEPROM.put(0, trimVal );
        softSerial.print("[CH] write "); softSerial.println(trimVal);
        break;

      case 0xFF906F:
        softSerial.print("value dump\n");
        softSerial.print("pos_middle  -  "); softSerial.println(pos_middle);
        softSerial.println();

      case 0xFFFFFFFF:
        break;
      default:
        softSerial.print(F("unknown = 0x"));
        softSerial.println(results.value, HEX);
    }
    irrecv.resume(); // Receive the next value

  }

  servoPos;
  potValue = analogRead(POTPIN);              // Read voltage on potentiometer

  // map the range of pot to limited map range on the servo.
  // a narrow band of the full range.
  // in both directions, with a stop space in middle

  //tongueDiff(potValue);
  /*
    topSpeedL = 20;
    topSpeedR = 150;
    lowSpeedL = 80;
    lowSpeedR = 90;
  */

  PROG = 2;
  switch (PROG) {
    case 1:
      if ( potValue < 350 ) {
        servoPos = map(potValue, 0, 350, 20, 80); //the88p0 has less
      } else if (potValue > 650) {
        servoPos = map(potValue, 650, 1024, 90, 150);
      } else {
        servoPos = pos_middle ;// 82; //86 with the 880
      }

      if (servoPos != prevServoPos) {
        softSerial.print(servoPos); softSerial.print(" ");
        softSerial.print(potValue); softSerial.print(" ");
        softSerial.println();
      }
      prevServoPos = servoPos;

      break;
    case 2:
      //to calibrate override the above
      servoPos = potValue / 4 ; //map(servoPos, 0, 1024, 0, 179);
      outPos = trimVal + servoPos;

      //if (outPos != prevServoPos) {

      //}
      prevServoPos = outPos;
      break;
  }

  softServo.write(outPos);

  //analogWrite(SERVO1PIN, servoPos); //doesnt work, servo need PPM 0.5-2ms

  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position
}





byte tongueDiff(int val, byte mid, int  ) {
  byte servoPos;
  if ( val < 350 ) {
    servoPos = map(val, 0, 350, 20, 80); //the88p0 has less
  } else if (val > 650) {
    servoPos = map(val, 1024, 650, 150, 90);
  } else {
    servoPos = pos_middle ;// 82; //86 with the 880
  }
  return servoPos;
}
