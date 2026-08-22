#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "st7735.h"

/*
 * -------------------------------------------------------------
 * dsPIC33FJ32MC204 -> TFT ST7735
 * -------------------------------------------------------------
 *
 * TFT 1 GND -> GND
 * TFT 2 VCC -> 3.3 V
 * TFT 3 SCL -> RC5 / RP21 / pin 38 = SCK1
 * TFT 4 SDA -> RC4 / RP20 / pin 37 = SDO1
 * TFT 5 RES -> RC3 / RP19 / pin 36
 * TFT 6 DC  -> RB12 / RP12 / pin 10
 * TFT 7 CS  -> RB13 / RP13 / pin 11
 * TFT 8 BL  -> 3.3 V
 */

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

#define ST7735_XSTART   0U
#define ST7735_YSTART   0U

/*
 * Este panel ya fue validado con RGB:
 * 0xC0 mantiene la orientacion usada y deja BGR desactivado.
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
     * RP20/RC4 -> SDO1     funcion PPS 7
     * RP21/RC5 -> SCK1OUT  funcion PPS 8
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

    SPI1CON1bits.MSTEN = 1U;
    SPI1CON1bits.MODE16 = 0U;
    SPI1CON1bits.CKP = 0U;
    SPI1CON1bits.CKE = 1U;
    SPI1CON1bits.SMP = 0U;

    /*
     * FCY = 40 MHz
     * PPRE 1:4
     * SPRE 1:1
     * SPI = 10 MHz
     */
    SPI1CON1bits.PPRE = 2U;
    SPI1CON1bits.SPRE = 7U;

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


static void st7735_command_data(
    uint8_t command,
    const uint8_t *data,
    uint8_t length
)
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


static void st7735_set_window(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h
)
{
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
    uint8_t data[4];

    x0 = (uint16_t)(x + ST7735_XSTART);
    y0 = (uint16_t)(y + ST7735_YSTART);

    x1 = (uint16_t)(x0 + w - 1U);
    y1 = (uint16_t)(y0 + h - 1U);

    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)x0;
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)x1;

    st7735_command_data(
        ST7735_CASET,
        data,
        4U
    );

    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)y0;
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)y1;

    st7735_command_data(
        ST7735_RASET,
        data,
        4U
    );

    st7735_command(ST7735_RAMWR);
}


static void st7735_stream_color(
    uint16_t color,
    uint32_t count
)
{
    uint8_t hi;
    uint8_t lo;

    hi = (uint8_t)(color >> 8);
    lo = (uint8_t)color;

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
    static const uint8_t frm12[] =
    {
        0x01U, 0x2CU, 0x2DU
    };

    static const uint8_t frm3[] =
    {
        0x01U, 0x2CU, 0x2DU,
        0x01U, 0x2CU, 0x2DU
    };

    static const uint8_t inv[] =
    {
        0x07U
    };

    static const uint8_t pwr1[] =
    {
        0xA2U, 0x02U, 0x84U
    };

    static const uint8_t pwr2[] =
    {
        0xC5U
    };

    static const uint8_t pwr3[] =
    {
        0x0AU, 0x00U
    };

    static const uint8_t pwr4[] =
    {
        0x8AU, 0x2AU
    };

    static const uint8_t pwr5[] =
    {
        0x8AU, 0xEEU
    };

    static const uint8_t vmctr[] =
    {
        0x0EU
    };

    static const uint8_t colmod[] =
    {
        0x05U
    };

    static const uint8_t madctl[] =
    {
        ST7735_MADCTL_VALUE
    };

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

    st7735_command_data(
        ST7735_FRMCTR1,
        frm12,
        sizeof(frm12)
    );

    st7735_command_data(
        ST7735_FRMCTR2,
        frm12,
        sizeof(frm12)
    );

    st7735_command_data(
        ST7735_FRMCTR3,
        frm3,
        sizeof(frm3)
    );

    st7735_command_data(
        ST7735_INVCTR,
        inv,
        sizeof(inv)
    );

    st7735_command_data(
        ST7735_PWCTR1,
        pwr1,
        sizeof(pwr1)
    );

    st7735_command_data(
        ST7735_PWCTR2,
        pwr2,
        sizeof(pwr2)
    );

    st7735_command_data(
        ST7735_PWCTR3,
        pwr3,
        sizeof(pwr3)
    );

    st7735_command_data(
        ST7735_PWCTR4,
        pwr4,
        sizeof(pwr4)
    );

    st7735_command_data(
        ST7735_PWCTR5,
        pwr5,
        sizeof(pwr5)
    );

    st7735_command_data(
        ST7735_VMCTR1,
        vmctr,
        sizeof(vmctr)
    );

    st7735_command(ST7735_INVOFF);

    st7735_command_data(
        ST7735_MADCTL,
        madctl,
        sizeof(madctl)
    );

    st7735_command_data(
        ST7735_COLMOD,
        colmod,
        sizeof(colmod)
    );

    st7735_command(ST7735_NORON);
    __delay_ms(10);

    st7735_command(ST7735_DISPON);
    __delay_ms(100);

    st7735_fill_screen(TFT_COLOR_NAVY);
}


void st7735_fill_screen(uint16_t color)
{
    st7735_set_window(
        0U,
        0U,
        ST7735_WIDTH,
        ST7735_HEIGHT
    );

    st7735_stream_color(
        color,
        (uint32_t)ST7735_WIDTH
        * (uint32_t)ST7735_HEIGHT
    );
}


void st7735_fill_rect(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    uint16_t color
)
{
    if ((w == 0U) || (h == 0U))
    {
        return;
    }

    if ((x >= ST7735_WIDTH) ||
        (y >= ST7735_HEIGHT))
    {
        return;
    }

    if ((uint16_t)(x + w) > ST7735_WIDTH)
    {
        w = (uint16_t)(ST7735_WIDTH - x);
    }

    if ((uint16_t)(y + h) > ST7735_HEIGHT)
    {
        h = (uint16_t)(ST7735_HEIGHT - y);
    }

    st7735_set_window(
        x,
        y,
        w,
        h
    );

    st7735_stream_color(
        color,
        (uint32_t)w * (uint32_t)h
    );
}


void st7735_draw_hline(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t color
)
{
    st7735_fill_rect(
        x,
        y,
        w,
        1U,
        color
    );
}


void st7735_draw_vline(
    uint16_t x,
    uint16_t y,
    uint16_t h,
    uint16_t color
)
{
    st7735_fill_rect(
        x,
        y,
        1U,
        h,
        color
    );
}


void st7735_draw_pixel(
    uint16_t x,
    uint16_t y,
    uint16_t color
)
{
    st7735_fill_rect(
        x,
        y,
        1U,
        1U,
        color
    );
}
