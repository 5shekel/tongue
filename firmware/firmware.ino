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

<<<<<<< HEAD
int pos_middle = 0;
int runningDelta = 0; //soft calbrate cneter
int servoPos, potValue, gestrue01;
=======
int pos_middle = 80;
int servoPos;
int potValue; 
  
float x_1,x_2, y;
>>>>>>> refs/remotes/origin/master

void setup() {

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("reboot");

  EEPROM.get(0, runningDelta );
  softSerial.println(runningDelta);

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {
  potValue = analogRead(POTPIN) / 4;

  if (irrecv.decode(&results)) {
    switch (results.value) {
<<<<<<< HEAD
      case 0xFF30CF:

        gestrue01 = potValue;
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
=======
      case 0xFFA25D:
        softSerial.println("CH-");
        pos_middle-=1;
        printdebug();
        break;
      case 0xFFE21D:
        softSerial.println("CH+");
        pos_middle+=1;
        printdebug();
        break;
      case 0xFF629D: 
        softSerial.println("CH");
        printdebug();
>>>>>>> refs/remotes/origin/master
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

<<<<<<< HEAD
  servoPos =  potValue - runningDelta;

  softServo.write(servoPos);

  //analogWrite(SERVO1PIN, potValue); //doesnt work, servo need PPM 0.5-2ms

  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position
}




void     printDebug() {
  softSerial.println("potValue gestrue01 runningDelta servoPos");
  softSerial.print(potValue); softSerial.print("    ");
  softSerial.print(gestrue01); softSerial.print("    ");


  softSerial.print(runningDelta); softSerial.print("      ");
  softSerial.print(servoPos); softSerial.print("      ");
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
=======
  potValue = analogRead(POTPIN);              // Read voltage on potentiometer
 
  //smoothing
  y = 0.10*potValue +0.75*y;
  servoPos = (y+pos_middle)/4 ;;

  if ( (servoPos > 70) && (servoPos < 85)) {
    servoPos = pos_middle ;// 82; //86 with the 880
  }
  
  if (millis() - prevValue >= 15) {
    prevValue = millis();
    softServo.write(servoPos);
    SoftwareServo::refresh();
	//printdebug();
  }

}

void printdebug(){
	//softSerial.println("pos_middle	servoPos	potValue");
	
	softSerial.print(pos_middle);
	softSerial.print("	");
	softSerial.print(servoPos);
	softSerial.print("	");
	softSerial.print(potValue);
    softSerial.println();
}


/* // map the range of pot to limited map range on the servo.
  // a narrow band of the full range.
  // in both directions, with a stop space in middle

  if ( potValue < 350 ) {
    servoPos = map(potValue, 0, 350, 20, 80); //the88p0 has less
  } else if (potValue > 650) {
    servoPos = map(potValue, 1024, 650, 150, 90);
  } else {
    servoPos = pos_middle ;// 82; //86 with the 880
  }
*/
>>>>>>> refs/remotes/origin/master
