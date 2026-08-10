#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

/* Pantalla usada por este ejemplo: 128 x 160, controlador ST7735/ST7735S. */
#define ST7735_WIDTH   128U
#define ST7735_HEIGHT  160U

/* RGB565 */
#define TFT_COLOR_NAVY        0x08A4U
#define TFT_COLOR_WHITE       0xFFFFU
#define TFT_COLOR_SILVER      0xD73CU
#define TFT_COLOR_GRAY        0x8410U
#define TFT_COLOR_BLUE        0x1B9FU

void st7735_init(void);
void st7735_fill_screen(uint16_t color);
void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7735_draw_hi(uint16_t x, uint16_t y, uint16_t color);
void st7735_draw_image_2bpp(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t *data,
    const uint16_t *palette);

#endif