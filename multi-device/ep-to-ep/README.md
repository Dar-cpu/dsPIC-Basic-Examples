# dsPIC33EP32MC204 ↔ dsPIC33EP32MC204

[← Volver a Multi-device](../README.md)

Esta sección reúne las pruebas de comunicación realizadas entre **dos tarjetas con dsPIC33EP32MC204**. Los tres ejemplos fueron compilados, programados y verificados físicamente antes de publicarse.

Para mantener una referencia común entre protocolos se utilizan los mismos bytes y el mismo indicador visual:

- EP1 transmite `0xA5`.
- EP2 responde `0x5A` cuando el protocolo lo permite.
- `RB4 / pin 33` se utiliza como LED de verificación en ambas tarjetas.
- Ambas placas trabajan con cristal externo de 8 MHz, `FOSC = 80 MHz` y `FCY = 40 MHz`.

## Interfaces verificadas

| Interfaz | Roles | Configuración principal | Verificación |
| --- | --- | --- | --- |
| [UART1](uart1) | EP1 iniciador / EP2 respuesta | 115200 baud, 8N1 | `0xA5` → `0x5A` |
| [I²C1](i2c1) | EP1 master / EP2 slave | ~100 kHz, dirección `0x42` | escritura `0xA5` + lectura `0x5A` |
| [SPI1](spi1) | EP1 master / EP2 slave | 625 kHz, 8 bits, full-duplex | `0xA5` por MOSI + `0x5A` por MISO |

## Estructura

```text
ep-to-ep/
├── README.md
├── uart1/
│   ├── README.md
│   ├── ep1/
│   ├── ep2/
│   └── img/
├── i2c1/
│   ├── README.md
│   ├── ep1/
│   ├── ep2/
│   └── img/
└── spi1/
    ├── README.md
    ├── ep1/
    ├── ep2/
    └── img/
```

Cada protocolo incluye:

- `main.c` separado para EP1 y EP2.
- Firmware `.hex` precompilado para cada tarjeta.
- Cableado y configuración documentados.
- Capturas de osciloscopio de las señales reales.
- GIF de la prueba visual con LEDs.

## Hardware común

- 2 × tarjetas TECKIO dsPIC33 con dsPIC33EP32MC204.
- 2 × LEDs con resistencia limitadora conectados a `RB4`.
- Jumpers.
- GND común.
- Para I²C1: 2 × resistencias pull-up de 4.7 kΩ a 3.3 V.

> Las dos tarjetas deben compartir GND. Si se alimentan mediante reguladores independientes, no unas directamente sus salidas de 3.3 V.

## Estado

**UART1, I²C1 y SPI1 verificados en hardware.**
