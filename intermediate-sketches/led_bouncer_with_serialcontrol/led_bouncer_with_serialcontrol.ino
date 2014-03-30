/*

This example fades a LED up and down, but having a serial connection
and pressing 'u' or 'd' will disrupt the fading.

*/

#include <Adafruit_NeoPixel.h>

String inputString = "";
boolean stringComplete = false;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

#define PIN 13
#define NUM_LEDS       1
#define STRING_LEN_MAX  (NUM_LEDS * 3 + 10) // 3 bytes per pixel plus some extra

Adafruit_NeoPixel ambilightstrip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // initialize serial:
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(STRING_LEN_MAX);
  ambilightstrip.begin();
  ambilightstrip.show(); // Initialize all pixels to 'off'
}

// control the LED over serial port: type 'u' or 'd' to go up/down.
// The LED does this automatically but the loop can be disrupted this way.
void loop() {
  static uint8_t r,g,b;
  static int going_up = 0;
  if(stringComplete) {
    // do something with the serial data, i.e. parse it to get the RGB values for the LED strips
    Serial.println(inputString);
    if(inputString[0] == 'u') {
      going_up = 1;
    } else if(inputString[0] == 'd') {
      going_up = 0;
    }

    inputString = "";
    stringComplete = false;
  }

  for(int i=0; i< ambilightstrip.numPixels(); i++) {
    ambilightstrip.setPixelColor(i, ambilightstrip.Color(r, g, b));
  }

  if(going_up) {
    r++;
    g++;
    b++;
    if(r == 0xff) {
      going_up = 0;
    }
  } else {
    r--;
    g--;
    b--;
    if(r == 0) {
      going_up = 1;
    }
  }

  ambilightstrip.show();
  delay(5);
}



/*  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  */
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



