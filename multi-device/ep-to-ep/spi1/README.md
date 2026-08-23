CH1 amarillo = SCK1 = RC3
CH2 azul     = MOSI = RA4 de EP1
Escala real  = 1 µs/div

fSCK = 625 kHz
TSCK = 1 / 625 kHz
     = 1.6 µs



     D7 D6 D5 D4 D3 D2 D1 D0
 1  0  1  0  0  1  0  1

= 1010 0101
= 0xA5
CH1 amarillo = SCK1 = RC3
CH2 azul     = MISO = RA9 de EP1


D7 D6 D5 D4 D3 D2 D1 D0
 0  1  0  1  1  0  1  0

= 0101 1010
= 0x5A

                    8 clocks @ 625 kHz
              ┌───────────────────────┐

EP1 MOSI  ───►│ 1 0 1 0 0 1 0 1      │──► EP2
              │       0xA5            │
              │                       │
EP1 MISO  ◄───│ 0 1 0 1 1 0 1 0      │◄── EP2
              │       0x5A            │
              └───────────────────────┘


dsPIC33EP32MC204 ↔ dsPIC33EP32MC204

UART1   ✅
I²C1    ✅
SPI1    ✅

SPI1:
625 kHz
8 bits
0xA5 MOSI
0x5A MISO
full-duplex







