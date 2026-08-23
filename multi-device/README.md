# Multi-device

Ejemplos que utilizan dos microcontroladores para demostrar comunicación real entre dispositivos de la familia dsPIC33.

Esta sección separa el firmware de cada microcontrolador, documenta el cableado necesario y conserva capturas de osciloscopio y pruebas visuales realizadas en hardware.

## Combinaciones disponibles

| Combinación | Estado | Interfaces |
| --- | --- | --- |
| [dsPIC33FJ32MC204 ↔ dsPIC33EP32MC204](fj-to-ep) | Verificado en hardware | UART, I²C y SPI |
| dsPIC33EP32MC204 ↔ dsPIC33EP32MC204 | Pendiente | Se añadirá después de la validación |

## Convención de las pruebas

Para facilitar la comparación entre interfaces se utilizan los mismos bytes de prueba:

- `0xA5`: dato enviado desde el dispositivo iniciador/master.
- `0x5A`: respuesta enviada por el segundo microcontrolador.
- `RB4`: LED de verificación visual en ambas tarjetas.

Cada ejemplo incluye el código de ambos dispositivos, firmware `.hex` verificado y evidencias de la prueba cuando están disponibles.

> Une siempre GND entre las dos tarjetas. Si ambas se alimentan independientemente, no unas directamente sus salidas de 3.3 V.
