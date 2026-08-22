# Ejemplos para dsPIC33FJ32MC204

[← Volver al inicio del repositorio](../README.md)

Esta carpeta reúne los ejemplos y proyectos desarrollados específicamente para el **Microchip dsPIC33FJ32MC204** sobre la tarjeta de desarrollo TECKIO dsPIC33 V1I2.

Los ejemplos utilizan principalmente un cristal externo de 8 MHz, `FOSC = 80 MHz`, `FCY = 40 MHz` y programación mediante **PGD1 / PGC1**. Cada carpeta conserva el código, las conexiones y los resultados de prueba necesarios para reproducir el funcionamiento.

<p align="center">
  <img src="../Images/dspic33fj_dev_V1I2.jpeg" alt="Tarjeta de desarrollo con dsPIC33FJ32MC204" width="660">
</p>

## Ruta de aprendizaje recomendada

| Orden | Ejemplo | Periféricos o concepto | Nivel |
| ---: | --- | --- | --- |
| 1 | [LED Blink](gpio/led-blink) | GPIO y retardos | Inicial |
| 2 | [Timer1](timer/timer1) | Timer e interrupciones | Inicial |
| 3 | [ADC + potenciómetro](adc/leer-potenciometro) | ADC de 10 bits | Inicial |
| 4 | [UART](uart/prueba-uart) | UART1 y PPS | Inicial |
| 5 | [LCD paralelo](lcd-sin-i2c/texto-simple) | GPIO y HD44780 | Inicial |
| 6 | [LCD por I²C](lcd-i2c/texto-simple) | I²C1 y PCF8574 | Intermedio |
| 7 | [Teclado 4×4](teclado-4x4/prueba-teclado) | Escaneo de GPIO | Intermedio |
| 8 | [Motor + ULN2003](motor-paso-a-paso/modulo-uln2003) | Secuencias de medio paso | Intermedio |
| 9 | [Generación SPWM](pwm/generacion-simple) | Motor Control PWM e ISR | Intermedio |
| 10 | [Grabar audio en EEPROM](eeprom-escritura/grabar-audio) | UART, Python, I²C y EEPROM | Avanzado |
| 11 | [Reproducir audio](eeprom-lectura/leer-24c512c) | EEPROM, buffer, Timer y PWM | Avanzado |
| 12 | [Mostrar imágenes en TFT](tft-st7735/mostrar-logo) | SPI1, RGB565, Flash/PSV y Python | Avanzado |
| 13 | [Mini osciloscopio ADC + TFT](tft-st7735/adc-tft) | ADC de 12 bits, timers, SPI y UI | Avanzado |

## Índice por categoría

| Categoría | Carpeta | Contenido actual |
| --- | --- | --- |
| GPIO | [`gpio/`](gpio) | Parpadeo básico de LED |
| ADC | [`adc/`](adc) | Lectura de una señal analógica |
| UART | [`uart/`](uart) | Comunicación serial |
| Timer | [`timer/`](timer) | Timer1 con interrupciones |
| PWM | [`pwm/`](pwm) | Generación SPWM |
| LCD + I²C | [`lcd-i2c/`](lcd-i2c) | LCD 16×2 mediante PCF8574 |
| LCD paralelo | [`lcd-sin-i2c/`](lcd-sin-i2c) | LCD 16×2 conectado directamente |
| Teclado | [`teclado-4x4/`](teclado-4x4) | Lectura de teclado matricial |
| Motores | [`motor-paso-a-paso/`](motor-paso-a-paso) | Control mediante ULN2003 |
| EEPROM escritura | [`eeprom-escritura/`](eeprom-escritura) | Grabación de audio en AT24C512C |
| EEPROM lectura | [`eeprom-lectura/`](eeprom-lectura) | Reproducción de audio desde EEPROM |
| TFT ST7735 | [`tft-st7735/`](tft-st7735) | Imágenes y mini osciloscopio |
| Proyectos | [`projects/`](projects) | Aplicaciones completas y electrónica de potencia |

## Cómo usar un ejemplo

1. Abre el README del ejemplo y revisa sus conexiones.
2. Crea un **Standalone Project** para `dsPIC33FJ32MC204`.
3. Selecciona XC16.
4. Añade `src/main.c` y los demás archivos indicados.
5. Compila y programa mediante PGD1 / PGC1.
6. Comprueba el resultado con las imágenes o mediciones incluidas.

Los ejemplos básicos contienen los Configuration Bits dentro de `src/main.c`. Los ejemplos TFT separan la configuración y los drivers en varios archivos.

## Organización

```text
dsPIC33FJ32MC204/
├── README.md
├── gpio/
│   └── led-blink/
├── adc/
├── uart/
├── timer/
├── pwm/
├── lcd-i2c/
├── lcd-sin-i2c/
├── teclado-4x4/
├── motor-paso-a-paso/
├── eeprom-escritura/
├── eeprom-lectura/
├── tft-st7735/
└── projects/
```

## Compatibilidad

Este código utiliza registros y Configuration Bits del dsPIC33FJ32MC204. No debe compilarse directamente para el dsPIC33EP32MC204 sin revisar primero las diferencias del periférico utilizado.

Para iniciar un proyecto que pueda seleccionar cualquiera de los dos dispositivos, utiliza la [plantilla universal](../universal-template).
