/*
 * ============================================================================
 * dsPIC33EP32MC204 - Prueba simultanea de todos los GPIO
 * ============================================================================
 *
 * Objetivo:
 *   Verificar rapidamente la conectividad de los GPIO de la tarjeta.
 *   Todos los GPIO utilizables cambian de estado AL MISMO TIEMPO:
 *
 *       500 ms HIGH
 *       500 ms LOW
 *       repetir
 *
 * Esto permite usar un solo LED y moverlo de pin en pin sin esperar una
 * secuencia individual para cada GPIO.
 *
 * MCU:   dsPIC33EP32MC204
 * Clock: cristal externo de 8 MHz + PLL
 *        FOSC = 80 MHz
 *        FCY  = 40 MHz (40 MIPS)
 *
 * GPIO probados: 33
 *
 * No se prueban:
 *   6  = VSS
 *   7  = VCAP
 *   16 = AVSS
 *   17 = AVDD
 *   18 = MCLR
 *   28 = VDD
 *   29 = VSS
 *   30 = OSC1 / RA2  (cristal)
 *   31 = OSC2 / RA3  (cristal)
 *   39 = VSS
 *   40 = VDD
 *
 * IMPORTANTE:
 *   El dsPIC33EP utiliza PGD3/PGC3 para ICSP en esta tarjeta:
 *
 *       pin 21 = RB0 / PGED3
 *       pin 22 = RB1 / PGEC3
 *
 *   Estos pines tambien se hacen parpadear. Programa la tarjeta y desconecta
 *   el PICkit antes de realizar la comprobacion completa.
 *
 * LED recomendado:
 *
 *       GPIO ---- 1k ----|>|---- GND
 *
 * ============================================================================
 */

#define FCY 40000000UL

#include <xc.h>
#include <stdint.h>
#include <libpic30.h>


/* ========================================================================== */
/* Configuration Bits                                                         */
/* ========================================================================== */

#pragma config ICS = PGD3       /* ICSP por RB0/RB1 */
#pragma config JTAGEN = OFF
#pragma config ALTI2C1 = OFF
#pragma config ALTI2C2 = OFF
#pragma config PLLKEN = ON
#pragma config FWDTEN = OFF

#pragma config POSCMD = XT       /* Cristal externo de 8 MHz */
#pragma config FNOSC = PRIPLL    /* Primary oscillator + PLL */
#pragma config IESO = OFF
#pragma config OSCIOFNC = OFF
#pragma config IOL1WAY = OFF
#pragma config FCKSM = CSDCMD


/* ========================================================================== */
/* Mascaras de GPIO                                                           */
/* ========================================================================== */

/*
 * PORTA utilizados como GPIO:
 *
 * RA0, RA1, RA4, RA7, RA8, RA9, RA10
 *
 * RA2 y RA3 NO se incluyen porque pertenecen al cristal externo.
 */
#define GPIO_MASK_A    0x0793U

/* RB0 ... RB15 son GPIO utilizables en esta prueba. */
#define GPIO_MASK_B    0xFFFFU

/* RC0 ... RC9 son GPIO utilizables en esta prueba. */
#define GPIO_MASK_C    0x03FFU


/* ========================================================================== */
/* Clock                                                                       */
/* ========================================================================== */

static void clock_init(void)
{
    /*
     * FIN = 8 MHz
     * N1  = 2
     * M   = 40
     * N2  = 2
     *
     * FOSC = 8 MHz / 2 * 40 / 2 = 80 MHz
     * FCY  = FOSC / 2            = 40 MHz
     */

    PLLFBD = 38U;              /* M = PLLFBD + 2 = 40 */
    CLKDIVbits.PLLPRE = 0U;    /* N1 = 2 */
    CLKDIVbits.PLLPOST = 0U;   /* N2 = 2 */

    while (OSCCONbits.LOCK == 0U)
    {
        ;
    }
}


/* ========================================================================== */
/* GPIO                                                                        */
/* ========================================================================== */

static void gpio_init(void)
{
    /* Todos los pines analogicos pasan a funcionamiento digital. */
    ANSELA = 0x0000U;
    ANSELB = 0x0000U;
    ANSELC = 0x0000U;

    /* Estado inicial LOW antes de habilitar las salidas. */
    LATA = 0x0000U;
    LATB = 0x0000U;
    LATC = 0x0000U;

    /*
     * Configurar como salida solamente los GPIO validos del encapsulado.
     *
     * TRIS bit = 0 -> salida
     * TRIS bit = 1 -> entrada
     */
    TRISA = (uint16_t)(~GPIO_MASK_A);
    TRISB = (uint16_t)(~GPIO_MASK_B);
    TRISC = (uint16_t)(~GPIO_MASK_C);
}


static void gpio_all_high(void)
{
    /* Todos los GPIO comprobables pasan a HIGH simultaneamente. */
    LATA = GPIO_MASK_A;
    LATB = GPIO_MASK_B;
    LATC = GPIO_MASK_C;
}


static void gpio_all_low(void)
{
    /* Todos los GPIO comprobables pasan a LOW simultaneamente. */
    LATA = 0x0000U;
    LATB = 0x0000U;
    LATC = 0x0000U;
}


/* ========================================================================== */
/* Main                                                                        */
/* ========================================================================== */

int main(void)
{
    clock_init();
    gpio_init();

    while (1)
    {
        gpio_all_high();
        __delay_ms(500);

        gpio_all_low();
        __delay_ms(500);
    }

    return 0;
}
