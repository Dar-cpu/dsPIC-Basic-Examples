/*
 * dsPIC33FJ32MC204 <-> dsPIC33EP32MC204
 * SPI bidireccional / Full-Duplex
 *
 * FJ = MASTER
 *
 * SCK1 -> RC5 / RP21 / pin 38
 * SDO1 -> RC4 / RP20 / pin 37  (MOSI)
 * SDI1 <- RC3 / RP19 / pin 36  (MISO)
 * CS   -> RB13       / pin 11
 *
 * LED  -> RB4        / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * SPI:
 *   8 bits
 *   CKP = 0
 *   CKE = 1
 *   625 kHz
 *
 * El FJ envia 0xA5.
 * Simultaneamente espera recibir 0x5A.
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


/* -------------------------------------------------------------------------- */
/* Configuration Bits                                                         */
/* -------------------------------------------------------------------------- */

#pragma config POSCMD = HS
#pragma config FNOSC = PRIPLL
#pragma config IESO = OFF
#pragma config FCKSM = CSDCMD
#pragma config OSCIOFNC = OFF

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config ICS = PGD1
#pragma config IOL1WAY = OFF


/* -------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* -------------------------------------------------------------------------- */

#define LED_LAT     LATBbits.LATB4
#define LED_TRIS    TRISBbits.TRISB4

#define CS_LAT      LATBbits.LATB13
#define CS_TRIS     TRISBbits.TRISB13


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
/* PPS                                                                        */
/* -------------------------------------------------------------------------- */

static void pps_unlock(void)
{
    __builtin_write_OSCCONL(OSCCON & 0xBFU);
}


static void pps_lock(void)
{
    __builtin_write_OSCCONL(OSCCON | 0x40U);
}


/* -------------------------------------------------------------------------- */
/* GPIO                                                                       */
/* -------------------------------------------------------------------------- */

static void gpio_init(void)
{
    AD1PCFGL = 0xFFFFU;

    /* LED */
    LED_LAT = 0U;
    LED_TRIS = 0U;

    /*
     * Slave Select.
     *
     * HIGH = EP no seleccionado
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
     * FJ PPS:
     *
     * RP21 / RC5 -> SCK1OUT
     * RP20 / RC4 -> SDO1
     * RP19 / RC3 -> SDI1
     */

    pps_unlock();

    RPOR10bits.RP21R = 8U;       /* SCK1OUT */
    RPOR10bits.RP20R = 7U;       /* SDO1 */
    RPINR20bits.SDI1R = 19U;     /* SDI1 <- RP19 */

    pps_lock();

    TRISCbits.TRISC5 = 0U;       /* SCK salida */
    TRISCbits.TRISC4 = 0U;       /* MOSI salida */
    TRISCbits.TRISC3 = 1U;       /* MISO entrada */

    SPI1STATbits.SPIEN = 0U;
    SPI1STATbits.SPIROV = 0U;

    SPI1CON1 = 0x0000U;
    SPI1CON2 = 0x0000U;

    SPI1CON1bits.MSTEN = 1U;     /* Master */
    SPI1CON1bits.MODE16 = 0U;    /* 8 bits */

    /*
     * SPI:
     *
     * CKP = 0 -> reloj idle LOW
     * CKE = 1 -> cambia datos Active -> Idle
     */
    SPI1CON1bits.CKP = 0U;
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

    SPI1CON1bits.PPRE = 1U;      /* 16:1 */
    SPI1CON1bits.SPRE = 4U;      /* 4:1 */

    SPI1STATbits.SPIEN = 1U;
}


/* -------------------------------------------------------------------------- */
/* Transferencia full-duplex                                                  */
/* -------------------------------------------------------------------------- */

static uint8_t spi1_transfer(uint8_t tx)
{
    uint8_t rx;

    /*
     * Escribir un byte inicia la transferencia.
     */
    SPI1BUF = tx;

    /*
     * Esperar los 8 clocks.
     */
    while (SPI1STATbits.SPIRBF == 0U)
    {
        ;
    }

    rx = (uint8_t)(SPI1BUF & 0x00FFU);

    return rx;
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
         * Seleccionar EP.
         */
        CS_LAT = 0U;

        /*
         * Pequeño tiempo de establecimiento.
         */
        __delay_us(2);

        /*
         * Full-duplex:
         *
         * MOSI: FJ -> EP = 0xA5
         * MISO: EP -> FJ = 0x5A
         */
        received = spi1_transfer(0xA5U);

        /*
         * Deseleccionar EP.
         */
        CS_LAT = 1U;

        /*
         * PONG correcto.
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