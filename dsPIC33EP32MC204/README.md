# Ejemplos para dsPIC33EP32MC204

[← Volver al inicio del repositorio](../README.md)

Esta carpeta está reservada para ejemplos desarrollados y verificados específicamente con el **Microchip dsPIC33EP32MC204** montado en la tarjeta de desarrollo TECKIO dsPIC33 V1I2.

## Estado actual

La compatibilidad básica del dispositivo ya fue comprobada mediante la [plantilla universal](../universal-template), que configura el cristal externo de 8 MHz, el PLL, los GPIO digitales y el canal de programación correspondiente.

Por ahora no se duplican aquí los ejemplos del dsPIC33FJ32MC204. Aunque ambos microcontroladores pueden utilizar la misma tarjeta, existen diferencias en:

- Configuration Bits.
- Registros de selección analógica.
- Inicialización y características de algunos periféricos.
- Peripheral Pin Select.
- Canal ICSP utilizado por la PCB.

| Parámetro | Configuración |
| --- | --- |
| Dispositivo | dsPIC33EP32MC204 |
| Cristal | 8 MHz externo |
| Frecuencia objetivo de la plantilla | `FCY = 40 MHz` |
| Programación y depuración | PGD3 / PGC3 |
| Compilador | XC16 |

> No cambies el Configuration Bit `ICS = PGD3` sin revisar el diseño de la tarjeta, porque podrías perder la comunicación con el programador por el conector utilizado.

## Estructura prevista

Las categorías se crearán conforme se validen ejemplos reales:

```text
dsPIC33EP32MC204/
├── README.md
├── gpio/
├── adc/
├── uart/
├── timer/
├── pwm/
├── spi/
└── i2c/
```

Cada incorporación deberá incluir como mínimo:

- Código específico para el dsPIC33EP32MC204.
- Configuration Bits correctos.
- Pines y conexiones documentados.
- Procedimiento de compilación y prueba.
- Verificación en hardware antes de publicarse.

## Comenzar un proyecto

1. Crea un **Standalone Project** en MPLAB X.
2. Selecciona `dsPIC33EP32MC204`.
3. Selecciona XC16.
4. Copia [`universal-template/main.c`](../universal-template/main.c).
5. Conserva PGD3 / PGC3 para la programación de esta tarjeta.
6. Añade únicamente la inicialización correspondiente al periférico que utilizarás.
