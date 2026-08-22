# Plantilla universal para dsPIC33FJ32MC204 y dsPIC33EP32MC204

[← Volver al índice de ejemplos](../README.md)

Esta carpeta contiene una plantilla mínima de código para trabajar con los microcontroladores **dsPIC33FJ32MC204** y **dsPIC33EP32MC204** usados en la tarjeta.

La intención de esta sección es mantener el repositorio limpio y fácil de consultar: se conserva únicamente el archivo fuente principal `main.c` y, como referencia, los archivos compilados dentro de `dist/default/production/`.

> Esta carpeta no pretende contener proyectos MPLAB X completos. Los archivos internos generados por el IDE, configuraciones locales, cachés y directorios de compilación que no aportan al aprendizaje se han eliminado deliberadamente.

## Estructura actual

```text
universal-template/
├── README.md
├── test_dspic33fj.X/
│   ├── main.c
│   └── dist/
│       └── default/
│           └── production/
│               ├── test_fj.X.production.hex
│               └── test_fj.X.production.elf
│
└── test_dspic33ep.X/
    ├── main.c
    └── dist/
        └── default/
            └── production/
                ├── test_ep.X.production.hex
                └── test_ep.X.production.elf
```

## Qué archivo utilizar

| Archivo | Uso |
| --- | --- |
| `main.c` | Código fuente principal. Es el archivo recomendado para estudiar, modificar o copiar a un proyecto propio de MPLAB X. |
| `.hex` | Firmware ya compilado que puede cargarse directamente al microcontrolador con MPLAB X IPE/IDE y un programador compatible. |
| `.elf` | Archivo generado por el compilador con información útil para depuración y herramientas de desarrollo. |

Para desarrollar un nuevo ejemplo, crea un proyecto MPLAB X para el dispositivo correspondiente y utiliza el `main.c` de esta carpeta como punto de partida.

## Dispositivos

| Carpeta | Microcontrolador | ICSP configurado |
| --- | --- | --- |
| `test_dspic33fj.X/` | dsPIC33FJ32MC204 | PGD1 / PGC1 |
| `test_dspic33ep.X/` | dsPIC33EP32MC204 | PGD3 / PGC3 |

El canal ICSP es diferente entre ambos dispositivos en esta tarjeta. No cambies el Configuration Bit `ICS` sin comprobar primero el hardware.

## Plantilla compartida

Los dos `main.c` utilizan compilación condicional para mantener una misma base de código y aplicar únicamente las diferencias necesarias entre familias.

### GPIO digital

La principal diferencia de inicialización básica es la configuración de las entradas analógicas.

```c
#if defined(__dsPIC33FJ32MC204__)
    AD1PCFGL = 0xFFFF;

#elif defined(__dsPIC33EP32MC204__)
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
#endif
```

| Familia | Registros utilizados |
| --- | --- |
| dsPIC33FJ | `AD1PCFGL` |
| dsPIC33EP | `ANSELA`, `ANSELB`, `ANSELC` |

## Reloj de la plantilla

La configuración base utiliza un cristal externo de **8 MHz** y PLL.

```c
PLLFBD = 38;
CLKDIVbits.PLLPOST = 0;
CLKDIVbits.PLLPRE = 0;
```

La configuración utilizada corresponde a:

```text
FIN  = 8 MHz
FVCO = 8 MHz / 2 × 40 = 160 MHz
FOSC = 160 MHz / 2 = 80 MHz
FCY  = FOSC / 2 = 40 MHz
```

Por tanto, la plantilla trabaja con:

- `FOSC = 80 MHz`
- `FCY = 40 MHz`
- `40 MIPS`

## Ejemplo base incluido

El código actual utiliza `RB4` como una prueba mínima de ejecución:

```c
TRISBbits.TRISB4 = 0;

while (1)
{
    LATBbits.LATB4 = 1;
    __delay_ms(500);

    LATBbits.LATB4 = 0;
    __delay_ms(500);
}
```

El resultado esperado es una señal de **1 Hz**:

```text
RB4 ── 500 ms HIGH ── 500 ms LOW ── repetir
```

Puedes comprobarla con un LED y una resistencia, un osciloscopio o un analizador lógico.

## Cómo usar el código fuente

### dsPIC33FJ32MC204

1. Crea un proyecto nuevo en MPLAB X.
2. Selecciona `dsPIC33FJ32MC204` como dispositivo.
3. Selecciona el compilador XC16.
4. Copia el contenido de `test_dspic33fj.X/main.c` al proyecto.
5. Compila y programa mediante PGD1/PGC1.

### dsPIC33EP32MC204

1. Crea un proyecto nuevo en MPLAB X.
2. Selecciona `dsPIC33EP32MC204` como dispositivo.
3. Selecciona el compilador XC16.
4. Copia el contenido de `test_dspic33ep.X/main.c` al proyecto.
5. Compila y programa mediante PGD3/PGC3.

## Programar directamente el firmware compilado

Si solo quieres ejecutar el ejemplo sin modificar el código, puedes utilizar el archivo `.hex` correspondiente:

```text
dsPIC33FJ32MC204
└── test_dspic33fj.X/dist/default/production/test_fj.X.production.hex

dsPIC33EP32MC204
└── test_dspic33ep.X/dist/default/production/test_ep.X.production.hex
```

Estos archivos permiten conservar una versión compilada del ejemplo junto al código fuente que la genera.

## Crear nuevos ejemplos

La plantilla está pensada como punto de partida. Para cada nuevo ejemplo:

1. Parte del `main.c` correspondiente al microcontrolador.
2. Conserva los Configuration Bits necesarios para la tarjeta.
3. Mantén el canal ICSP correcto para el dispositivo.
4. Añade únicamente la inicialización y el código del periférico que se desea demostrar.
5. Verifica el ejemplo físicamente antes de incorporarlo al repositorio público.

El objetivo es que cada carpeta del repositorio sea pequeña, clara y centrada en el código necesario para aprender y utilizar una función concreta del dsPIC.
