#!/usr/bin/python3

import time
import spidev
import RPi.GPIO as GPIO
from struct import unpack

from dmddisplay.console import display_image

notify_gpio=7

GPIO.setmode(GPIO.BCM)
GPIO.setup(notify_gpio, GPIO.IN)

spi = spidev.SpiDev()
spi.open(1,0)
spi.max_speed_hz=5000000

print("SPI opened")

done=False

while not(done):
    if not(GPIO.input(notify_gpio)):
        GPIO.wait_for_edge(notify_gpio, GPIO.RISING)
    print("received notify")

    header=bytes(spi.readbytes(4))
    print(header);

    (length,packettype) = unpack(">HH",header);

    if packettype==0:
        continue

    if length<=4:
        continue

    print(packettype, length)

    buf=spi.readbytes(length-4)

    if (packettype==1):
        print(buf[:8])
        (columns, rows, _padding, bitsperpixel) = unpack(">HHHH",bytes(buf[:8]));
        
        display_image(rows, columns, bitsperpixel, buf[8:])

    
GPIO.cleanup()
