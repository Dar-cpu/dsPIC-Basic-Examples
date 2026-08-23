/*
 * dsPIC33EP32MC204 <-> dsPIC33EP32MC204
 * I2C1 bidireccional
 *
 * EP2 = SLAVE
 *
 * SDA1 -> RC4 / pin 37
 * SCL1 -> RC5 / pin 38
 * LED  -> RB4 / pin 33
 *
 * Cristal: 8 MHz
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 *
 * Dirección I2C:
 *   0x42
 *
 * Funcionamiento:
 *
 * EP1 escribe 0xA5.
 *
 * Si EP2 recibe 0xA5:
 *   cambia el estado del LED.
 *
 * Cuando EP1 solicita una lectura:
 *   EP2 responde 0x5A.
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


#define I2C_SLAVE_ADDRESS    0x42U

#define LED_LAT              LATBbits.LATB4
#define LED_TRIS             TRISBbits.TRISB4


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
/* I2C1 Slave                                                                 */
/* -------------------------------------------------------------------------- */

static void i2c1_slave_init(void)
{
    /*
     * SDA1 = RC4
     * SCL1 = RC5
     */

    TRISCbits.TRISC4 = 1U;
    TRISCbits.TRISC5 = 1U;

    I2C1CON = 0x0000U;
    I2C1STAT = 0x0000U;

    /*
     * Dirección 7-bit.
     */
    I2C1ADD = I2C_SLAVE_ADDRESS;

    /*
     * Sin máscara.
     */
    I2C1MSK = 0x0000U;

    /*
     * Permitir clock stretching.
     */
    I2C1CONbits.STREN = 1U;

    IFS1bits.SI2C1IF = 0U;

    I2C1CONbits.I2CEN = 1U;
}


/* -------------------------------------------------------------------------- */
/* Slave service                                                              */
/* -------------------------------------------------------------------------- */

static void i2c1_slave_service(void)
{
    uint8_t received;

    /*
     * ¿Ocurrió un evento I2C?
     */
    if (IFS1bits.SI2C1IF == 0U)
    {
        return;
    }

    IFS1bits.SI2C1IF = 0U;


    /*
     * Recuperarse de overflow.
     */
    if (I2C1STATbits.I2COV != 0U)
    {
        I2C1STATbits.I2COV = 0U;
    }


    /*
     * D_A = 0:
     *
     * acaba de llegar ADDRESS.
     */
    if (I2C1STATbits.D_A == 0U)
    {
        /*
         * Vaciar receive buffer.
         */
        if (I2C1STATbits.RBF != 0U)
        {
            received =
                (uint8_t)(I2C1RCV & 0x00FFU);

            (void)received;
        }


        /*
         * R_W = 0
         *
         * Master va a escribir.
         */
        if (I2C1STATbits.R_W == 0U)
        {
            I2C1CONbits.SCLREL = 1U;
        }

        /*
         * R_W = 1
         *
         * Master quiere leer.
         */
        else
        {
            /*
             * Preparar respuesta.
             */
            I2C1TRN = 0x5AU;

            /*
             * Liberar SCL.
             */
            I2C1CONbits.SCLREL = 1U;
        }

        return;
    }


    /*
     * D_A = 1:
     *
     * acaba de llegar DATA.
     */
    if (
        (I2C1STATbits.R_W == 0U) &&
        (I2C1STATbits.RBF != 0U)
    )
    {
        received =
            (uint8_t)(I2C1RCV & 0x00FFU);

        /*
         * PING correcto.
         */
        if (received == 0xA5U)
        {
            LED_LAT ^= 1U;
        }

        /*
         * Liberar reloj.
         */
        I2C1CONbits.SCLREL = 1U;
    }
}


/* -------------------------------------------------------------------------- */
/* Main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    clock_init();
    gpio_init();
    i2c1_slave_init();

    __delay_ms(100);

    while (1)
    {
        i2c1_slave_service();
    }

    return 0;
}