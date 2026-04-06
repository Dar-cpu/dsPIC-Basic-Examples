/* * Proyecto: DAR-CPU - Demo Motor Paso a Paso
 * Descripción: Girar un motor paso a paso (ULN2003) continuamente.
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

// Secuencia de Medio Paso
const unsigned char secuencia_motor[8] = {
    0b0001, 0b0011, 0b0010, 0b0110, 
    0b0100, 0b1100, 0b1000, 0b1001
};

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;  // 40 MIPS
    AD1PCFGL = 0xFFFF; // pines DIGITALES 
    
    TRISBbits.TRISB12 = 0;   // RB12 a RB15 como salidas
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB15 = 0;

    int paso = 0;

    while(1) {
        // Enviar el patrón de bits 
        LATBbits.LATB12 = (secuencia_motor[paso] & 0x01) ? 1 : 0;
        LATBbits.LATB13 = (secuencia_motor[paso] & 0x02) ? 1 : 0;
        LATBbits.LATB14 = (secuencia_motor[paso] & 0x04) ? 1 : 0;
        LATBbits.LATB15 = (secuencia_motor[paso] & 0x08) ? 1 : 0;
        
        paso++;
        if(paso > 7) paso = 0; // Reiniciar secuencia
        
        __delay_ms(2); // Velocidad 
    }
    return 0;
}