/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/tongue

   pin out attiny85
   RST    -1+----+8-   VCC
   3/LDR  -2|*   |7-   2/PWM
   4/IR   -3|    |6-   1/SERIAL
   GND    -4+----+5-   0/SERVO

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

int midval = 90;
int minval = 50;
int maxval = 400; 
int trim = 0;
int servoPos;
int potValue, tmpval;
  
float x_1, y;
int graph_state = 0;

void setup() {
  //pinMode(2, OUTPUT);
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
  softServo.setMaximumPulse(2000);

  delay(15);
}

void loop()  {

  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D:
        softSerial.println("CH-");
	      minval = servoPos;
        
        printdebug();
        break;

      case 0xFF629D: 
        softSerial.println("CH");
	      midval = servoPos;
        printdebug();
        break;
        
      case 0xFFE21D:
        softSerial.println("CH+");
        maxval = servoPos;
        printdebug();
        break;

      case 0xFFE01F:
        softSerial.println("[-]");
        trim++;
        break;

      case 0xFFA857:
        softSerial.println("[+]");
        trim--;
        break;

      case 0xFF52AD:
        softSerial.println("[9]");
        if (graph_state)
          graph_state=0;
        else 
          graph_state=1;    
          
        break;

      case 0xFF906F:
        softSerial.println();
        softSerial.println("[EQ] - value dump 0xFF906F");
        printdebug();
        break;

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
  servoPos = int(y)  ;
  
  tmpval = int(mapped(servoPos));
  tmpval = constrain(tmpval, 0, 179);

  if (millis() - prevValue >= 15) {
    prevValue = millis();
    softServo.write(tmpval);

    //analogWrite(2, servoPos);
    SoftwareServo::refresh();
	  //printdebug();
    printGraph();
  }

}


int mapped(int input){
	int output;
  if ( (input >= (midval - 20) ) && (input <= (midval + 20)) ){
		output = 90-trim ;
	  } else if (input < (midval - 20)){
		output = map(input, minval, midval, 0, 90-trim   );
	  }else if(input > (midval + 20)){
		output = map(input, midval, maxval, 90-trim  , 179 );
	}
	return output;
}
void printdebug(){};

void printGraph(){};
  /*
void printGraph(){
  if(graph_state){
    softSerial.print(tmpval);
	  softSerial.print("	");
	  softSerial.print(servoPos);
	  softSerial.print("	");
	  softSerial.print(potValue);
    softSerial.println();
  }
}

void printdebug(){
	//softSerial.println("minval midval	maxval");
	softSerial.print(minval);
	softSerial.print("	");
	softSerial.print(midval);
	softSerial.print("	");
	softSerial.print(maxval);
  softSerial.println();
	
	//softSerial.println("tmpval	servoPos	potValue");
	softSerial.print(tmpval);
	softSerial.print("	");
	softSerial.print(servoPos);
	softSerial.print("	");
	softSerial.print(potValue);
  softSerial.println();

  //softSerial.println("trim	xxx	xxx");
	softSerial.print(trim);
	softSerial.print("	");
	softSerial.print("xxx");
	softSerial.print("	");
	softSerial.print("xxx");
  softSerial.println();
}
*/
