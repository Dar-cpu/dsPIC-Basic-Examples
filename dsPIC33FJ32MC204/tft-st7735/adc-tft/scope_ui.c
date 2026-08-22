#include <stdint.h>

#include "adc_scope.h"
#include "scope_ui.h"
#include "st7735.h"

#define DISPLAY_SAMPLES     128U

#define STATUS_TOP          0U
#define STATUS_HEIGHT       10U

#define PLOT_TOP            10U
#define PLOT_BOTTOM         (ST7735_HEIGHT - 1U)
#define PLOT_HEIGHT         (ST7735_HEIGHT - PLOT_TOP)


/* -------------------------------------------------------------------------- */
/* Tiny 5x7 font                                                               */
/* -------------------------------------------------------------------------- */

/*
 * Solo contiene los caracteres que necesita la barra de estado.
 * Cada byte es una columna de 7 pixeles, bit 0 = fila superior.
 */

static uint8_t glyph_column(char c, uint8_t column)
{
    static const uint8_t digits[10][5] =
    {
        {0x3EU, 0x51U, 0x49U, 0x45U, 0x3EU}, /* 0 */
        {0x00U, 0x42U, 0x7FU, 0x40U, 0x00U}, /* 1 */
        {0x42U, 0x61U, 0x51U, 0x49U, 0x46U}, /* 2 */
        {0x21U, 0x41U, 0x45U, 0x4BU, 0x31U}, /* 3 */
        {0x18U, 0x14U, 0x12U, 0x7FU, 0x10U}, /* 4 */
        {0x27U, 0x45U, 0x45U, 0x45U, 0x39U}, /* 5 */
        {0x3CU, 0x4AU, 0x49U, 0x49U, 0x30U}, /* 6 */
        {0x01U, 0x71U, 0x09U, 0x05U, 0x03U}, /* 7 */
        {0x36U, 0x49U, 0x49U, 0x49U, 0x36U}, /* 8 */
        {0x06U, 0x49U, 0x49U, 0x29U, 0x1EU}  /* 9 */
    };

    static const uint8_t A[5] = {0x7EU,0x09U,0x09U,0x09U,0x7EU};
    static const uint8_t D[5] = {0x7FU,0x41U,0x41U,0x22U,0x1CU};
    static const uint8_t E[5] = {0x7FU,0x49U,0x49U,0x49U,0x41U};
    static const uint8_t F[5] = {0x7FU,0x09U,0x09U,0x09U,0x01U};
    static const uint8_t H[5] = {0x7FU,0x08U,0x08U,0x08U,0x7FU};
    static const uint8_t K[5] = {0x7FU,0x08U,0x14U,0x22U,0x41U};
    static const uint8_t L[5] = {0x7FU,0x40U,0x40U,0x40U,0x40U};
    static const uint8_t N[5] = {0x7FU,0x02U,0x04U,0x08U,0x7FU};
    static const uint8_t O[5] = {0x3EU,0x41U,0x41U,0x41U,0x3EU};
    static const uint8_t R[5] = {0x7FU,0x09U,0x19U,0x29U,0x46U};
    static const uint8_t T[5] = {0x01U,0x01U,0x7FU,0x01U,0x01U};
    static const uint8_t U[5] = {0x3FU,0x40U,0x40U,0x40U,0x3FU};
    static const uint8_t X[5] = {0x63U,0x14U,0x08U,0x14U,0x63U};

    const uint8_t *glyph = 0;

    if (column >= 5U)
    {
        return 0U;
    }

    if ((c >= '0') && (c <= '9'))
    {
        return digits[(uint8_t)(c - '0')][column];
    }

    switch (c)
    {
        case 'A': glyph = A; break;
        case 'D': glyph = D; break;
        case 'E': glyph = E; break;
        case 'F': glyph = F; break;
        case 'H': glyph = H; break;
        case 'K': glyph = K; break;
        case 'L': glyph = L; break;
        case 'N': glyph = N; break;
        case 'O': glyph = O; break;
        case 'R': glyph = R; break;
        case 'T': glyph = T; break;
        case 'U': glyph = U; break;
        case 'X': glyph = X; break;
        case ' ': return 0U;
        default:  return 0U;
    }

    return glyph[column];
}


static void draw_char(
    uint16_t x,
    uint16_t y,
    char c,
    uint16_t color
)
{
    uint8_t col;
    uint8_t row;
    uint8_t bits;

    for (col = 0U; col < 5U; col++)
    {
        bits = glyph_column(c, col);

        for (row = 0U; row < 7U; row++)
        {
            if ((bits & (1U << row)) != 0U)
            {
                st7735_draw_pixel(
                    (uint16_t)(x + col),
                    (uint16_t)(y + row),
                    color
                );
            }
        }
    }
}


static void draw_text(
    uint16_t x,
    uint16_t y,
    const char *text,
    uint16_t color
)
{
    while (*text != '\0')
    {
        draw_char(
            x,
            y,
            *text,
            color
        );

        x = (uint16_t)(x + 6U);
        text++;
    }
}


/* -------------------------------------------------------------------------- */
/* Status string                                                               */
/* -------------------------------------------------------------------------- */

static uint8_t append_string(
    char *buffer,
    uint8_t position,
    const char *text
)
{
    while (*text != '\0')
    {
        buffer[position] = *text;
        position++;
        text++;
    }

    return position;
}


static uint8_t append_rate(
    char *buffer,
    uint8_t position,
    uint32_t rate
)
{
    if (rate == 50000UL)
    {
        return append_string(
            buffer,
            position,
            "50K"
        );
    }

    if (rate == 100000UL)
    {
        return append_string(
            buffer,
            position,
            "100K"
        );
    }

    if (rate == 200000UL)
    {
        return append_string(
            buffer,
            position,
            "200K"
        );
    }

    if (rate == 250000UL)
    {
        return append_string(
            buffer,
            position,
            "250K"
        );
    }

    return append_string(
        buffer,
        position,
        "----"
    );
}


void scope_ui_draw_status(
    uint32_t sample_rate_hz,
    scope_scale_t scale_mode,
    scope_trigger_t trigger_mode,
    uint8_t running
)
{
    char line[22];
    uint8_t p = 0U;

    p = append_rate(
        line,
        p,
        sample_rate_hz
    );

    line[p++] = ' ';

    switch (scale_mode)
    {
        case SCOPE_SCALE_FULL:
            p = append_string(line, p, "X1");
            break;

        case SCOPE_SCALE_X2:
            p = append_string(line, p, "X2");
            break;

        case SCOPE_SCALE_X4:
            p = append_string(line, p, "X4");
            break;

        case SCOPE_SCALE_AUTO:
        default:
            p = append_string(line, p, "AUTO");
            break;
    }

    line[p++] = ' ';

    switch (trigger_mode)
    {
        case SCOPE_TRIGGER_RISING:
            p = append_string(line, p, "R");
            break;

        case SCOPE_TRIGGER_FALLING:
            p = append_string(line, p, "F");
            break;

        case SCOPE_TRIGGER_FREE:
        default:
            p = append_string(line, p, "FREE");
            break;
    }

    line[p++] = ' ';

    if (running != 0U)
    {
        p = append_string(
            line,
            p,
            "RUN"
        );
    }
    else
    {
        p = append_string(
            line,
            p,
            "HOLD"
        );
    }

    line[p] = '\0';

    st7735_fill_rect(
        0U,
        STATUS_TOP,
        ST7735_WIDTH,
        STATUS_HEIGHT,
        TFT_COLOR_BLACK
    );

    draw_text(
        1U,
        1U,
        line,
        (running != 0U)
            ? TFT_COLOR_GREEN
            : TFT_COLOR_YELLOW
    );
}


/* -------------------------------------------------------------------------- */
/* Escala                                                                      */
/* -------------------------------------------------------------------------- */

static void get_scale_limits(
    const uint16_t *samples,
    uint16_t start_index,
    scope_scale_t scale_mode,
    uint16_t *low,
    uint16_t *high
)
{
    uint16_t i;
    uint16_t min_value;
    uint16_t max_value;
    uint16_t span;
    uint16_t margin;
    uint32_t temp_low;
    uint32_t temp_high;

    switch (scale_mode)
    {
        case SCOPE_SCALE_FULL:
            *low = 0U;
            *high = ADC_SCOPE_MAX_VALUE;
            return;

        case SCOPE_SCALE_X2:
            *low = 1024U;
            *high = 3072U;
            return;

        case SCOPE_SCALE_X4:
            *low = 1536U;
            *high = 2560U;
            return;

        case SCOPE_SCALE_AUTO:
        default:
            break;
    }

    min_value = ADC_SCOPE_MAX_VALUE;
    max_value = 0U;

    for (i = 0U; i < DISPLAY_SAMPLES; i++)
    {
        uint16_t value =
            samples[start_index + i];

        if (value < min_value)
        {
            min_value = value;
        }

        if (value > max_value)
        {
            max_value = value;
        }
    }

    span = (uint16_t)(
        max_value - min_value
    );

    /*
     * Evita zoom exagerado con una entrada DC/noise.
     */
    if (span < 128U)
    {
        span = 128U;
    }

    margin = (uint16_t)(
        span / 8U
    );

    if (margin < 16U)
    {
        margin = 16U;
    }

    if (min_value > margin)
    {
        temp_low =
            (uint32_t)min_value
            - (uint32_t)margin;
    }
    else
    {
        temp_low = 0UL;
    }

    temp_high =
        (uint32_t)max_value
        + (uint32_t)margin;

    if (temp_high > ADC_SCOPE_MAX_VALUE)
    {
        temp_high =
            ADC_SCOPE_MAX_VALUE;
    }

    if ((temp_high - temp_low) < 128UL)
    {
        temp_high = temp_low + 128UL;

        if (temp_high > ADC_SCOPE_MAX_VALUE)
        {
            temp_high = ADC_SCOPE_MAX_VALUE;

            if (temp_high >= 128UL)
            {
                temp_low = temp_high - 128UL;
            }
        }
    }

    *low = (uint16_t)temp_low;
    *high = (uint16_t)temp_high;
}


static uint16_t adc_to_y(
    uint16_t sample,
    uint16_t low,
    uint16_t high
)
{
    uint32_t scaled;
    uint32_t range;

    if (sample <= low)
    {
        return PLOT_BOTTOM;
    }

    if (sample >= high)
    {
        return PLOT_TOP;
    }

    range =
        (uint32_t)high
        - (uint32_t)low;

    scaled =
        ((uint32_t)(sample - low)
        * (uint32_t)(PLOT_HEIGHT - 1U))
        / range;

    return (uint16_t)(
        PLOT_BOTTOM
        - (uint16_t)scaled
    );
}


/* -------------------------------------------------------------------------- */
/* Grid                                                                        */
/* -------------------------------------------------------------------------- */

static void draw_grid(void)
{
    uint16_t x;
    uint16_t y;
    uint16_t center_y;

    st7735_fill_rect(
        0U,
        PLOT_TOP,
        ST7735_WIDTH,
        PLOT_HEIGHT,
        TFT_COLOR_NAVY
    );

    /*
     * Vertical cada 16 px.
     */
    for (x = 0U; x < ST7735_WIDTH; x += 16U)
    {
        st7735_draw_vline(
            x,
            PLOT_TOP,
            PLOT_HEIGHT,
            TFT_COLOR_DARKGRAY
        );
    }

    /*
     * Horizontal cada 25 px.
     */
    for (y = PLOT_TOP; y < ST7735_HEIGHT; y += 25U)
    {
        st7735_draw_hline(
            0U,
            y,
            ST7735_WIDTH,
            TFT_COLOR_DARKGRAY
        );
    }

    /*
     * Centro de la ventana vertical.
     */
    center_y = (uint16_t)(
        PLOT_TOP
        + (PLOT_HEIGHT / 2U)
    );

    st7735_draw_hline(
        0U,
        center_y,
        ST7735_WIDTH,
        TFT_COLOR_BLUE
    );
}


/* -------------------------------------------------------------------------- */
/* Waveform                                                                    */
/* -------------------------------------------------------------------------- */

static void draw_waveform(
    const uint16_t *samples,
    uint16_t start_index,
    uint16_t low,
    uint16_t high
)
{
    uint16_t x;
    uint16_t y0;
    uint16_t y1;
    uint16_t y_min;
    uint16_t y_max;

    y0 = adc_to_y(
        samples[start_index],
        low,
        high
    );

    st7735_draw_pixel(
        0U,
        y0,
        TFT_COLOR_YELLOW
    );

    for (x = 1U; x < DISPLAY_SAMPLES; x++)
    {
        y1 = adc_to_y(
            samples[start_index + x],
            low,
            high
        );

        if (y1 < y0)
        {
            y_min = y1;
            y_max = y0;
        }
        else
        {
            y_min = y0;
            y_max = y1;
        }

        st7735_draw_vline(
            x,
            y_min,
            (uint16_t)(
                y_max - y_min + 1U
            ),
            TFT_COLOR_YELLOW
        );

        y0 = y1;
    }
}


/* -------------------------------------------------------------------------- */
/* Public                                                                      */
/* -------------------------------------------------------------------------- */

void scope_ui_init(void)
{
    st7735_fill_screen(
        TFT_COLOR_NAVY
    );
}


void scope_ui_draw(
    const uint16_t *samples,
    uint16_t start_index,
    uint32_t sample_rate_hz,
    scope_scale_t scale_mode,
    scope_trigger_t trigger_mode,
    uint8_t running
)
{
    uint16_t low;
    uint16_t high;

    get_scale_limits(
        samples,
        start_index,
        scale_mode,
        &low,
        &high
    );

    scope_ui_draw_status(
        sample_rate_hz,
        scale_mode,
        trigger_mode,
        running
    );

    draw_grid();

    draw_waveform(
        samples,
        start_index,
        low,
        high
    );
}