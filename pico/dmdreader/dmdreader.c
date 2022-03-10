#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "spi_slave_sender.pio.h"
#include "dmd_counter.pio.h"
#include "dmd_interface_wpc.pio.h"

/**
 * Glossary
 * 
 * Plane
 *  image with one bit data per pixel. This doesn't NOT mean it is stored with 1bit/pixel
 * 
 * Frame
 *  image with potentially more than one bit per pixel 
 * 
 */

#define SENDFRAMES_SPI 1

typedef struct buf32_t 
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} buf32_t;

// SPI data types and header blocks
// header block length should always be a multiple of 32bit
#define SPI_BLOCK_PIX 1

typedef struct __attribute__((__packed__)) block_header_t
{
    uint16_t block_type; // block type
    uint16_t len;        // length of the whole data including header in bytes
} block_header_t;

typedef struct __attribute__((__packed__)) block_pix_header_t
{
    uint16_t columns;      // number of columns
    uint16_t rows;         // number of rows
    uint16_t bitsperpixel; // bits per pixel
    uint16_t padding;
} block_pix_header_t;

// SPI Defines
#define SPI0 spi0
#define SPI_BASE 16
#define SPI0_MISO SPI_BASE
#define SPI0_CS (SPI_BASE + 1)
#define SPI0_SCK (SPI_BASE + 2)
#define SPI0_MOSI (SPI_BASE + 3)

// DMD types
#define DMD_UNKNOWN 0
#define DMD_WPC 1
#define DMD_WHITESTAR 2

// data buffer
#define MAX_WIDTH 192
#define MAX_HEIGHT 64
#define MAX_BITSPERPIXEL 4
#define MAX_PLANESPERFRAME 4

uint16_t lcd_width;
uint16_t lcd_height;
uint16_t lcd_bitsperpixel;
uint16_t lcd_pixelsperbyte;
uint16_t lcd_bytes;
uint16_t lcd_pixelsperframe;
uint16_t lcd_wordsperplane;
uint16_t lcd_bytesperplane;
uint16_t lcd_planesperframe;
uint16_t lcd_wordsperframe;
uint16_t lcd_bytesperframe;

// raw data read from DMD
uint8_t planebuf1[MAX_WIDTH * MAX_HEIGHT * MAX_BITSPERPIXEL * MAX_PLANESPERFRAME / 8];
uint8_t planebuf2[MAX_WIDTH * MAX_HEIGHT * MAX_BITSPERPIXEL * MAX_PLANESPERFRAME / 8];
uint8_t *lastplane = planebuf2;

// processed frames (merged planes)
uint8_t framebuf1[MAX_WIDTH * MAX_HEIGHT * MAX_BITSPERPIXEL / 8];
uint8_t framebuf2[MAX_WIDTH * MAX_HEIGHT * MAX_BITSPERPIXEL/ 8];
uint8_t* lastframe = framebuf1;

uint32_t stat_frames_received=0;
uint32_t stat_spi_skipped=0;


// SPI PIO
PIO spi_pio;
uint spi_sm;

// DMD reader PIO
PIO dmd_pio;
uint dmd_sm;

// Frame detection PIO
PIO frame_pio;
uint frame_sm;

// DMA
uint dmd_dma_chan = 0;
uint spi_dma_chan = 1;

dma_channel_config dmd_dma_chan_cfg;
dma_channel_config spi_dma_chan_cfg;

volatile bool spi_dma_running=false;

// Interrupts
uint dmd_int = 0;

volatile bool frame_received = false;

/**
 * @brief Send data via SPI, transfer data via DMA
 *
 * @param buf a byte buffer
 * @param len
 */
void spi_send_dma(uint32_t *buf, uint16_t len)
{
    spi_dma_running=true;
    // SET DMA source address and immediately start transfer
    dma_channel_set_read_addr(spi_dma_chan,buf,false);
    dma_channel_set_trans_count(spi_dma_chan,len/4,true);
}

/**
 * @brief Send data via SPI, using blocking IO
 *
 * @param buf a byte buffer
 * @param len
 */
void spi_send_blocking(uint32_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i += 4)
    {
        pio_sm_put_blocking(spi_pio, spi_sm, *buf);
        buf++;
    } 
}

/**
 * @brief Check if there is still an active SPI data transfer
 * 
 * @return true if there is still data in the TX FIFO
 * @return false if there is no data in the TX FIFO
 */
bool spi_busy() {
    if (!(pio_sm_is_tx_fifo_empty (spi_pio, spi_sm))) {
        return true;
    }

    if (dma_channel_is_busy(spi_dma_chan)) {
        return true;
    }

    if (spi_dma_running) {
        return true;
    }

    return false;
}

/**
 * @brief Abort running SPI transfers. This can be necessary in case the SPI master hangs
 * 
 */
void spi_abort() {
    if (dma_channel_is_busy(spi_dma_chan)) {
        dma_channel_abort(spi_dma_chan);
    }

    if (!(pio_sm_is_tx_fifo_empty (spi_pio, spi_sm))) {
        pio_sm_clear_fifos (spi_pio, spi_sm);
    }

    spi_dma_running=false;
}

/**
 * @brief Notify on pin 17 that data are ready on SPI
 * 
 * The SPI master (the Pico is slave) should start a data transfer when this signal is received
 * It toggles pin 17 to H
 * 
 */
void start_spi()
{
    gpio_put(17, 1);
}

/**
 * @brief Set pin 17 to L to signal that there is no active SPI data transfer
 * 
 */
void finish_spi() {
    gpio_put(17, 0);
}


/**
 * @brief Send a pix buffer via SPI
 * 
 * @param pixbuf a frame to send
 */
bool spi_send_pix(uint8_t *pixbuf, bool skip_when_busy)
{
    block_header_t h = {
        .block_type = SPI_BLOCK_PIX};
    block_pix_header_t ph = {};

    // round length to 4-byte blocks
    h.len = (((lcd_bytes + 3) / 4) * 4) + sizeof(h) + sizeof(ph);
    ph.columns = lcd_width;
    ph.rows = lcd_height;
    ph.bitsperpixel = lcd_bitsperpixel;

    if (skip_when_busy) {
        if (spi_busy()) return false;
    }

    spi_send_blocking((uint32_t *)&h, sizeof(h));
    spi_send_blocking((uint32_t *)&ph, sizeof(ph));
    spi_send_dma((uint32_t *)pixbuf, lcd_bytes);
    start_spi();

    return true;
}


/**
 * @brief Count a clock using different PIO programs defined in dmd_counter.pio
 *
 * @return uint32_t Number of clocks per second
 */
uint32_t count_clock(const pio_program_t *program)
{

    dmd_pio = pio0;
    uint offset = pio_add_program(dmd_pio, program);
    dmd_sm = pio_claim_unused_sm(dmd_pio, true);
    dmd_counter_program_init(dmd_pio, dmd_sm, offset);
    pio_sm_set_enabled(dmd_pio, dmd_sm, true);
    sleep_ms(500);
    pio_sm_exec(dmd_pio, dmd_sm, pio_encode_in(pio_x, 32));
    uint32_t count = ~pio_sm_get(dmd_pio, dmd_sm);
    pio_sm_set_enabled(dmd_pio, dmd_sm, false);
    pio_remove_program(dmd_pio, program, offset);
    pio_sm_unclaim(dmd_pio, dmd_sm);

    return count * 2;
}

int detect_dmd()
{

    uint32_t dotclk = count_clock(&dmd_count_dotclk_program);
    uint32_t de = count_clock(&dmd_count_de_program);
    uint32_t rdata = count_clock(&dmd_count_rdata_program);

    printf("", dotclk, de, rdata);

    if ((dotclk > 450000) && (dotclk < 550000) &&
        (de > 3800) && (de < 4000) &&
        (rdata > 115) && (rdata < 130))
    {
        printf("WPC detected\n");
        return DMD_WPC;
    } else if ((dotclk > 640000) && (dotclk < 700000) &&
        (de > 5000) && (de < 5300) &&
        (rdata > 70) && (rdata < 85)) {
        printf("Stern Whitestar detected\n");
        return DMD_WHITESTAR;
    }

    return DMD_UNKNOWN;
}

/**
 * @brief Is being called when SPI DMA transfer has finished
 * 
 */
void spi_dma_handler() {
    // Clear the interrupt request
    dma_hw->ints1 = 1u << spi_dma_chan;

    finish_spi();
    spi_dma_running=false;
}

/**
 * @brief Handles DMD DMA requests by switching between the buffers
 * 
 */
void dmd_dma_handler() {

    uint8_t *target;
    uint32_t *planebuf;

    // Switch between buffers
    if (lastplane == planebuf1) {
        target = planebuf1;
        lastplane = planebuf2;
        lastframe = framebuf2;
    } else {
        target = planebuf2;
        lastplane = planebuf1;
        lastframe = framebuf1;
    }

    // Clear the interrupt request
    dma_hw->ints0 = 1u << dmd_dma_chan;

    // Start a new DMA transfer to the new buffer
    dma_channel_set_write_addr(dmd_dma_chan,target,true);

    // Just for debugging purposes
    stat_frames_received++;

    // Fix byte order within the buffer
    planebuf = (uint32_t *)lastplane;
    buf32_t *v;
    uint32_t res;
    for (int i=0; i<lcd_wordsperframe; i++) {
        v=(buf32_t*)planebuf;
        res=(v->byte3 << 24) | (v->byte2 << 16) | (v->byte1 << 8) | (v->byte0);
        *planebuf=res;
        planebuf++;
    }

    // Merge multiple planes

    // calculate offsets for each plane and cache these
    uint16_t offset[MAX_PLANESPERFRAME];
    for (int i=0; i<MAX_PLANESPERFRAME; i++) {
        offset[i]=i*lcd_wordsperplane;
    }

    // add all planes to get the frame data
    planebuf = (uint32_t *)lastplane;
    uint32_t *framebuf = (uint32_t *)lastframe;
    for (int px=0; px<lcd_wordsperplane; px++) {
        uint32_t pixval=0;
        for (int frame=0; frame<lcd_planesperframe; frame++) {
            pixval += planebuf[offset[frame]+px];
        }
        framebuf[px]=pixval;
    }

    frame_received=true;
}


bool init()
{
    stdio_init_all();

    printf("DMD reader starting\n");

    // this is uses to notify the Pi that data is available
    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_put(17, 0);
    printf("IRQ pin initialized\n");

    // initialize SPI slave PIO
    spi_pio = pio0;
    uint offset = pio_add_program(spi_pio, &clocked_output_program);
    spi_sm = pio_claim_unused_sm(spi_pio, true);
    clocked_output_program_init(spi_pio, spi_sm, offset, SPI_BASE);
    printf("SPI slave initialized \n");

    // Initialize DMD counter (only used for initialization)

    int dmd_type = detect_dmd();

    // Initialize DMD reader
    if (dmd_type == DMD_WPC)
    {
        dmd_pio = pio0;
        offset = pio_add_program(dmd_pio, &dmd_reader_wpc_program);
        dmd_sm = pio_claim_unused_sm(dmd_pio, true);
        dmd_reader_wpc_program_init(dmd_pio, dmd_sm, offset);
        printf("WPC DMD reader initialized\n");

        // The framedetect program just runs and detects the beginning of a new frame
        frame_pio = pio0;
        offset = pio_add_program(frame_pio, &dmd_framedetect_wpc_program);
        frame_sm = pio_claim_unused_sm(frame_pio, true);
        dmd_reader_wpc_program_init(frame_pio, frame_sm, offset);
        pio_sm_set_enabled(frame_pio, frame_sm, true);
        printf("WPC frame detection initialized\n");

        lcd_width = 128;
        lcd_height = 32;
        lcd_bitsperpixel = 2;
        lcd_pixelsperbyte = 8 / lcd_bitsperpixel;
        lcd_planesperframe = 3;
    }
    else
    {
        printf("Unknown DMD type, aborting\n");
        return false;
    }

    // Calculate Display parameters
    lcd_bytes = lcd_width * lcd_height * lcd_bitsperpixel / 8;
    lcd_pixelsperframe = lcd_width * lcd_height;
    lcd_wordsperplane = lcd_bytes / 4;
    lcd_bytesperplane = lcd_bytes;
    lcd_wordsperframe = lcd_wordsperplane * lcd_planesperframe;
    lcd_bytesperframe = lcd_bytesperplane * lcd_planesperframe;

    printf("LCD buffer initialized");

    // DMA for DMD reader
    dmd_dma_chan_cfg = dma_channel_get_default_config(dmd_dma_chan);
    channel_config_set_read_increment(&dmd_dma_chan_cfg, false);
    channel_config_set_write_increment(&dmd_dma_chan_cfg, true);
    channel_config_set_dreq(&dmd_dma_chan_cfg, pio_get_dreq(dmd_pio, dmd_sm, false));

    dma_channel_configure(dmd_dma_chan, &dmd_dma_chan_cfg,
                          NULL,                  // Destination pointer, needs to be set later
                          &dmd_pio->rxf[dmd_sm], // Source pointer
                          lcd_wordsperframe,     // Number of transfers
                          false                  // Do not yet start
    );

    dma_channel_set_irq0_enabled(dmd_dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dmd_dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // DMA for SPI
    spi_dma_chan_cfg = dma_channel_get_default_config(spi_dma_chan);
    channel_config_set_read_increment(&spi_dma_chan_cfg, true);
    channel_config_set_write_increment(&spi_dma_chan_cfg, false);
    channel_config_set_dreq(&spi_dma_chan_cfg, pio_get_dreq(spi_pio, spi_sm, true));

    dma_channel_configure(spi_dma_chan, &spi_dma_chan_cfg,
                          &spi_pio->txf[spi_sm], // Destination pointer
                          NULL,                  // Source pointer
                          0,                     // Number of transfers
                          false                  // Do not yet start
    );

    dma_channel_set_irq1_enabled(spi_dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_1, spi_dma_handler);
    irq_set_enabled(DMA_IRQ_1, true);

    // Finally start DMD reader PIO program and DMA
    dmd_dma_handler();
    pio_sm_set_enabled(dmd_pio, dmd_sm, true);

    return true;
}

int main()
{
    if (!init()) {
        printf("Error during initialisation, aborting...\n");
        return 0;
    }

    while (true) {
        // Wait for the next frame
        if (!(frame_received)) sleep_ms(1);
        frame_received=false; 

        // do something
        spi_send_pix(lastframe,true);
    }

    return 0;
}
