/*
 * dsPIC33EP32MC204 <-> dsPIC33EP32MC204
 * SPI1 Full-Duplex
 *
 * EP2 = SLAVE
 *
 * SCK1 <- RC3 / pin 36
 * SDI1 <- RA9 / pin 35      MOSI
 * SDO1 -> RA4 / pin 34      MISO
 * SS1  <- RB0 / pin 21
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
 *
 * EP2 espera 0xA5
 * y transmite 0x5A
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
     * SPI1 dedicado:
     *
     * RC3 = SCK1
     * RA9 = SDI1
     * RA4 = SDO1
     * RB0 = SS1
     */

    TRISCbits.TRISC3 = 1U;      /* SCK entrada */
    TRISAbits.TRISA9 = 1U;      /* MOSI entrada */
    TRISAbits.TRISA4 = 0U;      /* MISO salida */
    TRISBbits.TRISB0 = 1U;      /* SS entrada */

    SPI1STATbits.SPIEN = 0U;
    SPI1STATbits.SPIROV = 0U;

    SPI1CON1 = 0x0000U;
    SPI1CON2 = 0x0000U;

    /*
     * Slave, 8 bits.
     */
    SPI1CON1bits.MSTEN = 0U;
    SPI1CON1bits.MODE16 = 0U;

    /*
     * Debe coincidir con EP1.
     */
    SPI1CON1bits.CKP = 0U;
    SPI1CON1bits.CKE = 1U;

    /*
     * En slave SMP = 0.
     */
    SPI1CON1bits.SMP = 0U;

    /*
     * Habilitar SS1.
     */
    SPI1CON1bits.SSEN = 1U;

    /*
     * Standard buffer mode.
     */
    SPI1CON2bits.SPIBEN = 0U;

    SPI1STATbits.SPIEN = 1U;

    /*
     * Preparar primer byte de respuesta.
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
         * Recuperar overflow.
         */
        if (SPI1STATbits.SPIROV != 0U)
        {
            SPI1STATbits.SPIROV = 0U;
        }

        /*
         * Transferencia terminada.
         */
        if (SPI1STATbits.SPIRBF != 0U)
        {
            received =
                (uint8_t)(SPI1BUF & 0x00FFU);

            /*
             * EP1 -> EP2 correcto.
             */
            if (received == 0xA5U)
            {
                LED_LAT ^= 1U;
            }

            /*
             * Preparar respuesta para
             * la próxima transferencia.
             */
            SPI1BUF = 0x5AU;
        }
    }

    return 0;
}