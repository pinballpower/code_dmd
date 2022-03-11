#!/usr/bin/python3

import time
import sys
from struct import unpack

from dmddisplay.console import display_image

f=open(sys.argv[1],"rb");


done=False

while not(done):

    buf=f.read(8);
    (columns, rows, _padding, bitsperpixel) = unpack(">HHHH",bytes(buf[:8]));
    length=int(columns*rows*bitsperpixel/8)
    imgdata=f.read(length)
    display_image(rows, columns, bitsperpixel, imgdata)
    time.sleep(1)

