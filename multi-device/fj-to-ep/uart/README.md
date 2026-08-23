BRGH=0, U1BRG=21 → ≈ 113636 baud → 8.80 µs/bit
BRGH=1, U1BRG=86 → ≈ 114943 baud → 8.70 µs/bit

START + 8 DATA + STOP  
10 × 8.7 µs ≈ 87 µs
FJ -> EP

0xA5 = 1010 0101

LSB primero:
1 0 1 0 0 1 0 1

Idle  Start   D0 D1 D2 D3 D4 D5 D6 D7  Stop
 HIGH   0      1  0  1  0  0  1  0  1    1

EP -> FJ

0x5A = 0101 1010

LSB primero:
0 1 0 1 1 0 1 0

Idle  Start   D0 D1 D2 D3 D4 D5 D6 D7  Stop
 HIGH   0      0  1  0  1  1  0  1  0    1

2 × 8.7 µs ≈ 17.4 µs  


10 µs/div