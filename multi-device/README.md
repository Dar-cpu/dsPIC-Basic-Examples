# Multi-device

Ejemplos que utilizan dos microcontroladores para demostrar comunicación real entre dispositivos de la familia dsPIC33.

Esta sección separa el firmware de cada microcontrolador, documenta el cableado necesario y conserva capturas de osciloscopio y pruebas visuales realizadas en hardware.

## Combinaciones disponibles

| Combinación | Estado | Interfaces verificadas |
| --- | --- | --- |
| [dsPIC33FJ32MC204 ↔ dsPIC33EP32MC204](fj-to-ep) | Verificado en hardware | UART, I²C y SPI |
| [dsPIC33EP32MC204 ↔ dsPIC33EP32MC204](ep-to-ep) | Verificado en hardware | UART1, I²C1 y SPI1 |

## Convención de las pruebas

Para facilitar la comparación entre interfaces se utilizan los mismos bytes:

- `0xA5`: dato enviado desde el dispositivo iniciador o master.
- `0x5A`: respuesta enviada por el segundo microcontrolador.
- `RB4`: LED de verificación visual en ambas tarjetas.

Cada ejemplo incluye el código de ambos dispositivos, firmware `.hex` probado y evidencias de la validación cuando están disponibles.

> Une siempre GND entre las dos tarjetas. Si ambas se alimentan independientemente, no unas directamente sus salidas de 3.3 V.
