# dsPIC33FJ32MC204 - ADC + TFT Oscilloscope

Ejemplo simple para usar el ADC del dsPIC y visualizar una señal analógica en una TFT ST7735 de 128x160.

## Configuración

- dsPIC33FJ32MC204
- cristal externo: 8 MHz
- FOSC: 80 MHz
- FCY: 40 MHz
- ADC: 12 bits
- entrada: AN6 / RC0 / TQFP pin 25
- VREF+: AVDD
- VREF-: AVSS
- muestreo: 200 kS/s
- trigger ADC: Timer3
- captura: 256 muestras
- visualización: 128 muestras
- TFT: ST7735 128x160 por SPI1 a 10 MHz

## TFT

La conexión es la misma del ejemplo anterior:

| TFT | dsPIC33FJ32MC204 |
|---|---|
| GND | GND |
| VCC | 3.3 V |
| SCL/SCK | RC5 / RP21 / pin 38 |
| SDA/MOSI | RC4 / RP20 / pin 37 |
| RES | RC3 / pin 36 |
| DC | RB12 / pin 10 |
| CS | RB13 / pin 11 |
| BL | 3.3 V |

## Entrada ADC

```text
Generador OUT ---- 1k ---- AN6 / RC0 / pin 25
Generador GND ------------ GND del dsPIC
```

La señal debe mantenerse dentro del rango analógico del dsPIC.

Para la prueba actual:

```text
WAVE   = SINE
FREQ   = 10 kHz
AMPL   = 1.2 V configurados en FY6900
OFFSET = 1.65 V
```

Antes de conectar al dsPIC, comprobar con osciloscopio que la señal completa permanece dentro de 0 V y 3.3 V aproximadamente.

## Muestreo

Timer3 funciona con FCY = 40 MHz:

```text
PR3 = 199
Fs = 40 MHz / (199 + 1)
Fs = 200 kS/s
Ts = 5 us
```

ADC 12 bits:

```text
ADCS = 5
TAD = 25 ns * (5 + 1)
TAD = 150 ns

Tconv = 14 * TAD
Tconv = 2.10 us
```

El datasheet requiere TAD >= 117.6 ns en 12 bits.

## Qué se ve

La TFT usa escala vertical fija:

```text
arriba  = ADC 4095 ~= AVDD
centro  = ADC 2048 ~= AVDD/2
abajo   = ADC 0    ~= 0 V
```

La línea azul horizontal marca media escala (~1.65 V si AVDD = 3.3 V).

La forma de onda se dibuja en amarillo.

## Trigger

Se capturan 256 muestras y se obtiene:

```text
threshold = (min + max) / 2
```

Después se busca el primer cruce ascendente por ese nivel y se muestran 128 puntos desde allí.

Esto ayuda a que una señal periódica se vea más estable sin necesitar un trigger externo.

## Archivos

- `main.c`
- `config_bits.c`
- `adc_scope.c`
- `adc_scope.h`
- `st7735.c`
- `st7735.h`

No se necesita `microchip.h` para este ejemplo.
