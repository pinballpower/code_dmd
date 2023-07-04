#!/bin/bash
if [ "$1" == "" ]; then
	echo "start with $0 hostname to program RPO2040 on PBOS"
	exit 1
fi

echo "Stopping DMDReader..."
sshpass -p pbos scp dmdreader.elf root@$1:/firmware 
sshpass -p pbos ssh -l root $1 'pkill dmdreader'
sleep 5
sshpass -p pbos ssh -l root $1 'pkill -KILL dmdreader'
echo "Programming RP2040..."
sshpass -p pbos ssh -l root $1 'cd /firmware; openocd -f raspberrypi-swd-dmdreader.cfg -f target/rp2040.cfg -c "program dmdreader.elf verify reset exit"'
echo "Rebooting..."
sshpass -p pbos ssh -l root $1 'reboot'
