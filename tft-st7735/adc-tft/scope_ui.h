#ifndef SCOPE_UI_H
#define SCOPE_UI_H

#include <stdint.h>

typedef enum
{
    SCOPE_SCALE_FULL = 0,
    SCOPE_SCALE_X2,
    SCOPE_SCALE_X4,
    SCOPE_SCALE_AUTO,
    SCOPE_SCALE_COUNT
} scope_scale_t;

typedef enum
{
    SCOPE_TRIGGER_RISING = 0,
    SCOPE_TRIGGER_FALLING,
    SCOPE_TRIGGER_FREE,
    SCOPE_TRIGGER_COUNT
} scope_trigger_t;

void scope_ui_init(void);

void scope_ui_draw(
    const uint16_t *samples,
    uint16_t start_index,
    uint32_t sample_rate_hz,
    scope_scale_t scale_mode,
    scope_trigger_t trigger_mode,
    uint8_t running
);

void scope_ui_draw_status(
    uint32_t sample_rate_hz,
    scope_scale_t scale_mode,
    scope_trigger_t trigger_mode,
    uint8_t running
);

#endif