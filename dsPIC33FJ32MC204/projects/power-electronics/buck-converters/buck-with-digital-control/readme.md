# Buck con control digital

[← Volver a convertidores buck](../readme.md)

## Estado

**Proyecto planificado.** Todavía no se ha publicado un esquemático, firmware de control ni prototipo validado.

## Objetivo previsto

Desarrollar un convertidor buck regulado por el dsPIC, integrando:

- disparo sincronizado del ADC con el PWM;
- muestreo de tensión y corriente;
- compensador digital ejecutado a periodo fijo;
- actualización segura del duty cycle;
- soft-start, saturación y anti-windup;
- protecciones rápidas de hardware y supervisión en firmware;
- telemetría y captura de respuesta dinámica.

## Contenido requerido antes de marcarlo como implementado

1. Especificaciones y etapa de potencia calculada.
2. Modelo continuo y equivalente discreto.
3. Frecuencia de muestreo, retardos y resolución cuantificados.
4. Diseño del compensador y análisis de estabilidad.
5. Esquemático, PCB y estrategia de puesta a tierra.
6. Firmware reproducible y procedimiento de puesta en marcha.
7. Mediciones de respuesta transitoria, ripple, eficiencia y temperatura.

> Esta página define el alcance del trabajo futuro; no representa todavía un convertidor listo para construcción.
