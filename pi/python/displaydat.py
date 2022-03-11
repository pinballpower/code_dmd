#!/usr/bin/python3

import time
import sys
from struct import unpack

f=open(sys.argv[1],"rb");

# Translate bits to characters for different bits/pixel
# 4 bit is really only 3 bit
symbols = {
 "wpc": [" ","*"],
 "whitestar": [" ",".","o","O","*","?","?","?","?","?","?","?","?","?","?","?"],
 "spike": [" "," ",".",".",".","o","o","o","O","O","O","*","*","*","#","#"]
}

s=symbols["spike"];

def move_cursor (y, x):
    print("\033[%d;%dH" % (y, x))

def print_image(columns, rows, bitsperpixel, data):
    move_cursor(0,0)
    pixelmask=0xff>>(8-bitsperpixel)
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
            pv = ((d >> pixelbit) & pixelmask)
            rstr+=s[pv]

        print(rstr)

done=False

while not(done):

    buf=f.read(8);
    (columns, rows, _padding, bitsperpixel) = unpack(">HHHH",bytes(buf[:8]));
    len=int(columns*rows*bitsperpixel/8)
    imgdata=f.read(len)
    print_image(rows, columns, bitsperpixel, imgdata)
    time.sleep(1)

    
GPIO.cleanup()
