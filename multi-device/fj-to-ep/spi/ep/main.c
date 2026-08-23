/*
 * dsPIC33EP32MC204 <-> dsPIC33FJ32MC204
 * SPI bidireccional / Full-Duplex
 *
 * EP = SLAVE
 *
 * SCK1 <- RC3 / pin 36
 * SDI1 <- RA9 / pin 35       (MOSI)
 * SDO1 -> RA4 / pin 34       (MISO)
 * SS1  <- RB0 / pin 21
 *
 * LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * SPI:
 *   8 bits
 *   CKP = 0
 *   CKE = 1
 *
 * El EP mantiene preparado 0x5A.
 *
 * Cuando el master genera los 8 clocks:
 *
 *   EP recibe 0xA5
 *   EP envia  0x5A
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
/* LED                                                                        */
/* -------------------------------------------------------------------------- */

#define LED_LAT     LATBbits.LATB4
#define LED_TRIS    TRISBbits.TRISB4


/* -------------------------------------------------------------------------- */
/* Clock                                                                      */
/* -------------------------------------------------------------------------- */

static void clock_init(void)
{
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
    /*
     * Pines digitales.
     *
     * Es especialmente importante para RB0,
     * porque comparte función con AN2.
     */

    ANSELA = 0x0000U;
    ANSELB = 0x0000U;
    ANSELC = 0x0000U;

    LED_LAT = 0U;
    LED_TRIS = 0U;
}


/* -------------------------------------------------------------------------- */
/* SPI1 Slave                                                                 */
/* -------------------------------------------------------------------------- */

static void spi1_slave_init(void)
{
    /*
     * SPI1 dedicado del dsPIC33EP32MC204:
     *
     * RC3 = SCK1
     * RA9 = SDI1
     * RA4 = SDO1
     * RB0 = SS1
     */

    TRISCbits.TRISC3 = 1U;       /* SCK entrada */
    TRISAbits.TRISA9 = 1U;       /* MOSI / SDI entrada */
    TRISAbits.TRISA4 = 0U;       /* MISO / SDO salida */
    TRISBbits.TRISB0 = 1U;       /* SS entrada */

    SPI1STATbits.SPIEN = 0U;
    SPI1STATbits.SPIROV = 0U;

    SPI1CON1 = 0x0000U;
    SPI1CON2 = 0x0000U;

    SPI1CON1bits.MSTEN = 0U;     /* Slave */
    SPI1CON1bits.MODE16 = 0U;    /* 8 bits */

    /*
     * Debe coincidir con el master.
     */
    SPI1CON1bits.CKP = 0U;
    SPI1CON1bits.CKE = 1U;

    /*
     * En Slave mode SMP debe permanecer en 0.
     */
    SPI1CON1bits.SMP = 0U;

    /*
     * Habilitar SS1 para seleccionar físicamente
     * este slave.
     */
    SPI1CON1bits.SSEN = 1U;

    /*
     * Standard Buffer mode.
     */
    SPI1CON2bits.SPIBEN = 0U;

    SPI1STATbits.SPIEN = 1U;

    /*
     * PRELOAD:
     *
     * El primer byte que el EP enviará por MISO
     * durante la siguiente transferencia será 0x5A.
     */
    SPI1BUF = 0x5AU;
}


/* -------------------------------------------------------------------------- */
/* Main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    uint8_t received;

    clock_init();
    gpio_init();
    spi1_slave_init();

    __delay_ms(100);

    while (1)
    {
        /*
         * Recuperarse de overflow.
         */
        if (SPI1STATbits.SPIROV != 0U)
        {
            SPI1STATbits.SPIROV = 0U;
        }

        /*
         * ¿Terminó una transferencia?
         */
        if (SPI1STATbits.SPIRBF != 0U)
        {
            /*
             * Este es el byte que llegó por MOSI.
             */
            received = (uint8_t)(SPI1BUF & 0x00FFU);

            /*
             * FJ -> EP correcto.
             */
            if (received == 0xA5U)
            {
                LED_LAT ^= 1U;
            }

            /*
             * Preparar nuevamente el PONG para
             * la próxima transferencia.
             */
            SPI1BUF = 0x5AU;
        }
    }

    return 0;
}