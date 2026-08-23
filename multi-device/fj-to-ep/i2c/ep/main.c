/*
 * dsPIC33EP32MC204 <-> dsPIC33FJ32MC204
 * I2C bidireccional
 *
 * EP = SLAVE
 *
 * I2C1:
 *   SDA1 -> RC4 / pin 37
 *   SCL1 -> RC5 / pin 38
 *
 * LED:
 *   RB4 / pin 33
 *
 * Dirección I2C:
 *   0x42
 *
 * Funcionamiento:
 *   recibe 0xA5 desde el FJ
 *   conmuta LED
 *   cuando el FJ hace una lectura, responde 0x5A
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>


#define I2C_SLAVE_ADDRESS   0x42U

#define LED_LAT             LATBbits.LATB4
#define LED_TRIS            TRISBbits.TRISB4


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
     * I2C1 estándar del EP:
     *
     * SDA1 = RC4
     * SCL1 = RC5
     */

    TRISCbits.TRISC4 = 1U;
    TRISCbits.TRISC5 = 1U;

    I2C1CON = 0x0000U;
    I2C1STAT = 0x0000U;

    /*
     * Dirección de 7 bits.
     */
    I2C1ADD = I2C_SLAVE_ADDRESS;

    /*
     * Ningún bit de dirección enmascarado.
     */
    I2C1MSK = 0x0000U;

    /*
     * Clock stretching habilitado.
     * Esto permite al software atender el byte
     * antes de liberar nuevamente SCL.
     */
    I2C1CONbits.STREN = 1U;

    IFS1bits.SI2C1IF = 0U;

    I2C1CONbits.I2CEN = 1U;
}


/* -------------------------------------------------------------------------- */
/* Atender eventos Slave                                                      */
/* -------------------------------------------------------------------------- */

static void i2c1_slave_service(void)
{
    uint8_t received;

    if (IFS1bits.SI2C1IF == 0U)
    {
        return;
    }

    /*
     * Limpiar primero el flag del evento actual.
     */
    IFS1bits.SI2C1IF = 0U;

    /*
     * Recuperarse de un posible overflow RX.
     */
    if (I2C1STATbits.I2COV != 0U)
    {
        I2C1STATbits.I2COV = 0U;
    }

    /*
     * D_A = 0:
     * acabamos de recibir una DIRECCIÓN.
     */
    if (I2C1STATbits.D_A == 0U)
    {
        /*
         * Leer I2C1RCV elimina la dirección
         * del receive buffer.
         */
        if (I2C1STATbits.RBF != 0U)
        {
            received = (uint8_t)I2C1RCV;
            (void)received;
        }

        /*
         * R_W = 0:
         * MASTER va a escribir datos al EP.
         */
        if (I2C1STATbits.R_W == 0U)
        {
            /*
             * Liberar SCL y esperar el byte.
             */
            I2C1CONbits.SCLREL = 1U;
        }

        /*
         * R_W = 1:
         * MASTER quiere leer del EP.
         */
        else
        {
            /*
             * Respuesta PONG.
             */
            I2C1TRN = 0x5AU;

            /*
             * Liberar el reloj para que el FJ
             * pueda clockear el byte.
             */
            I2C1CONbits.SCLREL = 1U;
        }

        return;
    }


    /*
     * D_A = 1:
     * evento asociado a DATA.
     *
     * Aquí nos interesa una escritura del master.
     */
    if (
        (I2C1STATbits.R_W == 0U) &&
        (I2C1STATbits.RBF != 0U)
    )
    {
        received = (uint8_t)I2C1RCV;

        /*
         * PING correcto.
         */
        if (received == 0xA5U)
        {
            LED_LAT ^= 1U;
        }

        /*
         * Si STREN mantiene SCL,
         * liberarlo.
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