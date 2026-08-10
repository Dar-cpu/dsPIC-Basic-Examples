#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "adc_scope.h"

/*
 * ADC 12 bits:
 *
 * FCY = 40 MHz -> TCY = 25 ns
 *
 * ADCS = 5:
 *   TAD = 25 ns * (5 + 1)
 *       = 150 ns
 *
 * Conversion 12-bit:
 *   14 TAD = 2.10 us
 *
 * Las Fs usadas por este ejemplo son:
 *
 *   50 kS/s  -> PR3 = 799
 *  100 kS/s  -> PR3 = 399
 *  200 kS/s  -> PR3 = 199
 *  250 kS/s  -> PR3 = 159
 */

#define ADC_SCOPE_DEFAULT_FS   200000UL

static volatile uint16_t g_adc_buffer[ADC_SCOPE_BUFFER_SIZE];
static volatile uint16_t g_adc_index = 0U;
static volatile uint8_t g_capture_done = 0U;

static volatile uint32_t g_sample_rate_hz = ADC_SCOPE_DEFAULT_FS;


/* -------------------------------------------------------------------------- */
/* ADC interrupt                                                               */
/* -------------------------------------------------------------------------- */

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
    uint16_t sample;

    sample = ADC1BUF0;

    if (g_adc_index < ADC_SCOPE_BUFFER_SIZE)
    {
        g_adc_buffer[g_adc_index] = sample;
        g_adc_index++;
    }

    if (g_adc_index >= ADC_SCOPE_BUFFER_SIZE)
    {
        /*
         * La captura ya esta completa.
         * Detenemos Timer3 para impedir nuevos triggers.
         */
        T3CONbits.TON = 0U;
        AD1CON1bits.ASAM = 0U;

        g_capture_done = 1U;
    }

    IFS0bits.AD1IF = 0U;
}


/* -------------------------------------------------------------------------- */
/* Sample rate                                                                 */
/* -------------------------------------------------------------------------- */

void adc_scope_set_sample_rate(uint32_t sample_rate_hz)
{
    uint32_t period;

    /*
     * Para este ejemplo solo se usan frecuencias que dividen FCY
     * exactamente y caben en Timer3 de 16 bits.
     */
    if (sample_rate_hz == 0UL)
    {
        return;
    }

    period = FCY / sample_rate_hz;

    if ((period == 0UL) || (period > 65536UL))
    {
        return;
    }

    T3CONbits.TON = 0U;
    TMR3 = 0U;

    PR3 = (uint16_t)(period - 1UL);

    g_sample_rate_hz = sample_rate_hz;
}


uint32_t adc_scope_get_sample_rate(void)
{
    return g_sample_rate_hz;
}


/* -------------------------------------------------------------------------- */
/* Inicializacion                                                              */
/* -------------------------------------------------------------------------- */

void adc_scope_init(void)
{
    /*
     * ADC apagado mientras se configura.
     */
    AD1CON1 = 0x0000U;
    AD1CON2 = 0x0000U;
    AD1CON3 = 0x0000U;
    AD1CHS0 = 0x0000U;
    AD1CSSL = 0x0000U;

    /*
     * Todos los ANx quedan digitales excepto AN6.
     *
     * Esto tambien deja RB2/AN4 y RC2/AN8 como GPIO digital,
     * necesarios para dos de los botones.
     */
    AD1PCFGL = 0xFFFFU;
    AD1PCFGLbits.PCFG6 = 0U;

    /*
     * AN6 / RC0 como entrada.
     */
    TRISCbits.TRISC0 = 1U;

    /*
     * 12 bits.
     * Resultado unsigned integer.
     */
    AD1CON1bits.AD12B = 1U;
    AD1CON1bits.FORM = 0U;

    /*
     * SSRC = 010:
     * Timer3 termina el sample y comienza la conversion.
     */
    AD1CON1bits.SSRC = 2U;

    /*
     * Un canal.
     * AVDD / AVSS.
     * Interrupcion despues de cada conversion.
     */
    AD1CON2bits.VCFG = 0U;
    AD1CON2bits.CHPS = 0U;
    AD1CON2bits.CSCNA = 0U;
    AD1CON2bits.SMPI = 0U;
    AD1CON2bits.ALTS = 0U;

    /*
     * TAD = 150 ns.
     */
    AD1CON3bits.ADRC = 0U;
    AD1CON3bits.ADCS = 5U;

    /*
     * CH0+ = AN6
     * CH0- = AVSS
     */
    AD1CHS0bits.CH0SA = ADC_SCOPE_CHANNEL;
    AD1CHS0bits.CH0NA = 0U;

    /*
     * Timer3:
     * clock interno, prescaler 1:1.
     */
    T3CON = 0x0000U;
    T3CONbits.TCKPS = 0U;

    adc_scope_set_sample_rate(
        ADC_SCOPE_DEFAULT_FS
    );

    /*
     * Timer3 no necesita ISR.
     */
    IEC0bits.T3IE = 0U;
    IFS0bits.T3IF = 0U;

    /*
     * ADC ISR.
     */
    IFS0bits.AD1IF = 0U;
    IPC3bits.AD1IP = 5U;
    IEC0bits.AD1IE = 1U;

    /*
     * Encender ADC.
     */
    AD1CON1bits.ADON = 1U;
    __delay_us(20);
}


/* -------------------------------------------------------------------------- */
/* Captura                                                                     */
/* -------------------------------------------------------------------------- */

void adc_scope_capture(void)
{
    T3CONbits.TON = 0U;
    TMR3 = 0U;

    g_adc_index = 0U;
    g_capture_done = 0U;

    IFS0bits.AD1IF = 0U;

    /*
     * Sample automatico.
     * Timer3 controla cuando termina cada sample.
     */
    AD1CON1bits.ASAM = 1U;
    AD1CON1bits.SAMP = 1U;

    T3CONbits.TON = 1U;

    while (g_capture_done == 0U)
    {
        ;
    }
}


const uint16_t *adc_scope_get_buffer(void)
{
    return (const uint16_t *)g_adc_buffer;
}