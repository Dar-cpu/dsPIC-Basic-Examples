/* * Proyecto: Parpadeo de prueba en RB11 (1Hz y 2Hz)
 * Descripción: Ciclos de 5 segundos intercambiando velocidades.
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

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0; // Reloj a 40 MIPS
    AD1PCFGL = 0xFFFF;  // Pines como digitales
    TRISBbits.TRISB11 = 0; // RB11 salida
    LATBbits.LATB11 = 0;   // Iniciar apagado

    while(1) {
        
        // 5 segundos a 1 Hz - 10 cambios de estado * 500 ms = 5000 ms
        for(int i = 0; i < 10; i++) {
            LATBbits.LATB11 ^= 1;  // Invertir LED o Toggle
            __delay_ms(500);     
        }

        // 5 segundos a 2 Hz - 20 cambios de estado * 250 ms = 5000 ms
        for(int i = 0; i < 20; i++) {
            LATBbits.LATB11 ^= 1;  // Toggle
            __delay_ms(250);       
        } 
    }
    return 0;
}