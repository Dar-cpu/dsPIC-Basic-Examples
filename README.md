# dsPIC-Basic-Examples

Colección de ejemplos básicos y funcionales en **C** para microcontroladores **dsPIC33F** de Microchip.

El repositorio está orientado principalmente al **dsPIC33FJ32MC204** y a la tarjeta de desarrollo diseñada para realizar pruebas, estudiar periféricos y servir como base para proyectos más complejos.

Los ejemplos están desarrollados para **MPLAB X IDE** y **XC16**.

---

## Tarjeta de desarrollo

Actualmente la versión utilizada para el desarrollo y validación de los ejemplos es la **V1I2**.

### PCB V1I1 — Descontinuada

La primera revisión de la tarjeta se conserva como referencia histórica del desarrollo.

<!-- Mantener aquí la imagen que ya tienes actualmente de la V1I1 -->

![dsPIC33FJ32MC204 Development Board V1I1](RUTA_ACTUAL_DE_LA_IMAGEN_V1I1)

### PCB V1I2 — Versión actual

Revisión actual de la tarjeta de desarrollo utilizada para los nuevos ejemplos y pruebas.

![dsPIC33FJ32MC204 Development Board V1I2](Images/dspic33fj_dev_V1I2.jpeg)

> **V1I1:** descontinuada.  
> **V1I2:** versión actual y recomendada.

---

## Hardware principal

- **MCU:** Microchip dsPIC33FJ32MC204
- **Arquitectura:** dsPIC33F de 16 bits
- **Oscilador externo:** cristal de 8 MHz
- **Programación / Debug:** ICSP
- **UART:** disponible para comunicación serial con PC
- **I²C:** disponible para periféricos externos
- **SPI:** disponible
- **ADC:** entradas analógicas
- **PWM:** módulo PWM para aplicaciones generales y control de potencia/motores
- **PPS:** Peripheral Pin Select para periféricos remapeables

---

## Contenido

Cada periférico tiene su propia carpeta con un ejemplo práctico completo:

| Periférico         | Carpeta                            | Descripción                             |
| ------------------ | ---------------------------------- | --------------------------------------- |
| ADC                | `adc/leer-potenciometro`           | Lectura potenciómetro / valor analógico |
| UART               | `uart/prueba-uart`                 | Comunicación serial con PC              |
| Timers             | `timers/timer1`                    | Uso básico de Timer1                    |
| LCD (con I2C)      | `lcd-i2c/texto-simple`             | Mostrar texto en LCD con adaptador I2C  |
| LCD (sin I2C)      | `lcd-sin-i2c/texto-simple`         | Control directo de LCD 16x2             |
| Teclado            | `teclado-4x4/prueba-teclado`       | Lectura de teclado matricial 4x4        |
| Motor Paso a Paso  | `motor-paso-a-paso/modulo-uln2003` | Control de motor stepper                |
| SPWM               | `spwm/generacion-simple`           | Generación de señal SPWM                |
| EEPROM (Lectura)   | `eeprom-lectura/leer-24c512c`      | Lectura de memoria EEPROM externa       |
| EEPROM (Escritura) | `eeprom-escritura/grabar-audio`    | Escritura de EEPROM                     |
| LED Blink          | `led-blink`                        | Conmutar un LED                         |
| Plantilla          | `universal-template`               | Plantilla base para dsPIC33             |
| Proyectos          | `projects`                         | Proyectos de electrónica con dsPIC      |

---

## Cómo usar

1. Clona el repositorio:

   ```bash
   git clone https://github.com/Dar-cpu/dsPIC-Basic-Examples.git
   ```

2. Abre **MPLAB X IDE**.

3. Crea o abre el proyecto correspondiente al ejemplo.

4. Selecciona como dispositivo:

   ```text
   dsPIC33FJ32MC204
   ```

5. Selecciona el compilador **XC16**.

6. Compila el proyecto.

7. Programa el microcontrolador mediante ICSP.

---

## Estructura de los ejemplos

Dependiendo del ejemplo, cada carpeta puede contener:

- `main.c` — código principal.
- `config_bits.c` — configuración del microcontrolador.
- `README.md` — descripción, conexiones y funcionamiento.
- Diagramas o imágenes de las pruebas.
- Archivos adicionales requeridos por el ejemplo.

El objetivo es mantener cada ejemplo lo más independiente y sencillo posible para facilitar su estudio y reutilización.

---

## Estado del proyecto

Este repositorio está en desarrollo continuo.

Se irán añadiendo ejemplos para distintos periféricos, interfaces, algoritmos de control y proyectos completos utilizando el **dsPIC33FJ32MC204**.

### Revisiones de hardware

| Revisión | Estado | Uso |
| -------- | ------ | --- |
| V1I1 | Descontinuada | Conservada como referencia |
| **V1I2** | **Actual** | Desarrollo y validación de nuevos ejemplos |

---

## Herramientas

- Microchip **MPLAB X IDE**
- Microchip **XC16 Compiler**
- Programador/debugger compatible con ICSP
- Tarjeta de desarrollo dsPIC33FJ32MC204 V1I2

---

## Licencia

Este repositorio está destinado principalmente a fines educativos, experimentales y de desarrollo.
