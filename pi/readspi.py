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

# Translate bits to characters for different bits/pixel
# 4 bit is really only 3 bit
symbols = {
 1: [" ","*"],
 2: [" ",".","o","O" ],
 3: [" "," ",".",".","o","o","O","O"],
 4: [" "," ",".",".","o","o","O","O","O","O","O","O","O","O","O","O"],
}

def move_cursor (y, x):
    print("\033[%d;%dH" % (y, x))

def print_image(columns, rows, bitsperpixel, data):
    move_cursor(0,0)
    pixelmask=0xff>>(8-bitsperpixel)
    pixelindex=0
    pixelbit=8
    s=symbols[bitsperpixel]
    for r in range(rows):
        rstr=""
        for c in range(columns):
            pixelbit -= bitsperpixel
            if pixelbit < 0:
                pixelbit += 8
                pixelindex += 1
            d=data[pixelindex]
            pv = ((d >> pixelbit) & pixelmask)
            #print(pv)
            rstr+=s[pv]

        print(rstr)

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
        # print("{}x{} pixel, {} bits/px".format(columns,rows,bitsperpixel))

        print_image(rows, columns, bitsperpixel, buf[8:])

    
GPIO.cleanup()
