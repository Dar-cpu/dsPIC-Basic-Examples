#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

/*
 * Botones del osciloscopio:
 *
 *   TIME    -> RB4 / CN1
 *   SCALE   -> RB3 / CN7   
 *   TRIGGER -> RC2 / CN10
 *   RUN     -> RB2 / CN6
 *
 * Todos se usan activos en LOW:
 *
 *   GPIO ---- pulsador ---- GND
 *
 * Pull-ups:
 *   RB4 -> interno CN1
 *   RC2 -> interno CN10
 *   RB2 -> interno CN6
 *   RB3 -> INTERNO CN7
 */
#define BUTTON_ACTIVE_LOW        1
#define BUTTON_DEBOUNCE_MS       20U

#define BUTTON_EVENT_TIME        0x01U
#define BUTTON_EVENT_SCALE       0x02U
#define BUTTON_EVENT_TRIGGER     0x04U
#define BUTTON_EVENT_RUN         0x08U

void buttons_init(void);

/*
 * Devuelve eventos de pulsacion ya filtrados por debounce.
 * Cada pulsacion se entrega una sola vez.
 */
uint8_t buttons_get_events(void);

#endif