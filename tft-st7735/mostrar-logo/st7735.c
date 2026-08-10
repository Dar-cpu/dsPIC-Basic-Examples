#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "st7735.h"

/*
 * -------------------------------------------------------------
 * CONEXION PROPUESTA dsPIC33FJ32MC204 (44-pin TQFP) -> TFT
 * -------------------------------------------------------------
 *
 * TFT 1  GND  -> GND
 * TFT 2  VCC  -> 3.3 V
 * TFT 3  SCL  -> RC5 / RP21 / pin fisico 38  = SCK1
 * TFT 4  SDA  -> RC4 / RP20 / pin fisico 37  = SDO1 (MOSI)
 * TFT 5  RES  -> RC3 / RP19 / pin fisico 36
 * TFT 6  DC   -> RB12 / RP12 / pin fisico 10
 * TFT 7  CS   -> RB13 / RP13 / pin fisico 11
 * TFT 8  BL   -> 3.3 V
 * BL se deja directo a 3.3 V para esta prueba.
 */

/* GPIO de control */
#define TFT_RST_LAT     LATCbits.LATC3
#define TFT_RST_TRIS    TRISCbits.TRISC3

#define TFT_DC_LAT      LATBbits.LATB12
#define TFT_DC_TRIS     TRISBbits.TRISB12

#define TFT_CS_LAT      LATBbits.LATB13
#define TFT_CS_TRIS     TRISBbits.TRISB13

/* ST7735 commands */
#define ST7735_SWRESET  0x01U
#define ST7735_SLPOUT   0x11U
#define ST7735_NORON    0x13U
#define ST7735_INVOFF   0x20U
#define ST7735_DISPON   0x29U
#define ST7735_CASET    0x2AU
#define ST7735_RASET    0x2BU
#define ST7735_RAMWR    0x2CU
#define ST7735_MADCTL   0x36U
#define ST7735_COLMOD   0x3AU
#define ST7735_FRMCTR1  0xB1U
#define ST7735_FRMCTR2  0xB2U
#define ST7735_FRMCTR3  0xB3U
#define ST7735_INVCTR   0xB4U
#define ST7735_PWCTR1   0xC0U
#define ST7735_PWCTR2   0xC1U
#define ST7735_PWCTR3   0xC2U
#define ST7735_PWCTR4   0xC3U
#define ST7735_PWCTR5   0xC4U
#define ST7735_VMCTR1   0xC5U

/*
 * Muchos TFT 1.8" 128x160 no necesitan offset.
 * Si tu imagen aparece corrida 1-3 pixeles, modifica estos dos valores.
 */
#define ST7735_XSTART   0U
#define ST7735_YSTART   0U

/*
 * 0xC0 = portrait y orden RGB para este panel.
 * El valor anterior 0xC8 activaba el bit BGR y convertia el azul en naranja.
 * Si alguna variante necesita BGR, use 0xC8.
 */
#define ST7735_MADCTL_VALUE  0xC0U

static void pps_unlock(void)
{
    __builtin_write_OSCCONL(OSCCON & 0xBFU);
}

static void pps_lock(void)
{
    __builtin_write_OSCCONL(OSCCON | 0x40U);
}

static void spi1_init(void)
{
    /*
     * PPS:
     *   RP20/RC4 <- SDO1     funcion PPS 7
     *   RP21/RC5 <- SCK1OUT  funcion PPS 8
     */
    pps_unlock();
    RPOR10bits.RP20R = 7U;
    RPOR10bits.RP21R = 8U;
    pps_lock();

    TRISCbits.TRISC4 = 0U;
    TRISCbits.TRISC5 = 0U;

    SPI1STATbits.SPIEN = 0U;
    SPI1STATbits.SPIROV = 0U;

    SPI1CON1 = 0x0000U;
    SPI1CON2 = 0x0000U;

    SPI1CON1bits.MSTEN = 1U;  /* Master */
    SPI1CON1bits.MODE16 = 0U; /* 8 bits */
    SPI1CON1bits.CKP = 0U;    /* Clock idle LOW */
    SPI1CON1bits.CKE = 1U;    /* SPI mode compatible con ST7735 */
    SPI1CON1bits.SMP = 0U;

    /*
     * FCY = 40 MHz.
     * Primary 1:4, secondary 1:1 => SCK = 10 MHz.
     * El dsPIC33FJ32MC204 especifica SPI de hasta 15 Mbps.
     */
    SPI1CON1bits.PPRE = 2U;   /* 1:4 */
    SPI1CON1bits.SPRE = 7U;   /* 1:1 */

    SPI1STATbits.SPIEN = 1U;
}

static uint8_t spi1_write8(uint8_t value)
{
    uint16_t rx;

    while (SPI1STATbits.SPITBF != 0U)
    {
        ;
    }

    SPI1BUF = (uint16_t)value;

    while (SPI1STATbits.SPIRBF == 0U)
    {
        ;
    }

    rx = SPI1BUF;
    return (uint8_t)rx;
}

static void tft_select(void)
{
    TFT_CS_LAT = 0U;
}

static void tft_unselect(void)
{
    TFT_CS_LAT = 1U;
}

static void st7735_command(uint8_t command)
{
    tft_select();
    TFT_DC_LAT = 0U;
    (void)spi1_write8(command);
    tft_unselect();
}

static void st7735_data8(uint8_t data)
{
    tft_select();
    TFT_DC_LAT = 1U;
    (void)spi1_write8(data);
    tft_unselect();
}

static void st7735_command_data(uint8_t command, const uint8_t *data, uint8_t length)
{
    uint8_t i;

    tft_select();

    TFT_DC_LAT = 0U;
    (void)spi1_write8(command);

    TFT_DC_LAT = 1U;
    for (i = 0U; i < length; i++)
    {
        (void)spi1_write8(data[i]);
    }

    tft_unselect();
}

static void st7735_set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint16_t x0 = (uint16_t)(x + ST7735_XSTART);
    uint16_t y0 = (uint16_t)(y + ST7735_YSTART);
    uint16_t x1 = (uint16_t)(x0 + w - 1U);
    uint16_t y1 = (uint16_t)(y0 + h - 1U);
    uint8_t data[4];

    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)x0;
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)x1;
    st7735_command_data(ST7735_CASET, data, 4U);

    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)y0;
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)y1;
    st7735_command_data(ST7735_RASET, data, 4U);

    st7735_command(ST7735_RAMWR);
}

static void st7735_stream_color(uint16_t color, uint32_t count)
{
    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)color;

    tft_select();
    TFT_DC_LAT = 1U;

    while (count > 0UL)
    {
        (void)spi1_write8(hi);
        (void)spi1_write8(lo);
        count--;
    }

    tft_unselect();
}

void st7735_init(void)
{
    static const uint8_t frm12[] = {0x01U, 0x2CU, 0x2DU};
    static const uint8_t frm3[]  = {0x01U, 0x2CU, 0x2DU, 0x01U, 0x2CU, 0x2DU};
    static const uint8_t inv[]   = {0x07U};
    static const uint8_t pwr1[]  = {0xA2U, 0x02U, 0x84U};
    static const uint8_t pwr2[]  = {0xC5U};
    static const uint8_t pwr3[]  = {0x0AU, 0x00U};
    static const uint8_t pwr4[]  = {0x8AU, 0x2AU};
    static const uint8_t pwr5[]  = {0x8AU, 0xEEU};
    static const uint8_t vmctr[] = {0x0EU};
    static const uint8_t colmod[] = {0x05U}; /* RGB565 = 16 bit */
    static const uint8_t madctl[] = {ST7735_MADCTL_VALUE};

    TFT_CS_LAT = 1U;
    TFT_DC_LAT = 0U;
    TFT_RST_LAT = 1U;

    TFT_CS_TRIS = 0U;
    TFT_DC_TRIS = 0U;
    TFT_RST_TRIS = 0U;

    spi1_init();

    __delay_ms(20);
    TFT_RST_LAT = 0U;
    __delay_ms(20);
    TFT_RST_LAT = 1U;
    __delay_ms(150);

    st7735_command(ST7735_SWRESET);
    __delay_ms(150);

    st7735_command(ST7735_SLPOUT);
    __delay_ms(120);

    st7735_command_data(ST7735_FRMCTR1, frm12, sizeof(frm12));
    st7735_command_data(ST7735_FRMCTR2, frm12, sizeof(frm12));
    st7735_command_data(ST7735_FRMCTR3, frm3,  sizeof(frm3));
    st7735_command_data(ST7735_INVCTR,  inv,   sizeof(inv));

    st7735_command_data(ST7735_PWCTR1, pwr1, sizeof(pwr1));
    st7735_command_data(ST7735_PWCTR2, pwr2, sizeof(pwr2));
    st7735_command_data(ST7735_PWCTR3, pwr3, sizeof(pwr3));
    st7735_command_data(ST7735_PWCTR4, pwr4, sizeof(pwr4));
    st7735_command_data(ST7735_PWCTR5, pwr5, sizeof(pwr5));
    st7735_command_data(ST7735_VMCTR1, vmctr, sizeof(vmctr));

    st7735_command(ST7735_INVOFF);
    st7735_command_data(ST7735_MADCTL, madctl, sizeof(madctl));
    st7735_command_data(ST7735_COLMOD, colmod, sizeof(colmod));

    st7735_command(ST7735_NORON);
    __delay_ms(10);

    st7735_command(ST7735_DISPON);
    __delay_ms(100);

    st7735_fill_screen(TFT_COLOR_NAVY);
}

void st7735_fill_screen(uint16_t color)
{
    st7735_set_window(0U, 0U, ST7735_WIDTH, ST7735_HEIGHT);
    st7735_stream_color(color, (uint32_t)ST7735_WIDTH * (uint32_t)ST7735_HEIGHT);
}

void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if ((w == 0U) || (h == 0U)) return;
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;

    if ((uint16_t)(x + w) > ST7735_WIDTH)
        w = (uint16_t)(ST7735_WIDTH - x);

    if ((uint16_t)(y + h) > ST7735_HEIGHT)
        h = (uint16_t)(ST7735_HEIGHT - y);

    st7735_set_window(x, y, w, h);
    st7735_stream_color(color, (uint32_t)w * (uint32_t)h);
}

/*
 * "HI," grande, dibujado con rectangulos.
 * Asi evitamos cargar una fuente completa solo para esta prueba.
 */
void st7735_draw_hi(uint16_t x, uint16_t y, uint16_t color)
{
    const uint16_t t = 5U;
    const uint16_t h = 30U;

    /* H */
    st7735_fill_rect(x, y, t, h, color);
    st7735_fill_rect((uint16_t)(x + 17U), y, t, h, color);
    st7735_fill_rect(x, (uint16_t)(y + 12U), 22U, 5U, color);

    /* I */
    x = (uint16_t)(x + 32U);
    st7735_fill_rect(x, y, 22U, t, color);
    st7735_fill_rect((uint16_t)(x + 8U), y, 5U, h, color);
    st7735_fill_rect(x, (uint16_t)(y + h - t), 22U, t, color);

    /* coma */
    x = (uint16_t)(x + 30U);
    st7735_fill_rect(x, (uint16_t)(y + 23U), 6U, 6U, color);
    st7735_fill_rect((uint16_t)(x - 3U), (uint16_t)(y + 28U), 5U, 5U, color);
}

void st7735_draw_image_2bpp(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t *data,
    const uint16_t *palette)
{
    uint16_t px;
    uint16_t py;
    uint32_t pixel_number;
    uint32_t byte_index = 0UL;
    uint8_t packed_byte = 0U;
    uint8_t shift;
    uint8_t pixel_index;
    uint16_t color;

    if ((data == 0) || (palette == 0)) return;
    if ((width == 0U) || (height == 0U)) return;

    if ((x + width > ST7735_WIDTH) ||
        (y + height > ST7735_HEIGHT))
    {
        return;
    }

    st7735_set_window(x, y, width, height);

    tft_select();
    TFT_DC_LAT = 1U;

    for (py = 0U; py < height; py++)
    {
        for (px = 0U; px < width; px++)
        {
            pixel_number =
                ((uint32_t)py * (uint32_t)width) + (uint32_t)px;

            if ((pixel_number & 3UL) == 0UL)
            {
                packed_byte = data[byte_index++];
            }

            shift = (uint8_t)(
                6U - (uint8_t)((pixel_number & 3UL) * 2UL)
            );

            pixel_index =
                (uint8_t)((packed_byte >> shift) & 0x03U);

            color = palette[pixel_index];

            (void)spi1_write8((uint8_t)(color >> 8));
            (void)spi1_write8((uint8_t)color);
        }
    }

    tft_unselect();
}
