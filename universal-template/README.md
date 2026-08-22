# Plantilla universal para dsPIC33FJ32MC204 y dsPIC33EP32MC204

[← Volver al inicio del repositorio](../README.md)

Esta carpeta contiene un único archivo fuente que sirve como punto de partida para los dos microcontroladores compatibles con la tarjeta de desarrollo TECKIO dsPIC33 V1I2.

## Estructura

```text
universal-template/
├── README.md
└── main.c
```

La plantilla no contiene un proyecto completo de MPLAB X ni archivos generados por el compilador. Para utilizarla, crea un proyecto nuevo y añade `main.c`.

## Selección automática del dispositivo

MPLAB X define la macro del dispositivo elegido en las propiedades del proyecto. El preprocesador compila solamente el bloque correspondiente:

```c
#if defined(__dsPIC33FJ32MC204__)
    /* Configuración del FJ */
#elif defined(__dsPIC33EP32MC204__)
    /* Configuración del EP */
#else
    #error "Seleccione dsPIC33FJ32MC204 o dsPIC33EP32MC204 en el proyecto."
#endif
```

El mismo archivo puede copiarse a ambos proyectos; no es necesario mantener dos versiones.

## Configuración incluida

| Función | dsPIC33FJ32MC204 | dsPIC33EP32MC204 |
| --- | --- | --- |
| Entradas analógicas desactivadas | `AD1PCFGL` | `ANSELA`, `ANSELB`, `ANSELC` |
| ICSP de la tarjeta | PGD1 / PGC1 | PGD3 / PGC3 |
| Cristal externo | 8 MHz | 8 MHz |
| Frecuencia objetivo | `FCY = 40 MHz` | `FCY = 40 MHz` |
| Watchdog | Desactivado | Desactivado |
| JTAG | Desactivado | Desactivado |

## Prueba mínima incluida

El `main.c` configura RB4 como salida y conmuta su estado cada 500 ms. Esta operación funciona como una comprobación básica de que el proyecto se compiló, el reloj se inició y el microcontrolador está ejecutando código.

```text
RB4: 500 ms HIGH + 500 ms LOW
Periodo aproximado: 1 s
Frecuencia aproximada: 1 Hz
```

Puedes reemplazar directamente ese bloque por el código de tu aplicación.

## Cómo utilizarla

### dsPIC33FJ32MC204

1. Crea un proyecto nuevo para `dsPIC33FJ32MC204`.
2. Selecciona XC16.
3. Añade [`main.c`](main.c).
4. Conserva `ICS = PGD1`.
5. Compila y programa mediante PGD1 / PGC1.

### dsPIC33EP32MC204

1. Crea un proyecto nuevo para `dsPIC33EP32MC204`.
2. Selecciona XC16.
3. Añade [`main.c`](main.c).
4. Conserva `ICS = PGD3`.
5. Compila y programa mediante PGD3 / PGC3.

El bloque que no corresponde al dispositivo seleccionado aparecerá inactivo en el editor y no formará parte de la compilación.

## Por qué no se incluyen archivos compilados

Los archivos `.hex` y `.elf` dependen del dispositivo, la versión del código y la configuración del proyecto. Como esta carpeta funciona como plantilla y no como ejemplo final, conservar únicamente el código fuente evita confundir binarios del FJ con binarios del EP.

Si un ejemplo futuro necesita firmware listo para programar, el archivo compilado deberá publicarse dentro de la carpeta de ese ejemplo o como una versión verificable en GitHub Releases.

## Crear nuevos ejemplos

1. Parte de este `main.c`.
2. Selecciona el microcontrolador correcto en MPLAB X.
3. Conserva el canal ICSP correspondiente.
4. Sustituye la prueba de RB4 por el periférico que deseas demostrar.
5. Verifica el resultado físicamente.
6. Publica el ejemplo dentro de la carpeta específica del dispositivo.
