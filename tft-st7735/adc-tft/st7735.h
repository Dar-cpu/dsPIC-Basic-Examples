#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

/* TFT usada por este ejemplo. */
#define ST7735_WIDTH   128U
#define ST7735_HEIGHT  160U

/* RGB565 */
#define TFT_COLOR_BLACK       0x0000U
#define TFT_COLOR_NAVY        0x08A4U
#define TFT_COLOR_WHITE       0xFFFFU
#define TFT_COLOR_GRAY        0x8410U
#define TFT_COLOR_DARKGRAY    0x4208U
#define TFT_COLOR_BLUE        0x1B9FU
#define TFT_COLOR_CYAN        0x07FFU
#define TFT_COLOR_GREEN       0x07E0U
#define TFT_COLOR_YELLOW      0xFFE0U
#define TFT_COLOR_RED         0xF800U

void st7735_init(void);

void st7735_fill_screen(uint16_t color);

void st7735_fill_rect(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    uint16_t color
);

void st7735_draw_hline(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t color
);

void st7735_draw_vline(
    uint16_t x,
    uint16_t y,
    uint16_t h,
    uint16_t color
);

void st7735_draw_pixel(
    uint16_t x,
    uint16_t y,
    uint16_t color
);

#endif
