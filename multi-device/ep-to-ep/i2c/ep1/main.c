/*
 * dsPIC33EP32MC204 <-> dsPIC33EP32MC204
 * I2C1 bidireccional
 *
 * EP1 = MASTER
 *
 * SDA1 -> RC4 / pin 37
 * SCL1 -> RC5 / pin 38
 * LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * I2C1:
 *   ~100 kHz
 *
 * EP2 slave:
 *   direccion 7-bit = 0x42
 *
 * Funcionamiento:
 *   EP1 escribe 0xA5
 *   EP2 responde 0x5A cuando EP1 realiza una lectura
 *
 * Si EP1 recibe 0x5A:
 *   LED RB4 prende durante 100 ms
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


#define EP2_ADDRESS    0x42U

#define LED_LAT        LATBbits.LATB4
#define LED_TRIS       TRISBbits.TRISB4


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
}


/* -------------------------------------------------------------------------- */
/* I2C1 Master                                                                */
/* -------------------------------------------------------------------------- */

static void i2c1_init(void)
{
    /*
     * I2C1 estándar:
     *
     * SDA1 = RC4
     * SCL1 = RC5
     */

    TRISCbits.TRISC4 = 1U;
    TRISCbits.TRISC5 = 1U;

    I2C1CON = 0x0000U;
    I2C1STAT = 0x0000U;

    /*
     * FCY = 40 MHz
     * aproximadamente 100 kHz
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
     * Habilitar recepción.
     */
    I2C1CONbits.RCEN = 1U;

    while (I2C1STATbits.RBF == 0U)
    {
        ;
    }

    data = (uint8_t)(I2C1RCV & 0x00FFU);

    /*
     * Solo queremos un byte.
     *
     * NACK para terminar la lectura.
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
/* EP1 -> EP2                                                                 */
/* -------------------------------------------------------------------------- */

static uint8_t ep2_write_ping(void)
{
    uint8_t ok;

    i2c1_start();

    /*
     * 0x42 << 1 = 0x84
     *
     * R/W = 0
     */
    ok = i2c1_write(
        (uint8_t)(EP2_ADDRESS << 1)
    );

    if (ok != 0U)
    {
        ok = i2c1_write(0xA5U);
    }

    i2c1_stop();

    return ok;
}


/* -------------------------------------------------------------------------- */
/* EP2 -> EP1                                                                 */
/* -------------------------------------------------------------------------- */

static uint8_t ep2_read_pong(uint8_t *data)
{
    uint8_t ok;

    i2c1_start();

    /*
     * Dirección de lectura:
     *
     * 0x42 << 1 = 0x84
     * R/W = 1
     *
     * byte = 0x85
     */
    ok = i2c1_write(
        (uint8_t)(
            (EP2_ADDRESS << 1)
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
         * EP1 -> EP2
         *
         * enviar 0xA5
         */
        if (ep2_write_ping() != 0U)
        {
            /*
             * Pequeña separación para hacer
             * la prueba fácil de observar.
             */
            __delay_ms(5);

            /*
             * EP2 -> EP1
             *
             * solicitar respuesta.
             */
            if (ep2_read_pong(&received) != 0U)
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