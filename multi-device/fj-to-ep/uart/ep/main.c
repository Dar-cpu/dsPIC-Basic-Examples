/*
 * dsPIC33EP32MC204 <-> dsPIC33FJ32MC204
 * UART bidireccional
 *
 * EP:
 *   U1TX -> RC8 / RP56 / pin 4
 *   U1RX -> RC9 / RP57 / pin 5
 *   LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * UART1: 115200 baud, 8N1
 *
 * Funcionamiento:
 *   EP espera 0xA5
 *   Al recibirlo cambia el estado del LED
 *   y responde 0x5A al FJ
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
    /* Todos los pines analogicos como digitales */
    ANSELA = 0x0000U;
    ANSELB = 0x0000U;
    ANSELC = 0x0000U;

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
     * U1TX -> RP56 / RC8
     * U1RX <- RP57 / RC9
     */

    pps_unlock();

    RPOR6bits.RP56R = 1U;
    RPINR18bits.U1RXR = 57U;

    pps_lock();

    TRISCbits.TRISC8 = 0U;     /* TX */
    TRISCbits.TRISC9 = 1U;     /* RX */

    U1MODE = 0x0000U;
    U1STA = 0x0000U;

    /*
     * FCY = 40 MHz
     * BRGH = 1
     * U1BRG = 86
     *
     * Baud real ~= 114943 baud
     */

    U1MODEbits.BRGH = 1U;
    U1BRG = 86U;

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
        /*
         * Recuperarse de un posible overrun.
         */
        if (U1STAbits.OERR != 0U)
        {
            U1STAbits.OERR = 0U;
        }

        /*
         * ¿Hay un byte recibido?
         */
        if (U1STAbits.URXDA != 0U)
        {
            uint8_t received;

            received = (uint8_t)(U1RXREG & 0x00FFU);

            /*
             * FJ -> EP: PING correcto.
             */
            if (received == 0xA5U)
            {
                /* Confirmar recepcion visualmente */
                LED_LAT ^= 1U;

                /* EP -> FJ: PONG */
                uart1_putc(0x5AU);
            }
        }
    }

    return 0;
}