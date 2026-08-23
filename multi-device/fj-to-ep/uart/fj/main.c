/*
 * dsPIC33FJ32MC204 <-> dsPIC33EP32MC204
 * UART bidireccional
 *
 * FJ:
 *   U1TX -> RC8 / RP24 / pin 4
 *   U1RX -> RC9 / RP25 / pin 5
 *   LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * UART1: 115200 baud, 8N1
 *
 * Funcionamiento:
 *   FJ envia 0xA5
 *   EP responde 0x5A
 *   Si FJ recibe 0x5A, enciende el LED durante 100 ms
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
/* LED                                                                        */
/* -------------------------------------------------------------------------- */

#define LED_LAT     LATBbits.LATB4
#define LED_TRIS    TRISBbits.TRISB4


/* -------------------------------------------------------------------------- */
/* Clock                                                                      */
/* -------------------------------------------------------------------------- */

static void clock_init(void)
{
    /*
     * 8 MHz / 2 * 40 / 2 = 80 MHz FOSC
     * FCY = 40 MHz
     */

    PLLFBD = 38U;              /* M = 40 */
    CLKDIVbits.PLLPRE = 0U;    /* N1 = 2 */
    CLKDIVbits.PLLPOST = 0U;   /* N2 = 2 */

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
    /* Todos los pines analogicos como digitales */
    AD1PCFGL = 0xFFFFU;

    /* RB4 como salida para el LED */
    LED_LAT = 0U;
    LED_TRIS = 0U;
}


/* -------------------------------------------------------------------------- */
/* UART1                                                                      */
/* -------------------------------------------------------------------------- */

static void uart1_init(void)
{
    /*
     * U1TX -> RP24 / RC8
     * U1RX <- RP25 / RC9
     */

    pps_unlock();

    RPOR12bits.RP24R = 3U;
    RPINR18bits.U1RXR = 25U;

    pps_lock();

    TRISCbits.TRISC8 = 0U;     /* TX */
    TRISCbits.TRISC9 = 1U;     /* RX */

    U1MODE = 0x0000U;
    U1STA = 0x0000U;

    /*
     * FCY = 40 MHz
     * BRGH = 0
     * U1BRG = 21
     *
     * Baud real ~= 113636 baud
     */

    U1MODEbits.BRGH = 0U;
    U1BRG = 21U;

    U1MODEbits.PDSEL = 0U;     /* 8 bits, sin paridad */
    U1MODEbits.STSEL = 0U;     /* 1 stop bit */

    U1MODEbits.UARTEN = 1U;
    U1STAbits.UTXEN = 1U;
}


static void uart1_putc(uint8_t data)
{
    while (U1STAbits.UTXBF != 0U)
    {
        ;
    }

    U1TXREG = data;
}


static void uart1_flush_rx(void)
{
    while (U1STAbits.URXDA != 0U)
    {
        (void)U1RXREG;
    }

    if (U1STAbits.OERR != 0U)
    {
        U1STAbits.OERR = 0U;
    }
}


/* -------------------------------------------------------------------------- */
/* Main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    clock_init();
    gpio_init();
    uart1_init();

    __delay_ms(100);

    while (1)
    {
        uint16_t timeout = 300U;

        /* Eliminar cualquier byte anterior */
        uart1_flush_rx();

        /* FJ -> EP: PING */
        uart1_putc(0xA5U);

        /*
         * Esperar hasta 300 ms por la respuesta.
         */
        while (timeout > 0U)
        {
            if (U1STAbits.OERR != 0U)
            {
                U1STAbits.OERR = 0U;
            }

            if (U1STAbits.URXDA != 0U)
            {
                uint8_t received;

                received = (uint8_t)(U1RXREG & 0x00FFU);

                /*
                 * EP -> FJ: PONG correcto
                 */
                if (received == 0x5AU)
                {
                    LED_LAT = 1U;
                    __delay_ms(100);
                    LED_LAT = 0U;
                }

                break;
            }

            __delay_ms(1);
            timeout--;
        }

        __delay_ms(600);
    }

    return 0;
}