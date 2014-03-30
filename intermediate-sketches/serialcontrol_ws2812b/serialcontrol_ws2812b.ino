/*

This example waits for serial data and sets an LED accordingly.
Data is sent in RGB order, uint8_t each, and terminated with a
newline character.

*/

#include <Adafruit_NeoPixel.h>

#define DIN_PIN           13 // data in pin on the first WS2812b
#define NUM_LEDS          1 // Number of LEDs in the strip
#define STRING_LEN_MAX    (NUM_LEDS * 3 + 10) // serial buffer: 3 bytes per pixel plus some extra

String inputString = "";
boolean stringComplete = false;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
//   Eg NEO_GRB + NEO_KHZ800

Adafruit_NeoPixel ambilightstrip = Adafruit_NeoPixel(NUM_LEDS, DIN_PIN, NEO_GRB + NEO_KHZ800);

// initialize serial and the LED strip
void setup() {
  Serial.begin(115200);
  inputString.reserve(STRING_LEN_MAX);
  ambilightstrip.begin();
  ambilightstrip.show();
}

/* Periodically parse serial data (if any) for RGB color values, and set them on the strip. */
void loop() {
  static uint8_t r = 0;
  static uint8_t g = 0;
  static uint8_t b = 0;

  // parse data
  if(stringComplete) {
    r = inputString[0];
    g = inputString[1];
    b = inputString[2];

    // set LEDs
    for(int i=0; i < ambilightstrip.numPixels(); i++) {
      ambilightstrip.setPixelColor(i,
        ambilightstrip.Color(inputString[i * 3 + 0],
                             inputString[i * 3 + 1],
                             inputString[i * 3 + 2]));
    }

    inputString = "";
    stringComplete = false;
  }


  // show and wait for next iteration
  ambilightstrip.show();
  delay(5);
}

/* UART peripheral rx callback. Buffers up the data, then sets
flag so it can be treated. Newline expected to end each transfer. */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}



