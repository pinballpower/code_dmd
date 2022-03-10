#!/usr/bin/python3

import time
import spidev
import RPi.GPIO as GPIO
from struct import unpack

notify_gpio=7

GPIO.setmode(GPIO.BCM)
GPIO.setup(notify_gpio, GPIO.IN)

spi = spidev.SpiDev()
spi.open(1,0)
spi.max_speed_hz=1000000

print("SPI opened")

def print_image(columns, rows, bitsperpixel, data):
    pixelmask=0xff>>(8-bitsperpixel)
    print(pixelmask)
    pixelindex=0
    pixelbit=8
    for r in range(rows):
        rstr=""
        for c in range(columns):
            pixelbit -= bitsperpixel
            if pixelbit < 0:
                pixelbit += 8
                pixelindex += 1
            d=data[pixelindex]
            pv = ((d >> pixelbit) & pixelmask);
            if pv>2:
                rstr+="*"
            elif pv>1:
                rstr+="o"
            elif pv>0:
                rstr+="."
            else:
                rstr+=" "


        print(rstr)

done=False
todo=20;

while not(done):
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
        print("{}x{} pixel, {} bits/px".format(columns,rows,bitsperpixel))

        print_image(rows, columns, bitsperpixel, buf[8:])

        todo -= 1;

    if todo==0:
        done=True

    

    
GPIO.cleanup()
