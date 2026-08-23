CH1 amarillo = EP1 RC8 / U1TX = 0xA5
CH2 azul     = EP1 RC9 / U1RX = 0x5A

EP2 ── 0x5A ──► EP1 RC9 / U1RX

}
UART idle = HIGH ≈ 3.3 V
START     = LOW
DATA      = 8 bits
STOP      = HIGH



Baud real ≈ 114943 baud
Tbit      ≈ 8.70 µs
           ≈ 0.87 divisiones a 10 µs/div

Trama 8N1:
10 bits ≈ 87 µs


EP1 TX → EP2 RX

0xA5 = 1010 0101

UART envía LSB primero:

START D0 D1 D2 D3 D4 D5 D6 D7 STOP
  0    1  0  1  0  0  1  0  1   1

CH2 azul = RC9 del EP1 = U1RX



EP2 TX → EP1 RX

0x5A = 0101 1010

START D0 D1 D2 D3 D4 D5 D6 D7 STOP
  0    0  1  0  1  1  0  1  0   1

2 × 8.70 µs ≈ 17.4 µs



