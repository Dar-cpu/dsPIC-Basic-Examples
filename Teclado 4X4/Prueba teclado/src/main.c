/* * Proyecto: DAR-CPU - Demo Teclado Matricial 4x4
 * Descripción: Lee el teclado y parpadea el LED en RB11 según la tecla
 */

#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>
#include <libpic30.h> 

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH, GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

const char teclas[4][4] = { // Mapa del teclado
    {1, 2, 3, 10}, // 10 = 'A'
    {4, 5, 6, 11}, // 11 = 'B'
    {7, 8, 9, 12}, // 12 = 'C'
    {14, 0, 15, 13} // 14='*', 15='#', 13='D'
};

char leer_teclado(void) {
    int fila, col;
    for(fila = 0; fila < 4; fila++) {
        LATB &= 0xFFF0; // RB0-RB3 en 0

        LATB |= (1 << fila); // Encender solo la fila actual
        __delay_ms(5);     
        int columnas = (PORTB >> 4) & 0x0F;     // Leer las columnas
        
        if(columnas != 0) {
            for(col = 0; col < 4; col++) {
                if(columnas & (1 << col)) {
                    while((PORTB >> 4) & 0x0F); // Antirrebote
                    return teclas[fila][col];
                }
            }
        }
    }
    return -1; // Ninguna tecla presionada
}

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;
    AD1PCFGL = 0xFFFF; // Pines digitales
    
    TRISB = (TRISB & 0xFF00) | 0x00F0;  // RB0-RB3 (Filas), RB4-RB7 (Columnas)
    TRISBbits.TRISB11 = 0; // LED 
    LATBbits.LATB11 = 0;

    while(1) {
        char tecla = leer_teclado(); 
        if(tecla != -1 && tecla > 0) {
            for(int i = 0; i < tecla; i++) { // Si presionó una tecla, parpadear el LED esa cantidad de veces
                LATBbits.LATB11 = 1;
                __delay_ms(200);
                LATBbits.LATB11 = 0;
                __delay_ms(200);
            }
            __delay_ms(1000);
        }
    }
    return 0;
}