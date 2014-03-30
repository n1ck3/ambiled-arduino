/*

This example is autonomous (ie needs no computer and serial connection) and
just sets a LED to different colors back and forth.

*/

#include <Adafruit_NeoPixel.h>


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

#define PIN 13
#define NUM_PIXELS 1
Adafruit_NeoPixel ambilightstrip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  ambilightstrip.begin();
  ambilightstrip.show(); // Initialize all pixels to 'off'
}

void loop() {
  static uint8_t r,g,b;
  static int going_up = 0;

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
  //delay(5);
}


