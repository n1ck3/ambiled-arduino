/*
 * Copyright (c) 2013, Marcus Linderoth, <linderoth.marcus@gmail.com>
 *
 * This is based on the Adafruit WS2812-library, but rewritten to remove
 * a number of less useful functions, but most importantly, does not rely
 * on having a separate, private, LED data buffer. The reason for this is
 * that if you maintain one such yourself, separate from this library, 
 * the buffers become redundant.
 *
 * A clear example of this is receiving serial data to control LEDs, then
 * you have a buffer for receiving the data. For 300 LEDs, it becomes
 * 3*300 = 900 bytes redundant RAM statically allocated.
 */
/*--------------------------------------------------------------------
  This file is part of the Adafruit NeoPixel library.

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/
#ifndef AMBILEDSTRIP_H
#define AMBILEDSTRIP_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include "ambiledStrip.h"

class ambiledStrip
{
  public:
    /* constructor for an ambiledStrip */
    ambiledStrip(uint8_t *buffer, uint16_t numpixels, uint8_t pin);
    /* update the LEDs based on already registrered buffer */
    void update();
    /* update the LEDs from any buffer */
    void update_from_buffer(uint8_t *buffer, uint16_t numpixels);
    /* update the LED in the buffer */
    void set_pixel_rgb(uint16_t pixelnum, uint8_t r, uint8_t g, uint8_t b);
    /* get number of pixels in the strip */
    uint16_t no_of_pixels();

  private:
    uint8_t _pin;
    uint8_t *_rgbbuf;
    uint16_t _numpixels;  
    uint32_t endTime; // Latch timing reference
#ifdef __AVR__
    volatile uint8_t *port; // Output PORT register
    uint8_t pinMask; // Output PORT bitmask
#endif
};
/*---------------------------------------------------------------------------*/
/* intended for future use */
#if 0
struct ws2812b_strip_s {
  uint8_t pin;
  uint8_t *rgbbuf;
  uint16_t numpixels;  
}
typedef struct ws2812b_strip_s ws2812b_strip_t;

struct rgb_s {
  uint8_t r;
  uint8_t g;
  uint8_t b;
}
typedef struct rgb_s rgb_t;
#endif  /* if 0; commented out code */
#endif  /* AMBILEDSTRIP_H */
