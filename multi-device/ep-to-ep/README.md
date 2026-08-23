# dsPIC33EP32MC204 ↔ dsPIC33EP32MC204

Tres ejemplos de comunicación entre dos tarjetas con **dsPIC33EP32MC204**, verificados físicamente antes de publicarse.

Para mantener una referencia común entre protocolos se emplean los mismos bytes de prueba:

- EP1 envía `0xA5`.
- EP2 responde `0x5A`.
- `RB4` se utiliza como indicador visual en ambas tarjetas.

## Ejemplos

| Interfaz | Roles | Configuración | Resultado |
| --- | --- | --- | --- |
| [UART1](uart1) | EP1 iniciador / EP2 respuesta | 115200 baud, 8N1 | `0xA5` → `0x5A` |
| [I²C1](i2c1) | EP1 master / EP2 slave | ~100 kHz, dirección `0x42` | escritura `0xA5` + lectura `0x5A` |
| [SPI1](spi1) | EP1 master / EP2 slave | 625 kHz, 8 bits | `0xA5` por MOSI y `0x5A` por MISO |

## Estructura

```text
ep-to-ep/
├── README.md
├── uart1/
├── i2c1/
└── spi1/
```

Cada interfaz contiene:

```text
interfaz/
├── README.md
├── ep1/
│   ├── main.c
│   └── dsPIC33EP32MC204-ep1.hex
├── ep2/
│   ├── main.c
│   └── dsPIC33EP32MC204-ep2.hex
└── img/
```

Los videos cortos muestran la respuesta de los LEDs y las capturas de osciloscopio permiten comprobar las señales reales.

> Las dos tarjetas deben compartir GND. Si se alimentan mediante reguladores independientes, no unas directamente sus salidas de 3.3 V.
