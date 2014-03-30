/*

This sketch demonstrates how to get serial data from a PC with a serial
connection to the Arduino, and doing something with that data.

The Arduino waits until it gets serial data terminated with a newline char,
then does *sth*.

*/

String inputString = "";
boolean stringComplete = false;

void setup() {
  // initialize serial:
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(STRING_LEN_MAX);
}

void loop() {
  if(stringComplete) {
    Serial.println(inputString); 
    inputString = "";
    stringComplete = false;
    // do sth with the data here, parse or whatever  
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.
*/
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

