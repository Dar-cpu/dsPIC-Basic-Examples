/*
 * dsPIC33FJ32MC204 <-> dsPIC33EP32MC204
 * I2C bidireccional
 *
 * FJ = MASTER
 *
 * I2C1:
 *   SDA1 -> RB9 / pin 1
 *   SCL1 -> RB8 / pin 44
 *
 * LED:
 *   RB4 / pin 33
 *
 * Slave EP:
 *   direccion 7-bit = 0x42
 *
 * Funcionamiento:
 *   FJ escribe 0xA5 al EP
 *   FJ lee 0x5A desde el EP
 *   si recibe 0x5A, enciende LED durante 100 ms
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


#define EP_I2C_ADDRESS    0x42U

#define LED_LAT           LATBbits.LATB4
#define LED_TRIS          TRISBbits.TRISB4


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

#pragma config ALTI2C = OFF


/* -------------------------------------------------------------------------- */
/* Clock                                                                      */
/* -------------------------------------------------------------------------- */

static void clock_init(void)
{
    PLLFBD = 38U;              /* M = 40 */
    CLKDIVbits.PLLPRE = 0U;    /* N1 = 2 */
    CLKDIVbits.PLLPOST = 0U;   /* N2 = 2 */

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
    AD1PCFGL = 0xFFFFU;

    LED_LAT = 0U;
    LED_TRIS = 0U;
}


/* -------------------------------------------------------------------------- */
/* I2C1 Master                                                                */
/* -------------------------------------------------------------------------- */

static void i2c1_init(void)
{
    /*
     * I2C1 estándar:
     *
     * SDA1 = RB9
     * SCL1 = RB8
     */

    TRISBbits.TRISB9 = 1U;
    TRISBbits.TRISB8 = 1U;

    I2C1CON = 0x0000U;
    I2C1STAT = 0x0000U;

    /*
     * FCY = 40 MHz
     * aproximadamente 100 kHz.
     *
     * Este mismo valor ya se había usado en el FJ
     * para I2C1 a ~100 kHz.
     */
    I2C1BRG = 393U;

    I2C1CONbits.I2CEN = 1U;

    __delay_ms(5);
}


static void i2c1_wait_idle(void)
{
    while (
        ((I2C1CON & 0x001FU) != 0U) ||
        (I2C1STATbits.TRSTAT != 0U)
    )
    {
        ;
    }
}


static void i2c1_start(void)
{
    i2c1_wait_idle();

    I2C1CONbits.SEN = 1U;

    while (I2C1CONbits.SEN != 0U)
    {
        ;
    }
}


static void i2c1_stop(void)
{
    i2c1_wait_idle();

    I2C1CONbits.PEN = 1U;

    while (I2C1CONbits.PEN != 0U)
    {
        ;
    }
}


static uint8_t i2c1_write(uint8_t data)
{
    i2c1_wait_idle();

    I2C1TRN = data;

    while (I2C1STATbits.TRSTAT != 0U)
    {
        ;
    }

    /*
     * ACKSTAT:
     * 0 = ACK recibido
     * 1 = NACK
     */
    return (I2C1STATbits.ACKSTAT == 0U);
}


static uint8_t i2c1_read_nack(void)
{
    uint8_t data;

    i2c1_wait_idle();

    /*
     * Habilitar recepción de un byte.
     */
    I2C1CONbits.RCEN = 1U;

    while (I2C1STATbits.RBF == 0U)
    {
        ;
    }

    data = (uint8_t)I2C1RCV;

    /*
     * Es el único byte que queremos leer:
     * responder NACK para terminar.
     */
    I2C1CONbits.ACKDT = 1U;
    I2C1CONbits.ACKEN = 1U;

    while (I2C1CONbits.ACKEN != 0U)
    {
        ;
    }

    return data;
}


/* -------------------------------------------------------------------------- */
/* Escribir 0xA5 al EP                                                        */
/* -------------------------------------------------------------------------- */

static uint8_t ep_write_ping(void)
{
    uint8_t ok;

    i2c1_start();

    /*
     * Dirección:
     *
     * 0x42 << 1 = 0x84
     * bit R/W = 0 -> escritura
     */
    ok = i2c1_write(
        (uint8_t)(EP_I2C_ADDRESS << 1)
    );

    if (ok != 0U)
    {
        ok = i2c1_write(0xA5U);
    }

    i2c1_stop();

    return ok;
}


/* -------------------------------------------------------------------------- */
/* Leer respuesta del EP                                                      */
/* -------------------------------------------------------------------------- */

static uint8_t ep_read_pong(
    uint8_t *data
)
{
    uint8_t ok;

    i2c1_start();

    /*
     * Dirección:
     *
     * 0x42 << 1 = 0x84
     * R/W = 1
     *
     * byte enviado = 0x85
     */
    ok = i2c1_write(
        (uint8_t)(
            (EP_I2C_ADDRESS << 1)
            | 0x01U
        )
    );

    if (ok != 0U)
    {
        *data = i2c1_read_nack();
    }

    i2c1_stop();

    return ok;
}


/* -------------------------------------------------------------------------- */
/* Main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    uint8_t received;

    clock_init();
    gpio_init();
    i2c1_init();

    __delay_ms(100);

    while (1)
    {
        /*
         * MASTER -> SLAVE
         *
         * Enviar PING.
         */
        if (ep_write_ping() != 0U)
        {
            /*
             * Dar tiempo al main del EP para procesarlo.
             * No es necesario para I2C en sí; simplifica
             * esta primera prueba.
             */
            __delay_ms(5);

            /*
             * SLAVE -> MASTER
             */
            if (ep_read_pong(&received) != 0U)
            {
                if (received == 0x5AU)
                {
                    LED_LAT = 1U;
                    __delay_ms(100);
                    LED_LAT = 0U;
                }
            }
        }

        __delay_ms(600);
    }

    return 0;
}