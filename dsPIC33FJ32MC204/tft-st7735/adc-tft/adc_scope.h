#ifndef ADC_SCOPE_H
#define ADC_SCOPE_H

#include <stdint.h>

/*
 * ADC:
 *   AN6 / RC0 / pin fisico 25
 *   12 bits
 *   VREF+ = AVDD
 *   VREF- = AVSS
 *
 * Muestreo:
 *   Timer3 -> trigger ADC
 *   Fs = 200 kS/s
 */
#define ADC_SCOPE_CHANNEL          6U
#define ADC_SCOPE_SAMPLE_RATE_HZ   200000UL
#define ADC_SCOPE_BUFFER_SIZE      256U
#define ADC_SCOPE_MAX_VALUE        4095U

void adc_scope_init(void);

/*
 * Captura ADC_SCOPE_BUFFER_SIZE muestras.
 * La funcion bloquea aproximadamente 1.28 ms a 200 kS/s.
 */
void adc_scope_capture(void);

/*
 * Devuelve el buffer de la ultima captura.
 */
const uint16_t *adc_scope_get_buffer(void);

#endif
