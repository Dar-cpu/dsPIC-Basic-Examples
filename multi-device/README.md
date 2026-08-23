# Multi-device

[← Volver al inicio del repositorio](../README.md)

Ejemplos que utilizan **dos microcontroladores** para demostrar comunicación real entre dispositivos de la familia dsPIC33.

Esta sección separa el firmware de cada tarjeta, documenta el cableado necesario y conserva capturas de osciloscopio y pruebas visuales realizadas en hardware.

## Combinaciones verificadas

| Combinación | Estado | Interfaces verificadas |
| --- | --- | --- |
| [dsPIC33FJ32MC204 ↔ dsPIC33EP32MC204](fj-to-ep) | Verificado en hardware | UART, I²C y SPI |
| [dsPIC33EP32MC204 ↔ dsPIC33EP32MC204](ep-to-ep) | Verificado en hardware | UART1, I²C1 y SPI1 |

## Convención de las pruebas

Para facilitar la comparación entre interfaces se utilizan los mismos valores siempre que el protocolo lo permite:

- `0xA5`: dato transmitido por el dispositivo iniciador o master.
- `0x5A`: respuesta del segundo microcontrolador.
- `RB4`: LED de verificación visual en ambas tarjetas.

Cada ejemplo incluye:

- código fuente de ambos dispositivos;
- firmware `.hex` precompilado y probado;
- conexiones entre tarjetas;
- configuración del periférico;
- capturas de osciloscopio;
- GIF de la prueba visual cuando está disponible.

## Organización

```text
multi-device/
├── README.md
├── fj-to-ep/
│   ├── README.md
│   ├── uart/
│   ├── i2c/
│   └── spi/
└── ep-to-ep/
    ├── README.md
    ├── uart1/
    ├── i2c1/
    └── spi1/
```

## Recomendaciones de conexión

- Une siempre GND entre las dos tarjetas.
- Si ambas placas se alimentan independientemente, no unas directamente sus salidas de 3.3 V.
- Para I²C utiliza resistencias pull-up adecuadas a 3.3 V.
- Verifica siempre TX/RX, SDO/SDI y master/slave antes de energizar el montaje.

Los ejemplos de esta carpeta se publican únicamente después de ser comprobados físicamente.
