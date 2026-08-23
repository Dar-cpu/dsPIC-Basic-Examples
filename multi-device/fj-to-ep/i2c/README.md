CH1 amarillo = SDA = RB9 del FJ
CH2 azul     = SCL = RB8 del FJ   

FJ = Master
EP = Slave

FJ escribe 0xA5  ─────► EP
                         └─ conmuta LED

FJ lee 0x5A      ◄───── EP
└─ prende LED 100 ms  


dsPIC33FJ32MC204                  dsPIC33EP32MC204

pin 1  RB9 / SDA1 ────────────── pin 37 RC4 / SDA1
pin 44 RB8 / SCL1 ────────────── pin 38 RC5 / SCL1

GND               ────────────── GND    


                 3.3 V
                  │
                4.7 kΩ
                  │
SDA ──────────────┼────────────── SDA


                3.3 V
                  │
                4.7 kΩ
                  │
SCL ──────────────┼────────────── SCL   



1) FJ escribe al EP

START
  ↓
0x84          ACK
│              │
└─ 0x42 + W ───┘
  ↓
0xA5          ACK
  ↓
STOP  



2) FJ lee del EP

START
  ↓
0x85          ACK
│              │
└─ 0x42 + R ───┘
  ↓
0x5A          NACK
  ↓
STOP


