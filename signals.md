# DMD Signals and operation

## Signals

## Timing/Frequencies

|System|Pixel (column) Clock|Line clock|Screen rate|
|---|---|---|---|
|WPC|1MHz|3.9kHz|125Hz|
|WhiteStar|||

## Operation WPC

Column data is sent serialized using SDATA SCLK signals. 
It takes 128us to transfer the 128bits of a line. 
However, about 50% of the time, there is no signal/clock
active.

Lines are stepped by a RCLK signal. RDATA is low. If RDATA 
is H when the RCLK signal is received, line is being 
reset to the first line

## Operation Whitestar

## Reading data

When reading data, we assume the data is sent correctly. 
This means we can read a full 128x32 bit frame 
as follows:
- Wait for RDATA to go to L
- Wait for RDATA to go to H
- Wait for RCLK to go to L
- Signal "FRAME START"
- Read 32x128 bit
