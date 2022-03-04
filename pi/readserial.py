#!/usr/bin/python

import serial, string, time

output = " "
open = False
while not(open):
  try:
    ser = serial.Serial('/dev/ttyACM0', 19200, 8, 'N', 1, timeout=1)
    open=True
  except:
    open=False
    time.sleep(1) 
  print("*")

print("TTY opened")

while True:
  output = ser.readline()
  if output:  
    print output

