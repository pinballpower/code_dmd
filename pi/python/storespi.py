#!/usr/bin/python3

import spidev
import RPi.GPIO as GPIO
import time
from struct import unpack
import hashlib

notify_gpio=7

GPIO.setmode(GPIO.BCM)
GPIO.setup(notify_gpio, GPIO.IN)

spi = spidev.SpiDev()
spi.open(1,0)
spi.max_speed_hz=1000000

print("SPI opened")

# store the hashes of frames that we have seen already
seen = {}


outputfile=open("spiimage.dat","wb")

done=False

while not(done):
    if not(GPIO.input(notify_gpio)):
        GPIO.wait_for_edge(notify_gpio, GPIO.RISING)
    header=bytes(spi.readbytes(4))

    (length,packettype) = unpack(">HH",header);

    if packettype==0:
        continue

    if length<=4:
        continue

    buf=spi.readbytes(length-4)
    
    if (packettype==1):
        data=bytes(buf)
        hash=hashlib.md5(data)
        d=hash.digest()
        if not(d in seen.keys()):
            seen[d]=1
            print(d)
            outputfile.write(data)

    time.sleep(0.01)


GPIO.cleanup()
