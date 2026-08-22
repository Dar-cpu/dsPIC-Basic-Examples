/*
 * dsPIC33FJ32MC204 + ST7735
 * Mini-osciloscopio ADC con 4 botones
 *
 * Clock:
 *   cristal 8 MHz
 *   FOSC = 80 MHz
 *   FCY  = 40 MHz
 *
 * ADC:
 *   AN6 / RC0 / pin 25
 *   12 bits
 *   Fs seleccionable
 *
 * Botones:
 *
 *   RB4 -> TIME
 *   RA8 -> SCALE
 *   RC2 -> TRIGGER
 *   RB2 -> RUN / HOLD
 *
 * TFT:
 *   ST7735 128x160
 *   SPI1 10 MHz
 */

#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "adc_scope.h"
#include "buttons.h"
#include "scope_ui.h"
#include "st7735.h"


#define DISPLAY_SAMPLES       128U

#define TRIGGER_SEARCH_LIMIT \
    (ADC_SCOPE_BUFFER_SIZE - DISPLAY_SAMPLES)


static const uint32_t g_sample_rates[] =
{
    50000UL,
    100000UL,
    200000UL,
    250000UL
};

#define SAMPLE_RATE_COUNT \
    ((uint8_t)(sizeof(g_sample_rates) / sizeof(g_sample_rates[0])))


/* -------------------------------------------------------------------------- */
/* Clock                                                                       */
/* -------------------------------------------------------------------------- */

static void clock_init(void)
{
    /*
     * 8 MHz / N1=2 * M=40 / N2=2
     *
     * FOSC = 80 MHz
     * FCY  = 40 MHz
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
/* Trigger                                                                     */
/* -------------------------------------------------------------------------- */

static uint16_t find_trigger(
    const uint16_t *samples,
    scope_trigger_t mode
)
{
    uint16_t i;
    uint16_t min_value;
    uint16_t max_value;
    uint16_t threshold;

    if (mode == SCOPE_TRIGGER_FREE)
    {
        return 0U;
    }

    min_value = ADC_SCOPE_MAX_VALUE;
    max_value = 0U;

    /*
     * Nivel de trigger = centro entre minimo y maximo
     * de toda la captura.
     */
    for (i = 0U; i < ADC_SCOPE_BUFFER_SIZE; i++)
    {
        if (samples[i] < min_value)
        {
            min_value = samples[i];
        }

        if (samples[i] > max_value)
        {
            max_value = samples[i];
        }
    }

    threshold =
        (uint16_t)(
            ((uint32_t)min_value
            + (uint32_t)max_value)
            / 2UL
        );

    for (i = 1U; i <= TRIGGER_SEARCH_LIMIT; i++)
    {
        if (mode == SCOPE_TRIGGER_RISING)
        {
            if ((samples[i - 1U] < threshold) &&
                (samples[i] >= threshold))
            {
                return i;
            }
        }
        else
        {
            if ((samples[i - 1U] > threshold) &&
                (samples[i] <= threshold))
            {
                return i;
            }
        }
    }

    /*
     * Si no aparece un cruce valido, usamos comienzo del buffer.
     */
    return 0U;
}


/* -------------------------------------------------------------------------- */
/* Main                                                                        */
/* -------------------------------------------------------------------------- */

int main(void)
{
    const uint16_t *samples = 0;

    uint16_t trigger_index = 0U;

    uint8_t events;
    uint8_t running = 1U;
    uint8_t redraw = 1U;

    uint8_t sample_rate_index = 2U;

    scope_scale_t scale_mode =
        SCOPE_SCALE_FULL;

    scope_trigger_t trigger_mode =
        SCOPE_TRIGGER_RISING;

    clock_init();

    st7735_init();
    adc_scope_init();
    buttons_init();

    adc_scope_set_sample_rate(
        g_sample_rates[sample_rate_index]
    );

    scope_ui_init();

    while (1)
    {
        /*
         * Los eventos quedan latched por Timer1.
         * Aunque el TFT tarde en redibujar, no se pierde
         * una pulsacion normal.
         */
        events = buttons_get_events();

        /* -------------------------------------------------------------- */
        /* RB4: TIME                                                      */
        /* -------------------------------------------------------------- */

        if ((events & BUTTON_EVENT_TIME) != 0U)
        {
            sample_rate_index++;

            if (sample_rate_index >= SAMPLE_RATE_COUNT)
            {
                sample_rate_index = 0U;
            }

            adc_scope_set_sample_rate(
                g_sample_rates[sample_rate_index]
            );

            redraw = 1U;
        }

        /* -------------------------------------------------------------- */
        /* RA8: SCALE                                                     */
        /* -------------------------------------------------------------- */

        if ((events & BUTTON_EVENT_SCALE) != 0U)
        {
            scale_mode =
                (scope_scale_t)(
                    (uint8_t)scale_mode + 1U
                );

            if (scale_mode >= SCOPE_SCALE_COUNT)
            {
                scale_mode = SCOPE_SCALE_FULL;
            }

            redraw = 1U;
        }

        /* -------------------------------------------------------------- */
        /* RC2: TRIGGER                                                   */
        /* -------------------------------------------------------------- */

        if ((events & BUTTON_EVENT_TRIGGER) != 0U)
        {
            trigger_mode =
                (scope_trigger_t)(
                    (uint8_t)trigger_mode + 1U
                );

            if (trigger_mode >= SCOPE_TRIGGER_COUNT)
            {
                trigger_mode =
                    SCOPE_TRIGGER_RISING;
            }

            /*
             * En HOLD podemos recalcular el trigger sobre
             * la captura congelada.
             */
            if (samples != 0)
            {
                trigger_index =
                    find_trigger(
                        samples,
                        trigger_mode
                    );
            }

            redraw = 1U;
        }

        /* -------------------------------------------------------------- */
        /* RB2: RUN / HOLD                                                */
        /* -------------------------------------------------------------- */

        if ((events & BUTTON_EVENT_RUN) != 0U)
        {
            running =
                (running == 0U) ? 1U : 0U;

            redraw = 1U;
        }

        /* -------------------------------------------------------------- */
        /* RUN                                                            */
        /* -------------------------------------------------------------- */

        if (running != 0U)
        {
            adc_scope_capture();

            samples =
                adc_scope_get_buffer();

            trigger_index =
                find_trigger(
                    samples,
                    trigger_mode
                );

            scope_ui_draw(
                samples,
                trigger_index,
                adc_scope_get_sample_rate(),
                scale_mode,
                trigger_mode,
                1U
            );

            /*
             * En RUN cada nueva captura necesita redibujado.
             */
            redraw = 0U;
        }

        /* -------------------------------------------------------------- */
        /* HOLD                                                           */
        /* -------------------------------------------------------------- */

        else
        {
            if ((redraw != 0U) &&
                (samples != 0))
            {
                /*
                 * SCALE y TRIGGER siguen funcionando sobre
                 * la captura congelada.
                 */
                trigger_index =
                    find_trigger(
                        samples,
                        trigger_mode
                    );

                scope_ui_draw(
                    samples,
                    trigger_index,
                    adc_scope_get_sample_rate(),
                    scale_mode,
                    trigger_mode,
                    0U
                );

                redraw = 0U;
            }

            /*
             * Mantener el CPU disponible para Timer1.
             */
            __delay_ms(2);
        }
    }

    return 0;
}