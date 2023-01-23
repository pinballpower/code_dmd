#!/bin/sh
openocd -f raspberrypi-swd-dmdreader.cfg -f target/rp2040.cfg -c "program dmdreader.elf verify reset exit"
