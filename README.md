# Read and display pinball DMD

This project allows to read the contents of a pinball DMD display. The hardware consists of a Raspberry Pi Pico and the Raspberry Pi. 
The Pi Pico implements the data interface and can be used stand-alone. If additional functionality is needed the Raspberry Pi can be used for this

## Hardware

The Pi Pico is directly connected to the 6 DMD data lines. Communication between the Pi Pico and the Pi is implemented by SPI with an additional 
IRQ line. On this IRQ line, the Pico signals that new data is available and the Pi starts the data transfer. 
Unfortunately the Pi can't act as an SPI slave. Therefore, this method is used.
