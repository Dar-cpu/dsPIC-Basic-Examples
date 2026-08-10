/*
 * dsPIC33FJ32MC204 + TFT 1.8" 128x160 SPI
 *
 * - Cristal externo de 8 MHz
 * - PLL -> FOSC = 80 MHz
 * - FCY = 40 MHz
 * - TFT ST7735/ST7735S por SPI1 a 10 MHz
 * - Muestra "HI," + imagen generada externamente
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>

#include "st7735.h"
#include "microchip.h"


static void clock_init(void)
{
    /*
     * 8 MHz / N1=2 * M=40 / N2=2
     *
     * FOSC = 80 MHz
     * FCY  = 40 MHz
     */

    PLLFBD = 38U;
    CLKDIVbits.PLLPRE = 0U;
    CLKDIVbits.PLLPOST = 0U;

    while (OSCCONbits.LOCK == 0U)
    {
        ;
    }
}


int main(void)
{
    uint16_t logo_x;

    clock_init();

    st7735_init();

    /* Fondo */
    st7735_fill_screen(TFT_COLOR_NAVY);

    /* Texto superior */
    st7735_draw_hi(
        22U,
        12U,
        TFT_COLOR_WHITE
    );

    /*
     * Centrado horizontal automático.
     *
     * 128 px -> X = 0
     * 120 px -> X = 4
     * 100 px -> X = 14
     */
    logo_x =
        (uint16_t)((ST7735_WIDTH - MICROCHIP_WIDTH) / 2U);

    st7735_draw_image_2bpp(
        logo_x,
        62U,
        MICROCHIP_WIDTH,
        MICROCHIP_HEIGHT,
        microchip_2bpp,
        microchip_palette_rgb565
    );

    while (1)
    {
        __delay_ms(1000);
    }

    return 0;
}