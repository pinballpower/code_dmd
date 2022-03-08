#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/pio.h"

#include "spi_slave_sender.pio.h"

// SPI data types and header blocks
// header block length should always be a multiple of 32bit
#define SPI_BLOCK_PIX 1

typedef struct __attribute__((__packed__)) block_header_t {
    uint16_t block_type;   // block type
    uint16_t len;         // length of the whole data including header in bytes
} block_header_t;

typedef struct __attribute__((__packed__)) block_pix_header_t {
    uint16_t columns;     // number of columns
    uint16_t rows;        // number of rows
    uint16_t bitsperpixel; // bits per pixel
    uint16_t padding;   
} block_pix_header_t;

// SPI Defines
#define SPI0 spi0
#define SPI_BASE  16
#define SPI0_MISO SPI_BASE
#define SPI0_CS   (SPI_BASE+1)
#define SPI0_SCK  (SPI_BASE+2)
#define SPI0_MOSI (SPI_BASE+3)

// data buffer
#define MAX_WIDTH 192
#define MAX_HEIGHT 64
#define MAX_BITSPERPIXEL 4

uint8_t lcd_width;
uint8_t lcd_height;
uint8_t lcd_bitsperpixel;
uint8_t lcd_pixelsperbyte;
uint16_t lcd_bytes;

uint8_t pixbuf1[MAX_WIDTH*MAX_HEIGHT*MAX_BITSPERPIXEL/8];
uint8_t pixbuf2[MAX_WIDTH*MAX_HEIGHT*MAX_BITSPERPIXEL/8];

// SPI PIO
PIO spi_pio;
uint spi_sm;


void demo_image() {
    for (uint16_t i=0; i<lcd_bytes; i++) {
        pixbuf1[i]=i & 0xff;
        pixbuf2[i]=i & 0xff;
    } 
} 


// Send picture to serial port, only use most significant bit
void serial_send_pix(uint8_t *buf) { 
    char linestr[MAX_WIDTH] = {0};
    uint16_t i=0;
    uint8_t bitoffset=0;
    uint8_t pixel=0;

    for (uint8_t line=0; line<lcd_height; line++) {
        for (uint8_t column=0; column<lcd_width; column++){
            if (bitoffset>=8) {
                buf++;
                bitoffset=0;
            } 

            // get the MSB from the current pixel
            pixel=(*buf >> bitoffset >> (lcd_bitsperpixel-1)) & 1;

            bitoffset += lcd_bitsperpixel;

            if (pixel) {
                linestr[column]='*';
            }  else {
                linestr[column]=' ';
            } 
        } 
        printf("%s\n",linestr);
    } 
}

/**
 * @brief Send data via SPI
 * 
 * @param buf a byte buffer
 * @param len 
 */
void spi_send(uint32_t *buf, uint16_t len) {
    for (uint16_t i=0; i<len; i+= 4) {
        pio_sm_put_blocking(spi_pio, spi_sm, *buf);
        buf+=4;
    }
}

void start_data() {
    gpio_put(17, 1);
}

void end_data() {
    gpio_put(17, 0);
}

// send the picture buffer via SPI
void spi_send_pix(uint8_t *pixbuf) {


    block_header_t h = {
        .block_type = SPI_BLOCK_PIX
    };
    block_pix_header_t ph = {};

    // round length to 4-byte blocks 
    h.len=(((lcd_bytes+3)/4)*4)+sizeof(h)+sizeof(ph);
    ph.columns=lcd_width;
    ph.rows=lcd_height;
    ph.bitsperpixel=lcd_bitsperpixel;

    //uint32_t x=0xffff0004;
    //spi_send(&x,1);
    spi_send((uint32_t*)&h,sizeof(h));
    start_data();
    spi_send((uint32_t*)&ph,sizeof(ph));
    end_data();
} 

void init() {
    stdio_init_all();

    printf("DMD reader starting");

    // this is uses to notify the Pi that data is available
    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_put(17, 0);

    spi_pio = pio0;
    uint offset = pio_add_program(spi_pio, &clocked_output_program);
    spi_sm = pio_claim_unused_sm(spi_pio, true);
    clocked_output_program_init(spi_pio, spi_sm, offset, SPI_BASE);

    printf("SPI slave initialized");

    lcd_width = 128;
    lcd_height = 32;
    lcd_bitsperpixel = 1;
    lcd_pixelsperbyte = 8/lcd_bitsperpixel;
    lcd_bytes= lcd_width*lcd_height*lcd_bitsperpixel/8;

    printf("LCD buffer initialized");
} 


int main()
{
    init();

    demo_image();
    // serial_send_pix(pixbuf1);
    spi_send_pix(pixbuf1);

    return 0;
}
