# dsPIC-Basic-Examples

Colección de ejemplos básicos y funcionales en **C** para microcontroladores **dsPIC** de Microchip.

Este repositorio está pensado para usarse con la tarjeta de desarrollo: ![board](Images/tarjeta.jpeg)  

---

## Contenido

Cada periférico tiene su propia carpeta con un ejemplo práctico completo:

| Periférico              | Carpeta                            | Descripción                              |
|-------------------------|------------------------------------|------------------------------------------|
| ADC                     | `adc/leer-potenciometro`           | Lectura potenciómetro / valor analógico  |
| UART                    | `uart/prueba-uart`                 | Comunicación serial con PC               |
| Timers                  | `timers/timer1`                    | Uso básico de Timer1                     |
| LCD (con I2C)           | `lcd-i2c/texto-simple`             | Mostrar texto en LCD con adaptador I2C   |
| LCD (sin I2C)           | `lcd-sin-i2c/texto-simple`         | Control directo de LCD 16x2              |
| Teclado                 | `teclado-4x4/prueba-teclado`       | Lectura de teclado matricial 4x4         |
| Motor Paso a Paso       | `motor-paso-a-paso/modulo-uln2003` | Control de motor stepper                 |
| SPWM                    | `spwm/generacion-simple`           | Generación de señal SPWM                 |
| EEPROM (Lectura)        | `eeprom-lectura/leer-24c512c`      | Lectura de memoria EEPROM externa        |
| EEPROM (Escritura)      | `eeprom-escritura/grabar-audio`    | Escritura de EEPROM (ejemplo grabación)  |
| LED Blink               | `led-blink`                        | Conmutar un LED                          |
| Plantilla               | `universal-template`               | Plantilla Universal dsPIC33 ep/fj        |
| Proyectos               | `projects`                         | Proyectos de electrónica con dsPIC       |

---

## Cómo usar

1. Clona el repositorio:
   ```bash
   git clone https://github.com/Dar-cpu/dsPIC-Basic-Examples.git

2. Configura un proyecto y selecciona el microcontrolador dsPIC33fj32mc204 en tu MPLAB X IDE.
   
3. Abre el Cóodigo que te interese en el MPLAB X IDE.
   
4. Compila y programa en tu tarjeta de desarrollo.

- Cada carpeta contiene: Código fuente (main.c) para simplificar configuraciones
- README.md con explicación
- Imágenes de pruebas
  
  ![Ejemplos](Images/dar.jpeg)


