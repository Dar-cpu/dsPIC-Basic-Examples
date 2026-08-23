dsPIC33FJ32MC204                    dsPIC33EP32MC204
MASTER                              SLAVE

RC5 / RP21 / pin 38  SCK  ───────► RC3 / pin 36  SCK1

RC4 / RP20 / pin 37  SDO  ───────► RA9 / pin 35  SDI1
                         MOSI

RC3 / RP19 / pin 36  SDI  ◄─────── RA4 / pin 34  SDO1
                         MISO

RB13 / pin 11         CS  ───────► RB0 / pin 21  SS1

GND                       ──────── GND   


FJ: RB4 / pin 33
EP: RB4 / pin 33    

CH1 amarillo = SCK
CH2 azul     = MOSI
Escala real  = 5 µs/div




fSCK = 625.000 kHz
T    = 1.600 µs





FCY = 40 MHz
Primary prescaler   = 16:1
Secondary prescaler = 4:1

40 MHz / 16 / 4 = 625 kHz

0xA5 = 1010 0101

D7 D6 D5 D4 D3 D2 D1 D0
 1  0  1  0  0  1  0  1



0x5A = 0101 1010

D7 D6 D5 D4 D3 D2 D1 D0
 0  1  0  1  1  0  1  0



                  8 clocks a 625 kHz
             ┌──────────────────────┐

SCK   ______/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\_/‾\____

MOSI      1   0   1   0   0   1   0   1
          └──────────── 0xA5 ────────────┘
                 FJ → EP

MISO      0   1   0   1   1   0   1   0
          └──────────── 0x5A ────────────┘
                 EP → FJ




                 







