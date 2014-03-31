#!/usr/bin/python
# ------------------------------------------------------------------------------
# Write to an Arduino to set RGB LEDs, begin with "START" magic string, write in 
# GRB-order, then terminate with a newline.
#
# Requires PySerial that is installed through
#   sudo apt-get install pyserial
# on OSX, visit their web, download the zip, unpack, then
#   sudo python setup.py install
#
# Todo:
#   make more robust
#   some kind of auto-negotiation between device and this script about number of LEDs
#   handle locking issues; if the device is unable to be opened, clear it and open
#   autodetect depending on OS (tty.usb*, ttyUSB* etc)
#
# ------------------------------------------------------------------------------
import os
import sys
import serial # serial communications
import glob # for finding the serial device
from time import sleep
import signal     # interrupt handler to close serial connection on close
import colorsys   # HSV to RGB conversion

# serial port speed, must be equal to that running on the Arduino
SPEED = 115200
# run what program? see below for valid numbers
PROGRAM = 6
# number of LEDs in the strip
num_leds = 300
# inter-step sleep, in seconds
SLEEP = 0.05

# reference: http://eli.thegreenplace.net/2009/08/20/frames-and-protocols-for-the-serial-port-in-python/

serialdevice = None
#-------------------------------------------------------------------------------
def die(reason = "", code = 0, show_usage = False):
  print(reason)
  if show_usage:
    print("LED WS2812B-strip show. Runs some colors over the LEDs.")
    print("Usage:")
    print("%s SERIALPORT" % (sys.argv[0]))
    print("  opens up SERIALPORT and runs the LED show over that.")
    print("%s" % (sys.argv[0]))
    print("  autodetects serialport, then as above.")
    print("Examples:")
    print("%s /dev/tty.usbmodem1441" % (sys.argv[0]))
    print("%s /dev/ttyS29" % (sys.argv[0]))
    print("Works only on OSX (? not tested). Requires PySerial (http://pyserial.sourceforge.net/).")
  sys.exit(code)
#-------------------------------------------------------------------------------
# callback for Ctrl+C and 'kill PID' - shutdown gracefully
def signal_handler(signal, frame):
  global serialdevice
  if serialdevice:
    serialdevice.close()
  die('Exiting, closing serial port.')
# ------------------------------------------------------------------------------
# open a serial device, and periodically output RGB LED data to it
def main():
  global serialdevice

  # register an interrupt callback that closes the serial port connection
  signal.signal(signal.SIGINT, signal_handler)
  signal.signal(signal.SIGTERM, signal_handler)

  # get serial port mount location
  port = None
  if len(sys.argv) < 2:
    serialports = glob.glob('/dev/tty.usb*')
    if len(serialports) > 0:
      port = serialports[0]
      print('Autodetected %s, opening that.' % (port))
    else:
      die('You have not specified the proper port and none could be found. Quitting.', -1, True)
  else:
    port = sys.argv[1]

  # open the serial port
  print('opening serial port:' + port)
  print('quit by Ctrl+C')
  try:
    serialdevice = serial.Serial(port, SPEED, timeout=0)
  except Exception as e:
    print(e)
    die('Opening serial port %s failed, quitting.' %(port), -2)

  # some modes: this uses a HSV to RGB conversion
  if PROGRAM == 1:
    print('Running HSV demo over %d LED(-s).' % (num_leds))
    while True:
      h = 0.0
      brightness = 1.0
      led = 1
      while(h < 1.0):
        # create the RGB color to show from HSV
        rgb = colorsys.hsv_to_rgb(h, 1.0, 1.0) # returns float[3]
        rgb = [x*brightness for x in rgb]

        # change color hue slightly
        h += 0.01

        led += 1
        if led >= num_leds:
          led = 1

        # create the data buffer to send: GRB as one uint8_t each
        sync_string = "START"
        syncframe = [ord(c) for c in sync_string]
        # rgbframe = [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)]
        # rgbframe = [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)] + [0, 0, 0]
        # rgbframe = (led - 1) * [0,0,0] + [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)] + (num_leds - led) * [0, 0, 0]
        rgbframe = (5) * [0,0,0] + [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)] * 10 + (num_leds - 5 - 10) * [0, 0, 0]
        rgblist = syncframe + rgbframe

        st = ''.join(chr(b) for b in rgblist)
        # print
        # print(len(st))

        # send over serial to the connected device
        # start = time.clock() * 1000
        try:
          serialdevice.write(st)
        except Exception as e:
          serialdevice.close()
          print(e)
          die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')
        # read data from the Arduino
        # data = serialdevice.read(serialdevice.inWaiting())
        # if len(data) > 0:
        #   print 'arduino: ', data
        # end = time.clock() * 1000
        # print('time: ' + str(end-start))

        # sleep for a while
        sleep(SLEEP)

  # some modes: this uses a HSV to RGB conversion
  elif PROGRAM == 2:
    print('Running heartbeat demo.')

    while True:
      hue = 0.5
      brightness = 1.0
      # create the RGB color to show from HSV
      rgb = colorsys.hsv_to_rgb(hue, 1.0, 1.0) # returns float[3]
      rgb = [x*brightness for x in rgb]

      # create the data buffer to send: GRB as one uint8_t each
      sync_string = "START"
      rgblist = [ord(c) for c in sync_string] + [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)]
      st = ''.join(chr(b) for b in rgblist)
      try:
        serialdevice.write(st)
      except Exception as e:
        serialdevice.close()
        print(e)
        die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')
      sleep(SLEEP)
        
  # this just sends RGB in various permutations.
  elif PROGRAM == 3:
    r = 0;
    g = 100;
    b = 200;
    while True:
      r += 1;
      g -= 1;
      b += 1;
      if r > 255:
        r = 0
      if g > 255:
        g = 0
      if b > 255:
        b = 0
      if r < 0:
        r = 255
      if g < 0:
        g = 255
      if b < 0:
        b = 255

      # create the data buffer, send, sleep
      sync_string = "START"
      rgblist = [ord(c) for c in sync_string] + [g, r, b]
      st = ''.join(chr(b) for b in rgblist)
      # send over serial to the connected device
      try:
        serialdevice.write(st)
      except Exception as e:
        serialdevice.close()
        print(e)
        die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')
      sleep(SLEEP)


  elif PROGRAM == 4:
      SLEEP = 1
      sync_string = "START"
      while True:
        try:
          r, g, b = (255, 0, 0)
          rgblist = [ord(c) for c in sync_string] + [g, r, b] * num_leds
          st = ''.join(chr(b) for b in rgblist)
          sleep(SLEEP)
          serialdevice.write(st)
          print("setting red")

          r, g, b = (0, 255, 0)
          rgblist = [ord(c) for c in sync_string] + [g, r, b] * num_leds
          st = ''.join(chr(b) for b in rgblist)
          sleep(SLEEP)
          serialdevice.write(st)
          print("setting green")

          r, g, b = (0, 0, 255)
          rgblist = [ord(c) for c in sync_string] + [g, r, b] * num_leds
          st = ''.join(chr(b) for b in rgblist)
          sleep(SLEEP)
          serialdevice.write(st)
          print("setting blue")
        except Exception as e:
          serialdevice.close()
          print(e)
          die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')

  elif PROGRAM == 5:
      SLEEP = 0.05
      sync_string = "START"
      pos = 1

      while True:
        pos += 1
        if pos == num_leds:
          pos = 1
        r, g, b = (255, 0, 0)
        rgblist = [ord(c) for c in sync_string]
        rgblist += [0, 0, 0] * (pos)
        rgblist += [g, r, b]
        rgblist += [0, 0, 0] * (num_leds - pos)
        st = ''.join(chr(b) for b in rgblist)

        try:
          sleep(SLEEP)
          serialdevice.write(st)
        except Exception as e:
          serialdevice.close()
          print(e)
          die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')

  elif PROGRAM == 6:
    SLEEP = 0.05
    sync_string = "START"
    pos = 99
    while True:
      h = 0.0
      brightness = 1.0
      led = 1
      while(h < 1.0):
        rgb = colorsys.hsv_to_rgb(h, 1.0, 1.0) # returns float[3]
        rgb = [x*brightness for x in rgb]
        h += 0.01
        if h >= 1.0:
          h = 0.0

        rgblist = [ord(c) for c in sync_string]
        rgblist += [0, 0, 0] * (pos)
        rgblist += [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)]
        rgblist += [0, 0, 0] * (num_leds - pos)
        st = ''.join(chr(b) for b in rgblist)
        try:
          sleep(SLEEP)
          serialdevice.write(st)
        except Exception as e:
          serialdevice.close()
          print(e)
          die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')
          
"""
  elif PROGRAM == 7:
    SLEEP = 1.0/25
    sync_string = "START"
    pos = 1
    num_leds = 100 # temp as broke stripper
    brightness = 1.0

    while True:
      pos += 1
      if pos == num_leds:
        pos = 1
      h = 0.0
      while(h < 1.0):
        rgb = colorsys.hsv_to_rgb(h, 1.0, 1.0) # returns float[3]
        rgb = [x*brightness for x in rgb]
        h += 0.01
        if h >= 1.0:
          h = 0.0

        rgblist = [ord(c) for c in sync_string]
        rgblist += [0, 0, 0] * (pos)
        rgblist += [int(rgb[1] * 255), int(rgb[0] * 255), int(rgb[2] * 255)]
        rgblist += [0, 0, 0] * (num_leds - pos)
        st = ''.join(chr(b) for b in rgblist)
        try:
          sleep(SLEEP)
          serialdevice.write(st)
        except Exception as e:
          serialdevice.close()
          print(e)
          die('Unexpected serial communication malfunction (did you unplug it?). Quitting.')
"""

# ------------------------------------------------------------------------------
if __name__=="__main__":
    main()
# ------------------------------------------------------------------------------
