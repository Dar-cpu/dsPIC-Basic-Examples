# dsPIC-Basic-Examples

Colección de ejemplos prácticos y funcionales en **C** para microcontroladores **dsPIC33** de Microchip.

El repositorio está orientado principalmente al **dsPIC33FJ32MC204** y a una tarjeta de desarrollo propia utilizada para estudiar periféricos, validar hardware y construir proyectos más completos a partir de ejemplos pequeños y reutilizables.

Los proyectos están desarrollados principalmente con **MPLAB X IDE** y el compilador **XC16**.

---

## Tarjeta de desarrollo

La revisión utilizada actualmente para el desarrollo y validación de nuevos ejemplos es la **PCB V1I2**.

### PCB V1I1 — Descontinuada

La primera revisión se conserva como referencia histórica del desarrollo.

<p align="center">
  <img src="Images/tarjeta.jpeg" alt="dsPIC33FJ32MC204 Development Board V1I1" width="620">
</p>

### PCB V1I2 — Versión actual

Revisión actual de la tarjeta utilizada para los nuevos ejemplos y pruebas.

<p align="center">
  <img src="Images/dspic33fj_dev_V1I2.jpeg" alt="dsPIC33FJ32MC204 Development Board V1I2" width="620">
</p>

| Revisión | Estado | Uso |
| --- | --- | --- |
| V1I1 | Descontinuada | Conservada como referencia |
| **V1I2** | **Actual** | Desarrollo y validación de nuevos ejemplos |

---

## Hardware principal

- **MCU principal:** Microchip dsPIC33FJ32MC204
- **MCU alternativo soportado por la PCB:** dsPIC33EP32MC204
- **Arquitectura:** dsPIC33 de 16 bits
- **Oscilador externo:** cristal de 8 MHz
- **Programación / Debug:** ICSP
- **UART:** comunicación serial
- **I²C:** periféricos y dispositivos externos
- **SPI:** comunicaciones síncronas y pantallas TFT
- **ADC:** adquisición de señales analógicas
- **Timers:** temporización, muestreo y generación de eventos
- **PWM:** aplicaciones generales, control de potencia y motores
- **PPS:** Peripheral Pin Select para periféricos remapeables

> **Importante sobre ICSP:** la plantilla universal conserva la configuración correspondiente a cada MCU. En esta PCB, el **dsPIC33FJ32MC204 utiliza PGD1/PGC1** y el **dsPIC33EP32MC204 utiliza PGD3/PGC3**. No conviene cambiar este ajuste sin verificar primero qué dispositivo está montado.

---

## Ejemplos destacados recientes

### TFT ST7735 — Imágenes y logos

Ejemplo para controlar una pantalla **ST7735/ST7735S 128x160** mediante SPI1 y mostrar imágenes compactadas a **2 bits por píxel** con una paleta RGB565 de cuatro colores.

Incluye además un conversor gráfico en Python para transformar imágenes PNG/JPG/BMP/GIF/WEBP directamente a un header C almacenado en Flash/PSV.

**[Ver ejemplo `tft-st7735/mostrar-logo`](tft-st7735/mostrar-logo)**

<p align="center">
  <img src="tft-st7735/mostrar-logo/images/prueba_tft.jpeg" alt="Logo mostrado en TFT ST7735" width="430">
</p>

### ADC + TFT — Mini osciloscopio

Ejemplo de adquisición utilizando el **ADC de 12 bits**, Timer3 y una TFT ST7735 para visualizar una señal analógica en tiempo real.

Incluye captura de 256 muestras, visualización de 128 puntos, frecuencias de muestreo seleccionables, escalas verticales, trigger por software y modos **RUN/HOLD** controlados mediante cuatro pulsadores.

**[Ver ejemplo `tft-st7735/adc-tft`](tft-st7735/adc-tft)**

<p align="center">
  <img src="tft-st7735/adc-tft/images/tft.jpeg" alt="Mini osciloscopio ADC en TFT ST7735" width="430">
</p>

---

## Contenido

Cada periférico o función tiene su propia carpeta con uno o más ejemplos prácticos.

| Categoría | Ejemplo | Descripción |
| --- | --- | --- |
| ADC | [adc/leer-potenciometro](adc/leer-potenciometro) | Lectura de potenciómetro / señal analógica |
| UART | [uart/prueba-uart](uart/prueba-uart) | Comunicación serial con PC |
| Timers | [timers/timer1](timers/timer1) | Uso básico de Timer1 |
| LCD + I²C | [lcd-i2c/texto-simple](lcd-i2c/texto-simple) | Mostrar texto en LCD mediante adaptador I²C |
| LCD paralelo | [lcd-sin-i2c/texto-simple](lcd-sin-i2c/texto-simple) | Control directo de LCD 16x2 |
| Teclado | [teclado-4x4/prueba-teclado](teclado-4x4/prueba-teclado) | Lectura de teclado matricial 4x4 |
| Motor paso a paso | [motor-paso-a-paso/modulo-uln2003](motor-paso-a-paso/modulo-uln2003) | Control de motor stepper mediante ULN2003 |
| SPWM | [spwm/generacion-simple](spwm/generacion-simple) | Generación de señal SPWM |
| EEPROM lectura | [eeprom-lectura/leer-24c512c](eeprom-lectura/leer-24c512c) | Lectura de memoria EEPROM externa |
| EEPROM escritura | [eeprom-escritura/grabar-audio](eeprom-escritura/grabar-audio) | Escritura de EEPROM |
| LED | [led-blink](led-blink) | Ejemplo básico de conmutación de un LED |
| TFT ST7735 | [tft-st7735/mostrar-logo](tft-st7735/mostrar-logo) | Mostrar logos e imágenes mediante SPI y RGB565/2 bpp |
| ADC + TFT | [tft-st7735/adc-tft](tft-st7735/adc-tft) | Mini osciloscopio con ADC de 12 bits y ST7735 |
| Plantilla | [universal-template](universal-template) | Plantilla base para dsPIC33FJ / dsPIC33EP |
| Proyectos | [projects](projects) | Proyectos completos de electrónica con dsPIC |

---

## Cómo usar

1. Clona el repositorio:

   ```bash
   git clone https://github.com/Dar-cpu/dsPIC-Basic-Examples.git
   ```

2. Abre **MPLAB X IDE**.

3. Crea un proyecto nuevo o utiliza la configuración correspondiente al ejemplo.

4. Selecciona el microcontrolador instalado en la tarjeta. Para la mayoría de los ejemplos:

   ```text
   dsPIC33FJ32MC204
   ```

5. Selecciona el compilador **XC16**.

6. Añade al proyecto los archivos `.c` y `.h` indicados en el README del ejemplo.

7. Compila y programa el microcontrolador mediante ICSP.

> Revisa siempre el `README.md` de cada ejemplo antes de conectar hardware externo. Allí se documentan pines, alimentación, señales de prueba y configuraciones específicas.

---

## Estructura de los ejemplos

Dependiendo de su complejidad, una carpeta puede contener:

```text
nombre-del-ejemplo/
├── README.md
├── main.c
├── config_bits.c
├── modulo.c
├── modulo.h
└── images/
    └── prueba.jpeg
```

Los archivos más comunes son:

- `main.c` — aplicación principal.
- `config_bits.c` — Configuration Bits del microcontrolador.
- `*.c` / `*.h` — drivers o módulos específicos del ejemplo.
- `README.md` — funcionamiento, conexiones y procedimiento de prueba.
- `images/` — fotografías, capturas de osciloscopio y resultados reales.
- Scripts o herramientas auxiliares cuando el ejemplo los requiere.

El objetivo es mantener cada ejemplo lo más **independiente, reproducible y sencillo de reutilizar** posible.

---

## Organización del repositorio

```text
dsPIC-Basic-Examples/
├── Images/                  # Imágenes generales de las PCB
├── adc/
├── uart/
├── timers/
├── lcd-i2c/
├── lcd-sin-i2c/
├── teclado-4x4/
├── motor-paso-a-paso/
├── spwm/
├── eeprom-lectura/
├── eeprom-escritura/
├── led-blink/
├── tft-st7735/
│   ├── mostrar-logo/
│   └── adc-tft/
├── universal-template/
├── projects/
├── LICENSE
└── README.md
```

Los ejemplos básicos se mantienen separados de `projects/`, donde pueden desarrollarse aplicaciones más completas que integren varios periféricos y módulos.

---

## Estado del proyecto

El repositorio está en **desarrollo continuo**.

Se irán añadiendo ejemplos de periféricos, comunicaciones, adquisición de señales, interfaces gráficas, control, electrónica de potencia y proyectos completos basados principalmente en el **dsPIC33FJ32MC204**.

La PCB V1I2 es la plataforma de referencia actual para las nuevas pruebas.

---

## Herramientas

- Microchip **MPLAB X IDE**
- Microchip **XC16 Compiler**
- Programador/debugger compatible con **ICSP**
- Tarjeta de desarrollo dsPIC V1I2
- Python 3 para los ejemplos que incluyen herramientas auxiliares

---

## Licencia

Este proyecto se distribuye bajo la **licencia MIT**.

Consulta el archivo [LICENSE](LICENSE) para más información.
