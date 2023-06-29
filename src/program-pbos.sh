#!/bin/bash
if [ "$1" == "" ]; then
	echo "start with $0 hostname to program RPO2040 on PBOS"
	exit 1
fi

sshpass -p pbos scp dmdreader.elf root@$1:/firmware 
sshpass -p pbos ssh -l root $1 'ps -ef | grep dmdreader | grep -v grep | awk '{print $1}' | xargs kill -KILL'
sshpass -p pbos ssh -l root $1 'cd /firmware; openocd -f raspberrypi-swd-dmdreader.cfg -f target/rp2040.cfg -c "program dmdreader.elf verify reset exit"'
sshpass -p pbos ssh -l root $1 'reboot'
