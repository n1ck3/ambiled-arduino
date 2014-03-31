#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <ambiledStrip.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/* 
 * LED strip update program. Accepts serial data in GRB format, with a preamble
 * sync magic string, puts this in a buffer and finally outputs to a LED strip.
 * Rinse, repeat.
 */
/*---------------------------------------------------------------------------*/
#define STRIP_PIN         13 // data in pin on the first WS2812b
#define NUM_LEDS          30 // Number of LEDs in the strip
#define RGB_BUFFER_LEN    (NUM_LEDS * 3 + 100) // serial buffer: 3 bytes per pixel plus some extra
#define SERIAL_SPEED      (115200 * 2) /* speed of serial communcation to host computer */

uint8_t rgbbuf[RGB_BUFFER_LEN]; /* serial buffer */
volatile int do_update = 0;   /* flag set from serialEvent() to indicate full buffer recevied */
ambiledStrip strip(rgbbuf, NUM_LEDS, STRIP_PIN);
/*---------------------------------------------------------------------------*/
/* initialize serial and the LED strip */
void
setup()
{
  Serial.begin(SERIAL_SPEED);
  strip.update();
}
/*---------------------------------------------------------------------------*/
/* Periodically parse serial data (if any) for RGB color values, and set them on the strip. */
void
loop()
{
  /* if we have received enough serial data to update the whole strip, do this now */
  if(do_update) {
    do_update = 0;
    strip.update();
  }
  /* here, the loop will check serial_event(), then start from the beginning */
}
/*---------------------------------------------------------------------------*/
/* Check incoming serial data for sync magic string */
int
sync_ok(char s)
{
  static int num_ok = 0;
  static char *sync_word = "START";
  static int sync_len_ok = 5; // len of "START"

  if(s == sync_word[num_ok]) {
    if(num_ok == sync_len_ok - 1) {
      num_ok = 0;
      return 1;
    } else {
      num_ok++;
      return 0;
    }
  } else {
    num_ok = 0;
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
/* UART peripheral rx callback. Buffers up the data, then sets flag so it can
  be treated. Newline expected to end each transfer. */
void serialEvent() {
  static int sync_found = 0;
  static int read_togo = NUM_LEDS * 3;

  while (Serial.available()) {
    char incoming_char;

    if(sync_found) {
      /* we have sync, read out the full buffer length */
      if(read_togo) {
        incoming_char = (char)Serial.read();
        rgbbuf[NUM_LEDS * 3 - read_togo] = incoming_char;
        read_togo--;
        if(read_togo == 0) {
          sync_found = 0;
          do_update = true;
        }
      } else {
        /* all done, set flag */
        sync_found = 0;
        do_update = true;
        // Serial.flush(); // XXX is this necessary?
      }
    } else {
      /* read a character and check for sync */
      incoming_char = (char)Serial.read();
      if(sync_ok(incoming_char)) {
        /* we got sync, start filling up the buffer with LED data */
        sync_found = 1;
        read_togo = NUM_LEDS * 3;
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
