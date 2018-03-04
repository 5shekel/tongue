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

int pos_middle = 80;
int servoPos;
int potValue, minval, maxval, tmpval;
  
float x_1,x_2, y;


void setup() {

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("reboot");
/*
  EEPROM.get(0, runningDelta);
  softSerial.println(runningDelta);
  EEPROM.get(2, gestrue01 );
  softSerial.println(gestrue01);
*/

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {

  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D:
        softSerial.println("CH-");
		
	    maxval = servoPos;
        printdebug();
        break;
      case 0xFFE21D:
        softSerial.println("CH+");
		
	    minval = servoPos;
        printdebug();
        break;
      case 0xFF629D: 
        softSerial.println("CH");
		
	    pos_middle = servoPos;
        printdebug();

      case 0xFF906F:
        softSerial.println("[EQ] - value dump 0xFF906F");
        printdebug();

      case 0xFFFFFFFF:
        break;
      default:
        softSerial.print(F("unknown = 0x"));
        softSerial.println(results.value, HEX);
    }
    irrecv.resume();
  }

  potValue = analogRead(POTPIN);              // Read voltage on potentiometer
 
  //smoothing
  y = 0.10*potValue +0.75*y;
  servoPos = (y+pos_middle)/4  ;
  
  tmpval = mapped(servoPos);

  if (millis() - prevValue >= 15) {
    prevValue = millis();
    softServo.write(servoPos);
    SoftwareServo::refresh();
	//printdebug();
  }

}


int mapped(int input){
	int output;
    if ( (input >= (pos_middle - 5) ) && (input <= (pos_middle + 5)) ){
		output = pos_middle ;
	} else if (input < (pos_middle - 5)){
		output = map(input, 0, maxval,pos_middle, 179   );
	}else if(input > (pos_middle + 5)){
		output = map(input, 0, minval,pos_middle, 0   );
	}
	    return output;
  }
  

void printdebug(){
	softSerial.println("minval pos_middle	maxval");
	softSerial.print(minval);
	softSerial.print("	");
	softSerial.print(pos_middle);
	softSerial.print("	");
	softSerial.print(maxval);
    softSerial.println();
	
	softSerial.println("tmpval	servoPos	potValue");
	softSerial.print(tmpval);
	softSerial.print("	");
	softSerial.print(servoPos);
	softSerial.print("	");
	softSerial.print(potValue);
    softSerial.println();
}
