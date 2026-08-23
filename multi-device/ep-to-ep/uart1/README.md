# UART1 — dsPIC33EP32MC204 ↔ dsPIC33EP32MC204

Prueba UART1 bidireccional entre dos dsPIC33EP32MC204, validada en hardware.

## Configuración

| Parámetro | EP1 | EP2 |
| --- | --- | --- |
| Rol | Iniciador | Respuesta |
| U1TX | RC8 / RP56 | RC8 / RP56 |
| U1RX | RC9 / RP57 | RC9 / RP57 |
| LED | RB4 | RB4 |
| Baudrate | 115200 | 115200 |
| Formato | 8N1 | 8N1 |

Ambos micros usan cristal externo de 8 MHz, `FOSC = 80 MHz` y `FCY = 40 MHz`. La UART está configurada con `BRGH = 1` y `U1BRG = 86`, para un baudrate real aproximado de 114943 baud.

## Cableado

```text
EP1                              EP2

RC8 / U1TX  ------------------> RC9 / U1RX
RC9 / U1RX  <------------------ RC8 / U1TX
GND         ------------------- GND
```

## Funcionamiento

EP1 envía `0xA5`. EP2 valida el byte, conmuta su LED y responde `0x5A`. Cuando EP1 recibe correctamente `0x5A`, enciende su LED durante 100 ms.

```text
EP1                         EP2
 |                           |
 |-------- 0xA5 ----------->|
 |                           |  RB4 ^= 1
 |<------- 0x5A ------------|
 |  RB4 = ON 100 ms          |
```

## Código y firmware

- [`ep1/main.c`](ep1/main.c): iniciador.
- [`ep2/main.c`](ep2/main.c): receptor/respuesta.
- [`ep1/dsPIC33EP32MC204-ep1.hex`](ep1/dsPIC33EP32MC204-ep1.hex)
- [`ep2/dsPIC33EP32MC204-ep2.hex`](ep2/dsPIC33EP32MC204-ep2.hex)

## Verificación con osciloscopio

Las capturas se realizaron a una escala real de **10 µs/div**.

### EP1 TX — `0xA5`

CH1 amarillo corresponde a `RC8 / U1TX` de EP1.

<p align="center">
  <img src="img/uart1-ep1-tx-0xa5-10us.png" alt="UART1 EP1 transmitiendo 0xA5" width="850">
</p>

`0xA5 = 1010 0101`. UART transmite LSB primero:

```text
START D0 D1 D2 D3 D4 D5 D6 D7 STOP
  0    1  0  1  0  0  1  0  1   1
```

### EP1 RX — `0x5A`

CH2 azul corresponde a `RC9 / U1RX` de EP1 y muestra la respuesta transmitida por EP2.

<p align="center">
  <img src="img/uart1-ep1-rx-0x5a-10us.png" alt="UART1 EP1 recibiendo 0x5A" width="850">
</p>

`0x5A = 0101 1010`, también transmitido LSB primero.

## Prueba visual

El video corto muestra el funcionamiento de los LEDs durante el intercambio:

[Ver prueba UART1 EP ↔ EP](img/uart1-ep-to-ep-led-test.mp4)

## Estado

**Verificado en hardware.**
