# Buck con control analógico

[← Volver a convertidores buck](../readme.md)

## Estado

**Proyecto planificado.** Todavía no se ha publicado un esquemático, firmware ni prototipo validado.

## Objetivo previsto

Desarrollar un convertidor buck cuya regulación principal se realice con un compensador analógico, mientras el dsPIC pueda encargarse de funciones auxiliares como:

- generación o habilitación del PWM;
- soft-start y secuencias de encendido;
- telemetría de tensión, corriente y temperatura;
- selección de consignas;
- registro de fallas y protecciones supervisoras.

## Contenido requerido antes de marcarlo como implementado

1. Especificaciones de entrada, salida y potencia.
2. Modelo de pequeña señal de la etapa de potencia.
3. Diseño del compensador y márgenes de estabilidad.
4. Esquemático, componentes y layout.
5. Procedimiento de arranque con fuente limitada en corriente.
6. Bode, respuesta transitoria, ripple, eficiencia y temperatura medidos.

> Esta página describe el alcance futuro; no es aún una guía de construcción.
