#!/usr/bin/python3

import time
import sys
from struct import unpack

from dmddisplay.raylib_display import RaylibDisplay

f=open(sys.argv[1],"rb");

d=RaylibDisplay(128*13+3,32*13+3, 10, 3, 16)
d.start()

done=False
c=0
startt=time.time()
while not(done):

    buf=f.read(8);
    if len(buf)<8:
        done=True
        break
        
    (columns, rows, _padding, bitsperpixel) = unpack(">HHHH",bytes(buf[:8]));
    length=int(columns*rows*bitsperpixel/8)
    imgdata=f.read(length)
    d.display_image(rows, columns, bitsperpixel, imgdata)
    c +=1
    time.sleep(0.1)
    
d.end()

print("Rendered {} frames in {} seconds".format(c,time.time()-startt))

