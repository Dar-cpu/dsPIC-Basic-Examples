#define FCY 40000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "buttons.h"

/*
 * Timer1 genera un tick de 1 ms:
 *
 * FCY = 40 MHz
 * prescaler = 1:64
 *
 * 40 MHz / 64 = 625 kHz
 *
 * PR1 = 624:
 * 625 cuentas = 1 ms
 */

#define BTN_TIME_PORT       PORTBbits.RB4
#define BTN_SCALE_PORT      PORTBbits.RB3
#define BTN_TRIGGER_PORT    PORTCbits.RC2
#define BTN_RUN_PORT        PORTBbits.RB2

/*
 * Estado estable:
 *   0 = released
 *   1 = pressed
 */
static volatile uint8_t g_events = 0U;

static volatile uint8_t g_state_time = 0U;
static volatile uint8_t g_state_scale = 0U;
static volatile uint8_t g_state_trigger = 0U;
static volatile uint8_t g_state_run = 0U;

static volatile uint8_t g_count_time = 0U;
static volatile uint8_t g_count_scale = 0U;
static volatile uint8_t g_count_trigger = 0U;
static volatile uint8_t g_count_run = 0U;


/* -------------------------------------------------------------------------- */
/* Lectura electrica                                                          */
/* -------------------------------------------------------------------------- */

static uint8_t button_level_to_pressed(uint8_t level)
{
#if BUTTON_ACTIVE_LOW
    return (level == 0U) ? 1U : 0U;
#else
    return (level != 0U) ? 1U : 0U;
#endif
}


/* -------------------------------------------------------------------------- */
/* Debounce                                                                    */
/* -------------------------------------------------------------------------- */

static void debounce_one(
    uint8_t raw_pressed,
    volatile uint8_t *stable_state,
    volatile uint8_t *counter,
    uint8_t event_mask
)
{
    /*
     * Si la lectura coincide con el estado ya aceptado,
     * no hay nada que filtrar.
     */
    if (raw_pressed == *stable_state)
    {
        *counter = 0U;
        return;
    }

    /*
     * La lectura nueva debe permanecer estable durante
     * BUTTON_DEBOUNCE_MS milisegundos.
     */
    if (*counter < BUTTON_DEBOUNCE_MS)
    {
        (*counter)++;
    }

    if (*counter >= BUTTON_DEBOUNCE_MS)
    {
        *counter = 0U;
        *stable_state = raw_pressed;

        /*
         * Evento solamente en el flanco RELEASED -> PRESSED.
         */
        if (raw_pressed != 0U)
        {
            g_events |= event_mask;
        }
    }
}


/* -------------------------------------------------------------------------- */
/* Timer1 ISR                                                                  */
/* -------------------------------------------------------------------------- */

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    debounce_one(
        button_level_to_pressed((uint8_t)BTN_TIME_PORT),
        &g_state_time,
        &g_count_time,
        BUTTON_EVENT_TIME
    );

    debounce_one(
        button_level_to_pressed((uint8_t)BTN_SCALE_PORT),
        &g_state_scale,
        &g_count_scale,
        BUTTON_EVENT_SCALE
    );

    debounce_one(
        button_level_to_pressed((uint8_t)BTN_TRIGGER_PORT),
        &g_state_trigger,
        &g_count_trigger,
        BUTTON_EVENT_TRIGGER
    );

    debounce_one(
        button_level_to_pressed((uint8_t)BTN_RUN_PORT),
        &g_state_run,
        &g_count_run,
        BUTTON_EVENT_RUN
    );

    IFS0bits.T1IF = 0U;
}


/* -------------------------------------------------------------------------- */
/* Init                                                                        */
/* -------------------------------------------------------------------------- */

void buttons_init(void)
{
    /* Entradas */
    TRISBbits.TRISB4 = 1U;   /* TIME    */
    TRISBbits.TRISB3 = 1U;   /* SCALE   */
    TRISCbits.TRISC2 = 1U;   /* TRIGGER */
    TRISBbits.TRISB2 = 1U;   /* RUN     */

    /*
     * Pines con funcion analogica usados como botones:
     *
     * RB3 = AN5
     * RB2 = AN4
     * RC2 = AN8
     *
     * Forzarlos a digital.
     */
    AD1PCFGLbits.PCFG5 = 1U;
    AD1PCFGLbits.PCFG4 = 1U;
    AD1PCFGLbits.PCFG8 = 1U;

    /*
     * Pull-ups internos:
     *
     * RB4 = CN1
     * RB3 = CN7
     * RC2 = CN10
     * RB2 = CN6
     */
    CNPU1bits.CN1PUE  = 1U;
    CNPU1bits.CN7PUE  = 1U;
    CNPU1bits.CN10PUE = 1U;
    CNPU1bits.CN6PUE  = 1U;

    __delay_us(50);

    /*
     * Leer estado inicial real.
     */
    g_state_time =
        button_level_to_pressed((uint8_t)BTN_TIME_PORT);

    g_state_scale =
        button_level_to_pressed((uint8_t)BTN_SCALE_PORT);

    g_state_trigger =
        button_level_to_pressed((uint8_t)BTN_TRIGGER_PORT);

    g_state_run =
        button_level_to_pressed((uint8_t)BTN_RUN_PORT);

    g_count_time = 0U;
    g_count_scale = 0U;
    g_count_trigger = 0U;
    g_count_run = 0U;
    g_events = 0U;

    /* Timer1 = 1 ms */
    T1CON = 0x0000U;
    T1CONbits.TCKPS = 2U;

    TMR1 = 0U;
    PR1 = 624U;

    IFS0bits.T1IF = 0U;
    IPC0bits.T1IP = 2U;
    IEC0bits.T1IE = 1U;

    T1CONbits.TON = 1U;
}


/* -------------------------------------------------------------------------- */
/* Eventos                                                                     */
/* -------------------------------------------------------------------------- */

uint8_t buttons_get_events(void)
{
    uint8_t events;

    /*
     * Seccion critica muy corta:
     * Timer1 no puede modificar g_events mientras main lo lee y limpia.
     */
    IEC0bits.T1IE = 0U;

    events = g_events;
    g_events = 0U;

    IEC0bits.T1IE = 1U;

    return events;
}