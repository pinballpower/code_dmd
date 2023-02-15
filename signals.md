# DMD Signals and operation

## Signals

|Signal|Name|Usage|
|---|---|---|
|DE|display enable|Turn on/off display|
|RDATA|row data|set first row|
|RCLK|row clock|next row|
|COLLAT|cloumn latch|activate transmitted data for current column|
|DOTCLK|dot clock|next column|
|SDATA|dot data|pixel data|

## Timing/Frequencies

|System|Pixel (column) Clock|Line clock|Screen rate|
|---|---|---|---|
|WPC|1MHz|3.9kHz|125Hz|
|WhiteStar|1MHz/500kHz|2.577kHz|78Hz|
|SAM|4.7MHz|2kHz|62.5Hz|
|Spike 1|4.8MHz|3.8kHz|63.5Hz|

## Operation WPC

Column data is sent serialized using SDATA SCLK signals. 
It takes 128us to transfer the 128bits of a line. 
However, about 50% of the time, there is no signal/clock
active.

Lines are stepped by a RCLK signal. RDATA is low. If RDATA 
is H when the RCLK signal is received, line is being 
reset to the first line

## Operation Whitestar

WhiteStar works differently than WPC. In Whitestar, each line 
is sent twice first with 500kHz pixel clock, then again with 
1MHz pixel clock. While this might seem strange, this allows
to use 2 bit brightness data with only 2 cycles.

## Operation Spike 1

Spike 1 supports 16 levels of brightness. They are displayed in 4 planes each 
using different timing:
plane 1 (least significant): 1.05ms
plane 2: 2.1ms
plane 3: 4.2ms
plane 4: 8.4ms
A full screen consist of these 4 planes and therefore is displayed for 15.75ms

# Reading data

When reading data, we assume the data is sent correctly. 
This means we can read a full 128x32 bit frame 
as follows:
 - Wait for frame start - this is different on different platforms
 - Read 32x128 bit

