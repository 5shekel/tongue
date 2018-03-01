/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/xxx

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


#define DEBUG 1

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
long lastPressTime = 0;

void setup() {
  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("hell");

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}



void loop()  {

  if (irrecv.decode(&results)) {

    softSerial.print(F("result = 0x"));
    softSerial.println(results.value, HEX);

    if (results.value == 0xFFA25D)  {
      softSerial.println("yes");
    } else if (results.value ==  0xFFFFFFFF) {
      softSerial.println("yess");

    }
    else {
      softSerial.println("No");
    }
    /*
      if (1) {
      if (millis() - lastPressTime > 150) {
        softSerial.println(results.value, HEX);
      }
      lastPressTime = millis();
      }
    */
    irrecv.resume(); // Receive the next value
  }

  int servoPos;
  int potValue = analogRead(POTPIN);              // Read voltage on potentiometer

  // map the range of pot to limited map range on the servo.
  // a narrow band of the full range.
  // in both directions, with a stop space in middle

  if ( potValue < 350 ) {
    servoPos = map(potValue, 0, 350, 20, 80); //the88p0 has less
  } else if (potValue > 650) {
    servoPos = map(potValue, 1024, 650, 150, 90);
  } else {
    servoPos = 82; //86 with the 880
  }


  //to calibrate override the above
  //servoPos = map(potValue, 0, 1024, 0, 179);
  if (servoPos != prevServoPos) {
    softSerial.print(servoPos);
    softSerial.print(" ");
    softSerial.println(potValue);
    softServo.write(servoPos);

  }
  prevServoPos = servoPos;


  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position


}
