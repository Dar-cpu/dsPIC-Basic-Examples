# Ejemplos para dsPIC33EP32MC204

[← Volver al inicio del repositorio](../README.md)

Esta carpeta contiene los ejemplos desarrollados específicamente para el **Microchip dsPIC33EP32MC204** montado en la tarjeta TECKIO dsPIC33 V1I2.

## Estado actual

La base del dispositivo ya está verificada con cristal externo de 8 MHz, PLL para `FOSC = 80 MHz`, `FCY = 40 MHz`, GPIO digitales y programación mediante **PGD3 / PGC3**.

Además, el dispositivo ya fue probado en comunicación real con otro dsPIC mediante:

- [FJ ↔ EP](../multi-device/fj-to-ep): UART, I²C y SPI.
- [EP ↔ EP](../multi-device/ep-to-ep): UART1, I²C1 y SPI1.

Los ejemplos individuales se irán incorporando después de validarlos físicamente.

## Configuración base

| Parámetro | Valor |
| --- | --- |
| Dispositivo | dsPIC33EP32MC204 |
| Cristal | 8 MHz externo |
| FOSC | 80 MHz |
| FCY | 40 MHz |
| Programación | PGD3 / PGC3 |
| Compilador | XC16 |

> Mantén `ICS = PGD3` para esta tarjeta, salvo que el hardware se modifique expresamente.

## Ruta prevista de ejemplos individuales

```text
dsPIC33EP32MC204/
├── README.md
├── gpio/
├── timer/
├── uart/
├── adc/
├── pwm/
├── i2c/
├── spi/
├── dht11/
├── lcd-i2c/
├── tft-st7735/
├── rfid-rc522/
└── stepper/
```

Las carpetas se crearán únicamente cuando exista al menos un ejemplo compilado y verificado.

## Diferencias respecto al dsPIC33FJ32MC204

Aunque ambos dispositivos pueden utilizar la misma PCB, no deben tratarse como equivalentes. Cambian, entre otros puntos:

- Configuration Bits.
- Registros de selección analógica (`ANSELx`).
- Peripheral Pin Select.
- Inicialización y disponibilidad de algunos periféricos.
- Canal ICSP utilizado por la tarjeta.

## Comenzar un proyecto

1. Crea un **Standalone Project** en MPLAB X.
2. Selecciona `dsPIC33EP32MC204`.
3. Selecciona XC16.
4. Copia [`../universal-template/main.c`](../universal-template/main.c).
5. Conserva PGD3 / PGC3.
6. Añade únicamente la inicialización del periférico que utilizarás.
