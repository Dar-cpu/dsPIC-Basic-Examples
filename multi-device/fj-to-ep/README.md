# Comunicación dsPIC33FJ32MC204 ↔ dsPIC33EP32MC204

Ejemplos verificados en hardware entre una tarjeta con **dsPIC33FJ32MC204** y otra con **dsPIC33EP32MC204**.

Las tres pruebas utilizan `RB4` como indicador visual y los bytes `0xA5` / `0x5A` para que el comportamiento sea fácil de comparar.

## Interfaces verificadas

| Interfaz | Roles | Configuración | Resultado |
| --- | --- | --- | --- |
| [UART](uart) | Comunicación bidireccional | 115200 baud, 8N1 | FJ envía `0xA5`; EP responde `0x5A` |
| [I²C](i2c) | FJ master / EP slave | 100 kHz aprox., dirección `0x42` | FJ escribe `0xA5` y luego lee `0x5A` |
| [SPI](spi) | FJ master / EP slave | 625 kHz, 8 bits | `0xA5` por MOSI y `0x5A` por MISO en full-duplex |

## Hardware común

- 1 × tarjeta TECKIO dsPIC33 con dsPIC33FJ32MC204.
- 1 × tarjeta TECKIO dsPIC33 con dsPIC33EP32MC204.
- 2 × LED con resistencia limitadora para `RB4`.
- Jumpers.
- GND común.
- Para I²C: 2 × resistencias pull-up de 4.7 kΩ a 3.3 V.

## Estructura

```text
fj-to-ep/
├── README.md
├── uart/
├── i2c/
└── spi/
```

Cada interfaz contiene el firmware del FJ y del EP por separado, además de imágenes de las señales medidas y un GIF corto de la prueba con LEDs.
