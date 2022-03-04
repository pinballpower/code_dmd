#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/divider.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"


// SPI Defines
#define SPI0 spi0
#define SPI0_MISO 16
#define SPI0_CS   17
#define SPI0_SCK  18
#define SPI0_MOSI 19

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}


void init() {
    stdio_init_all();

    printf("DMD reader starting");

    // SPI initialisation at 1MHz
    spi_init(SPI0, 1000*1000);
    gpio_set_function(SPI0_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_CS,   GPIO_FUNC_SIO);
    gpio_set_function(SPI0_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(SPI0_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(SPI0_CS, GPIO_OUT);
    gpio_put(SPI0_CS, 1);

    printf("SPI initialized");
} 


int main()
{
    init();

    return 0;
}
