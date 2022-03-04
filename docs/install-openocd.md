#OpenOCD

## Installation
```
sudo apt install automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev
git clone --recurse-submodules --depth 1 --branch rp2040 https://github.com/raspberrypi/openocd.git
cd openocd/
./bootstrap 
./configure --enable-ftdi --enable-sysfsgpio --enable-bcm2835gpio
make -j 4
sudo make install
```

## Configuration
sudo vi /usr/local/share/openocd/scripts/interface/raspberrypi-swd.cfg

```
# Use RPI GPIO pins
adapter driver bcm2835gpio

bcm2835gpio_speed_coeffs 146203 36

# SWD                swclk swdio
# Header pin numbers
bcm2835gpio_swd_nums 23    22

transport select swd

adapter speed 1000
```

## Usage

```
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg
```

