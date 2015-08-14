/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2,3);
//   avoid using pins with LEDs attached

int pinIndex = 8;
int pinLED = 13;

void setup() {
  Serial.begin(115200);
  Serial.println("Basic Encoder Test:");
  pinMode(pinIndex, INPUT);
}


long oldPosition  = -999;



void loop() {

  //get current encoder position
  long newPosition = myEnc.read();
  
  //check if we're at the index position, if so, zero encoder counter
  //and turn on LED
  if(digitalRead(pinIndex)) {
     newPosition = 0;
     myEnc.write(newPosition);
     digitalWrite(pinLED,HIGH);
  } else {
     digitalWrite(pinLED,LOW); 
  }
  
  //if position changed since last time, write value over serial
  if(newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.println(newPosition);
  }
}
