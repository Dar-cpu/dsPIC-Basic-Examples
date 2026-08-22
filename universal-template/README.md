# Plantilla universal para dsPIC33FJ32MC204 y dsPIC33EP32MC204

[← Volver al índice de ejemplos](../README.md)

Esta carpeta contiene dos proyectos MPLAB X preparados para comprobar rápidamente una tarjeta equipada con cualquiera de los dos microcontroladores compatibles:

- **dsPIC33FJ32MC204:** hardware FJ y simulación en Proteus.
- **dsPIC33EP32MC204:** hardware EP.

Ambos proyectos utilizan cristal externo de 8 MHz y PLL para trabajar con `FOSC = 80 MHz` y `FCY = 40 MHz` (40 MIPS).

La aplicación incluida es una prueba de conectividad: **todos los GPIO utilizables cambian HIGH/LOW simultáneamente cada 500 ms**. De esta forma basta con conectar un LED con resistencia y moverlo de pin en pin para comprobar rápidamente los headers de la tarjeta.

## Cuándo usar cada proyecto

| Carpeta / archivo | Dispositivo | Uso |
| --- | --- | --- |
| `test_dspic33fj.X/` | dsPIC33FJ32MC204 | MPLAB X, hardware FJ y simulación Proteus |
| `test_dspic33ep.X/` | dsPIC33EP32MC204 | MPLAB X y hardware EP |
| `test_dspic33fj32mc204.pdsprj` | dsPIC33FJ32MC204 | Esquema de simulación Proteus |

Abre siempre la carpeta `.X` correspondiente al MCU instalado en la tarjeta. Los dos dispositivos comparten gran parte del pinout, pero sus registros de configuración y el canal ICSP utilizado no son iguales.

## Prueba de todos los GPIO

Con el cristal externo conectado se comprueban **33 GPIO**.

```text
500 ms  -> todos los GPIO HIGH
500 ms  -> todos los GPIO LOW
repetir indefinidamente
```

Puedes utilizar un solo LED:

```text
GPIO ---- 1 kΩ ----|>|---- GND
```

Mientras el programa está ejecutándose, mueve el LED entre los GPIO de los headers. Cualquier GPIO incluido en la prueba debe encender y apagar el LED aproximadamente una vez por segundo.

### GPIO incluidos

```text
PORTA: RA0, RA1, RA4, RA7, RA8, RA9, RA10
PORTB: RB0 ... RB15
PORTC: RC0 ... RC9
```

Los pines `RA2` y `RA3` no se utilizan como GPIO porque están conectados al cristal externo mediante `OSC1` y `OSC2`.

## Pines no incluidos en la prueba

| Pin físico | Función |
| ---: | --- |
| 6 | VSS |
| 7 | VCAP |
| 16 | AVSS |
| 17 | AVDD |
| 18 | MCLR |
| 28 | VDD |
| 29 | VSS |
| 30 | OSC1 / RA2 |
| 31 | OSC2 / RA3 |
| 39 | VSS |
| 40 | VDD |

## Diferencias entre FJ y EP

### Configuración digital

| Familia | Registro usado |
| --- | --- |
| dsPIC33FJ | `AD1PCFGL = 0xFFFF` |
| dsPIC33EP | `ANSELA`, `ANSELB`, `ANSELC` |

En el EP se utiliza:

```c
ANSELA = 0x0000;
ANSELB = 0x0000;
ANSELC = 0x0000;
```

### ICSP

| Dispositivo | Configuration Bit | Pines usados en la tarjeta |
| --- | --- | --- |
| dsPIC33FJ32MC204 | `ICS = PGD1` | RB0 / PGED1 y RB1 / PGEC1 |
| dsPIC33EP32MC204 | `ICS = PGD3` | RB0 / PGED3 y RB1 / PGEC3 |

RB0 y RB1 también forman parte de la prueba GPIO. Para evitar que el programador interfiera con estas líneas, programa primero la tarjeta y **desconecta el PICkit antes de comprobar todos los pines**.

> No cambies el canal ICSP sin revisar el hardware de la tarjeta. Una selección incorrecta puede impedir la siguiente sesión de programación o depuración.

## Configuración de reloj

Los dos proyectos utilizan:

```c
PLLFBD = 38;             // M = 40
CLKDIVbits.PLLPRE = 0;   // N1 = 2
CLKDIVbits.PLLPOST = 0;  // N2 = 2
```

Con el cristal de 8 MHz:

```text
FIN  = 8 MHz
FVCO = 8 MHz / 2 × 40 = 160 MHz
FOSC = 160 MHz / 2 = 80 MHz
FCY  = FOSC / 2 = 40 MHz
```

El oscilador primario está configurado en modo `XT`, adecuado para el cristal externo de 8 MHz utilizado en la tarjeta.

## Uso con MPLAB X

### dsPIC33FJ32MC204

1. Abre `test_dspic33fj.X`.
2. Confirma que el dispositivo del proyecto sea `dsPIC33FJ32MC204`.
3. Selecciona XC16.
4. Compila para generar el `.hex`.
5. Programa por PGD1/PGC1.
6. Desconecta el PICkit si también vas a comprobar RB0 y RB1.
7. Recorre los GPIO con el LED de prueba.

### dsPIC33EP32MC204

1. Abre `test_dspic33ep.X`.
2. Confirma que el dispositivo sea `dsPIC33EP32MC204`.
3. Selecciona XC16.
4. Compila y programa la tarjeta por PGD3/PGC3.
5. Desconecta el PICkit si también vas a comprobar RB0 y RB1.
6. Recorre los GPIO con el LED de prueba.

## Uso con Proteus

La simulación incluida utiliza el **dsPIC33FJ32MC204**.

1. Abre `test_dspic33fj32mc204.pdsprj`.
2. Compila `test_dspic33fj.X`.
3. Asigna al dsPIC simulado el archivo HEX generado en `dist/default/production/`.
4. Ejecuta la simulación.

La simulación no representa automáticamente el dsPIC33EP32MC204.

## Adaptar la plantilla

Cuando quieras utilizar esta carpeta como punto de partida para una aplicación nueva:

1. Abre el proyecto correspondiente al MCU instalado.
2. Conserva sus Configuration Bits e ICSP.
3. Conserva la configuración de cristal y PLL si utilizarás los mismos 8 MHz y 40 MIPS.
4. Sustituye la prueba GPIO por tu aplicación.
5. Configura como analógicos únicamente los pines que realmente necesites para ADC.

## Estructura

```text
universal-template/
├── README.md
├── test_dspic33fj.X/
│   ├── main.c
│   └── nbproject/
├── test_dspic33ep.X/
│   ├── main.c
│   └── nbproject/
└── test_dspic33fj32mc204.pdsprj
```

## Si no funciona

| Síntoma | Comprobación |
| --- | --- |
| El proyecto no compila | Confirma el MCU seleccionado y la versión de XC16. |
| El programador no detecta la tarjeta | Verifica PGD1/PGC1 para FJ o PGD3/PGC3 para EP. |
| Ningún GPIO parpadea | Revisa alimentación, MCLR, cristal de 8 MHz y PLL. |
| Solo un GPIO no parpadea | Revisa continuidad, soldadura, header y pista de ese pin. |
| RB0/RB1 no se comportan como esperas | Desconecta el PICkit después de programar. |
| Los retardos no son de 500 ms | Verifica que `FCY` sea 40 MHz. |
