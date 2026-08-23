/*
 * dsPIC33EP32MC204 <-> dsPIC33EP32MC204
 * SPI1 Full-Duplex
 *
 * EP1 = MASTER
 *
 * SCK1 -> RC3 / pin 36
 * SDO1 -> RA4 / pin 34      MOSI
 * SDI1 <- RA9 / pin 35      MISO
 * CS   -> RB0 / pin 21
 *
 * LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * SPI1:
 *   8 bits
 *   CKP = 0
 *   CKE = 1
 *   625 kHz
 *
 * EP1 envia 0xA5
 * EP2 responde 0x5A simultaneamente
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


/* -------------------------------------------------------------------------- */
/* Configuration Bits                                                         */
/* -------------------------------------------------------------------------- */

#pragma config ICS = PGD3
#pragma config JTAGEN = OFF

#pragma config ALTI2C1 = OFF
#pragma config ALTI2C2 = OFF

#pragma config PLLKEN = ON
#pragma config FWDTEN = OFF

#pragma config POSCMD = HS
#pragma config FNOSC = PRIPLL
#pragma config IESO = OFF
#pragma config OSCIOFNC = OFF
#pragma config IOL1WAY = OFF
#pragma config FCKSM = CSDCMD


/* -------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* -------------------------------------------------------------------------- */

#define LED_LAT     LATBbits.LATB4
#define LED_TRIS    TRISBbits.TRISB4

#define CS_LAT      LATBbits.LATB0
#define CS_TRIS     TRISBbits.TRISB0


/* -------------------------------------------------------------------------- */
/* Clock                                                                      */
/* -------------------------------------------------------------------------- */

static void clock_init(void)
{
    /*
     * 8 MHz / 2 * 40 / 2 = 80 MHz FOSC
     * FCY = 40 MHz
     */

    PLLFBD = 38U;
    CLKDIVbits.PLLPRE = 0U;
    CLKDIVbits.PLLPOST = 0U;

    while (OSCCONbits.LOCK == 0U)
    {
        ;
    }
}


/* -------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* -------------------------------------------------------------------------- */

static void gpio_init(void)
{
    ANSELA = 0x0000U;
    ANSELB = 0x0000U;
    ANSELC = 0x0000U;

    LED_LAT = 0U;
    LED_TRIS = 0U;

    /*
     * Slave no seleccionado inicialmente.
     */
    CS_LAT = 1U;
    CS_TRIS = 0U;
}


/* -------------------------------------------------------------------------- */
/* SPI1 Master                                                                */
/* -------------------------------------------------------------------------- */

static void spi1_init(void)
{
    /*
     * SPI1 dedicado:
     *
     * RC3 = SCK1
     * RA4 = SDO1
     * RA9 = SDI1
     */

    TRISCbits.TRISC3 = 0U;      /* SCK salida */
    TRISAbits.TRISA4 = 0U;      /* SDO / MOSI salida */
    TRISAbits.TRISA9 = 1U;      /* SDI / MISO entrada */

    SPI1STATbits.SPIEN = 0U;
    SPI1STATbits.SPIROV = 0U;

    SPI1CON1 = 0x0000U;
    SPI1CON2 = 0x0000U;

    /*
     * Master, 8 bits.
     */
    SPI1CON1bits.MSTEN = 1U;
    SPI1CON1bits.MODE16 = 0U;

    /*
     * Clock idle LOW.
     */
    SPI1CON1bits.CKP = 0U;

    /*
     * Datos cambian Active -> Idle.
     */
    SPI1CON1bits.CKE = 1U;

    SPI1CON1bits.SMP = 0U;

    /*
     * FCY = 40 MHz
     *
     * Primary   = 16:1
     * Secondary = 4:1
     *
     * 40 MHz / 16 / 4 = 625 kHz
     */
    SPI1CON1bits.PPRE = 1U;
    SPI1CON1bits.SPRE = 4U;

    /*
     * SS controlado manualmente mediante RB0.
     */
    SPI1CON1bits.SSEN = 0U;

    SPI1STATbits.SPIEN = 1U;
}


/* -------------------------------------------------------------------------- */
/* Transferencia SPI                                                          */
/* -------------------------------------------------------------------------- */

static uint8_t spi1_transfer(uint8_t tx)
{
    SPI1BUF = tx;

    while (SPI1STATbits.SPIRBF == 0U)
    {
        ;
    }

    return (uint8_t)(SPI1BUF & 0x00FFU);
}


/* -------------------------------------------------------------------------- */
/* Main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    uint8_t received;

    clock_init();
    gpio_init();
    spi1_init();

    __delay_ms(100);

    while (1)
    {
        /*
         * Seleccionar EP2.
         */
        CS_LAT = 0U;

        __delay_us(2);

        /*
         * Full-Duplex:
         *
         * MOSI -> 0xA5
         * MISO <- 0x5A
         */
        received = spi1_transfer(0xA5U);

        CS_LAT = 1U;

        /*
         * Respuesta correcta.
         */
        if (received == 0x5AU)
        {
            LED_LAT = 1U;

            __delay_ms(100);

            LED_LAT = 0U;
        }

        __delay_ms(600);
    }

    return 0;
}