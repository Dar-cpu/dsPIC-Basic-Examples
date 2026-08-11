# Plantilla universal para dsPIC33FJ32MC204 y dsPIC33EP32MC204

[← Volver al índice de ejemplos](../README.md)

Esta carpeta contiene dos proyectos MPLAB X con una misma aplicación base adaptada mediante compilación condicional para dos microcontroladores:

- **dsPIC33FJ32MC204:** simulación en Proteus y uso en hardware con el MCU FJ.
- **dsPIC33EP32MC204:** uso en la tarjeta cuando está montado el MCU EP.

Ambas variantes configuran el cristal externo de 8 MHz y el PLL para trabajar con `FOSC = 80 MHz` y `FCY = 40 MHz` (40 MIPS). Como prueba mínima, RB4 cambia de estado cada 500 ms.

## Cuándo usar cada proyecto

| Carpeta / archivo | Dispositivo | Uso |
| --- | --- | --- |
| `test_dspic33fj.X/` | dsPIC33FJ32MC204 | MPLAB X, hardware FJ y simulación Proteus |
| `test_dspic33ep.X/` | dsPIC33EP32MC204 | MPLAB X y hardware EP |
| `test_dspic33fj32mc204.pdsprj` | dsPIC33FJ32MC204 | Esquema de simulación Proteus |

Aunque los dos `main.c` incluidos son equivalentes, cada proyecto conserva su dispositivo y configuración de compilación. Abre la carpeta `.X` correspondiente; no cambies únicamente el nombre del MCU esperando que todos los registros sean idénticos.

## Diferencias que maneja la plantilla

### Pines analógicos

| Familia | Registro usado para configurar GPIO digital |
| --- | --- |
| dsPIC33FJ | `AD1PCFGL` |
| dsPIC33EP | `ANSELA`, `ANSELB`, `ANSELC` |

La función `Init_Hardware_Base()` selecciona el bloque correcto mediante macros definidas por XC16:

```c
#if defined(__dsPIC33FJ32MC204__)
    AD1PCFGL = 0xFFFF;
#elif defined(__dsPIC33EP32MC204__)
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
#endif
```

### Configuration Bits e ICSP

| Dispositivo | Canal ICSP configurado | Uso en la tarjeta |
| --- | --- | --- |
| dsPIC33FJ32MC204 | `ICS = PGD1` | PGD1 / PGC1 |
| dsPIC33EP32MC204 | `ICS = PGD3` | PGD3 / PGC3 |

> **No cambies estos bits sin revisar el hardware.** El puerto ICSP es diferente para cada MCU en esta tarjeta y una selección incorrecta puede impedir la siguiente sesión de programación o depuración.

Otros Configuration Bits también cambian entre familias; por ejemplo, el dsPIC33EP incorpora la opción de bloqueo del PLL (`PLLKEN`) y registros alternativos de I²C que no existen de la misma forma en el FJ.

## Configuración de reloj

Para ambos dispositivos se usan estos valores:

```c
PLLFBD = 38;             // M = 40
CLKDIVbits.PLLPOST = 0;  // N2 = 2
CLKDIVbits.PLLPRE = 0;   // N1 = 2
```

```text
FIN  = 8 MHz
FVCO = 8 MHz / 2 × 40 = 160 MHz
FOSC = 160 MHz / 2 = 80 MHz
FCY  = FOSC / 2 = 40 MHz
```

## Prueba incluida

Después de inicializar el hardware, el ejemplo configura RB4 como salida:

```c
TRISBbits.TRISB4 = 0;
```

El pin permanece 500 ms en alto y 500 ms en bajo, por lo que la frecuencia de salida es 1 Hz.

```text
RB4 ── 500 ms alto ── 500 ms bajo ── repetir
```

Conecta un LED con resistencia o un osciloscopio para comprobar la ejecución.

## Uso con MPLAB X

### dsPIC33FJ32MC204

1. Abre `test_dspic33fj.X`.
2. Confirma que el dispositivo del proyecto sea `dsPIC33FJ32MC204`.
3. Selecciona XC16.
4. Compila para generar el `.hex`.
5. Programa por PGD1/PGC1 o carga el HEX en Proteus.

### dsPIC33EP32MC204

1. Abre `test_dspic33ep.X`.
2. Confirma que el dispositivo sea `dsPIC33EP32MC204`.
3. Selecciona XC16.
4. Compila y programa la tarjeta por PGD3/PGC3.

## Uso con Proteus

1. Abre `test_dspic33fj32mc204.pdsprj`.
2. Compila `test_dspic33fj.X`.
3. Asigna al dsPIC simulado el archivo HEX generado en `dist/default/production/`.
4. Ejecuta la simulación y observa RB4.

La simulación está preparada para el **dsPIC33FJ32MC204**; no representa automáticamente el comportamiento del EP.

## Adaptar la plantilla

Para iniciar un proyecto nuevo:

1. Conserva la rama de Configuration Bits del MCU elegido.
2. Mantén la configuración ICSP correspondiente a la tarjeta.
3. Sustituye únicamente el bloque de aplicación dentro de `main()`.
4. Añade inicialización diferenciada bajo `#if` cuando un periférico cambie entre FJ y EP.
5. Compila ambas variantes después de cualquier cambio compartido.

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
| XC16 muestra el `#error` del dispositivo | Abre el proyecto correcto y verifica el MCU seleccionado. |
| El programador no detecta la tarjeta | Revisa si corresponde PGD1/PGC1 o PGD3/PGC3. |
| RB4 no conmuta | Comprueba reloj, alimentación y que el pin esté configurado como digital. |
| Proteus no inicia | Confirma el dispositivo FJ y la ruta del HEX recién compilado. |
| Los retardos son incorrectos | Verifica que `FCY` sea 40 MHz. |
