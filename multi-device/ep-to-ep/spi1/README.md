# SPI1 — dsPIC33EP32MC204 ↔ dsPIC33EP32MC204

Prueba SPI1 full-duplex entre dos dsPIC33EP32MC204, con EP1 como master y EP2 como slave, validada en hardware.

## Configuración

| Parámetro | EP1 | EP2 |
| --- | --- | --- |
| Rol | Master | Slave |
| SCK1 | RC3 / pin 36 | RC3 / pin 36 |
| SDO1 | RA4 / pin 34 | RA4 / pin 34 |
| SDI1 | RA9 / pin 35 | RA9 / pin 35 |
| SS / CS | RB0 / pin 21 | RB0 / pin 21 |
| LED | RB4 | RB4 |
| Frecuencia | 625 kHz | Sigue SCK1 |
| Formato | 8 bits | 8 bits |

Configuración de reloj SPI: `CKP = 0`, `CKE = 1`, `PPRE = 16:1` y `SPRE = 4:1`, con `FCY = 40 MHz`.

## Cableado

```text
EP1 MASTER                       EP2 SLAVE

RC3 / SCK1  ------------------> RC3 / SCK1
RA4 / SDO1  ------------------> RA9 / SDI1    MOSI
RA9 / SDI1  <------------------ RA4 / SDO1    MISO
RB0 / CS    ------------------> RB0 / SS1
GND         ------------------- GND
```

## Funcionamiento

SPI transfiere ambos bytes durante los mismos ocho pulsos de reloj:

```text
                    8 clocks @ 625 kHz
              ┌───────────────────────┐
EP1 MOSI  --->│ 1 0 1 0 0 1 0 1      │---> EP2
              │       0xA5            │
              │                       │
EP1 MISO  <---│ 0 1 0 1 1 0 1 0      │<--- EP2
              │       0x5A            │
              └───────────────────────┘
```

Cuando EP2 recibe `0xA5`, conmuta su LED. Si EP1 recibe `0x5A`, genera un pulso de 100 ms en su LED.

## Código y firmware

- [`ep1/main.c`](ep1/main.c): master SPI1.
- [`ep2/main.c`](ep2/main.c): slave SPI1.
- [`ep1/dsPIC33EP32MC204-ep1.hex`](ep1/dsPIC33EP32MC204-ep1.hex)
- [`ep2/dsPIC33EP32MC204-ep2.hex`](ep2/dsPIC33EP32MC204-ep2.hex)

## Verificación con osciloscopio

### SCK1 + MOSI

- CH1 amarillo: `SCK1 = RC3`.
- CH2 azul: `MOSI = RA4` de EP1.
- Escala real: **1 µs/div**.

<p align="center">
  <img src="img/spi1-sck-mosi-1us.png" alt="SPI1 SCK y MOSI entre dos dsPIC33EP32MC204" width="850">
</p>

La señal MOSI representa `0xA5 = 1010 0101`.

### SCK1 + MISO

- CH1 amarillo: `SCK1 = RC3`.
- CH2 azul: `MISO = RA9` de EP1.

<p align="center">
  <img src="img/spi1-sck-miso-1us.png" alt="SPI1 SCK y MISO entre dos dsPIC33EP32MC204" width="850">
</p>

La señal MISO representa `0x5A = 0101 1010`.

Con `fSCK = 625 kHz`, el período esperado es `1.6 µs`, consistente con las capturas.

## Prueba visual

[Ver prueba SPI1 EP ↔ EP](img/spi1-ep-to-ep-led-test.mp4)

## Estado

**Verificado en hardware.**
