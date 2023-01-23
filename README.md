# Read and display pinball DMD data

This project allows to read the contents of a pinball DMD display. The hardware consists of a Raspberry Pi Pico and the Raspberry Pi. 
The Pi Pico implements the data interface and can be used stand-alone. If additional functionality is needed the Raspberry Pi can be used for this

## Hardware

The Pi Pico is directly connected to the 6 DMD data lines. Communication between the Pi Pico and the Pi is implemented by SPI with an additional 
IRQ line. On this IRQ line, the Pico signals that new data is available and the Pi starts the data transfer. 
Unfortunately the Pi can't act as an SPI slave. Therefore, this method is used.

## License

The license has been changed from MIT to GPL v3 on 2.5.2022. In general, I'm not a big fan of the GPL and rather prefer the more open MIT license. However, in similar products open source software has been used to create close-source software that should replace existing open-source software. To make sure that won't happen with this software, the license of this software has been changed to GPL v3. 
Just to clarify: That doesn't mean anything about "commercial" or "non-commercial" use cases. If you want to create a commercial product based on this software, feel free to do so. If you want to sell compiled versions of this software, you can do this as long as you provide users a way to access the full source code that's needed to build their own software. This is also valid for any derivate work. 
