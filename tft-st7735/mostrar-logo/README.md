# dsPIC Image Converter

Utilidad gráfica para convertir imágenes normales al formato compacto usado por el proyecto **dsPIC33FJ32MC204 + ST7735 128x160**.

## Funciones

- Selección de PNG, JPG, BMP, GIF o WEBP
- Ajuste máximo a 128x160
- Opción de mantener proporción
- Dithering opcional
- Fondo configurable para imágenes con transparencia
- Vista previa del resultado
- Cuantización automática a 4 colores
- Conversión a RGB565
- Empaquetado 2 bits/pixel
- Copia directa del header C al portapapeles
- Guardado directo como `.h`
- Copia de una llamada de ejemplo para MPLAB X

## Instalación

```bash
python -m pip install -r requirements.txt
```

En Windows, `tkinter` normalmente viene incluido con Python.

## Ejecutar

```bash
python dspic_image_converter.py
```

o hacer doble clic en:

```text
run_windows.bat
```

## Formato generado

Cada imagen produce:

```c
#define LOGO_WIDTH  ...
#define LOGO_HEIGHT ...
#define LOGO_BYTES  ...

static const uint16_t logo_palette_rgb565[4]
    __attribute__((space(auto_psv))) = { ... };

static const uint8_t logo_2bpp[LOGO_BYTES]
    __attribute__((space(auto_psv))) = { ... };
```

La paleta también se genera automáticamente, de modo que no estás limitado a los cuatro colores originales del logo TECKIO.

## Función genérica en el dsPIC

Los archivos `st7735.c` y `st7735.h` incluidos ya contienen la función genérica:

```c
void st7735_draw_image_2bpp(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const uint8_t *data,
    const uint16_t *palette
);
```

Ejemplo:

```c
#include "mi_logo.h"

st7735_draw_image_2bpp(
    4U, 62U,
    MI_LOGO_WIDTH,
    MI_LOGO_HEIGHT,
    mi_logo_2bpp,
    mi_logo_palette_rgb565
);
```

## Memoria

RGB565 completo de 128x160:

```text
128 * 160 * 2 = 40960 bytes
```

2 bpp:

```text
128 * 160 / 4 = 5120 bytes
```

más 8 bytes de paleta.

Un logo de 120x60 ocupa:

```text
120 * 60 / 4 = 1800 bytes
```

Esto mantiene los gráficos en Flash/PSV y evita consumir la RAM de 2 KB del dsPIC.
