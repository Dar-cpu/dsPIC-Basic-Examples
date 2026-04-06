/* * Proyecto: DAR-CPU - Demo Timer 1 por Interrupciones
 * Descripción: Parpadeo exacto de 1 segundo (1Hz) sin usar delay_ms()
 */

#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH
#pragma config GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

volatile int contador_100ms = 0;

// Vector de Interrupción del Timer 1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    contador_100ms++;

    if(contador_100ms >= 5) { // 5 interrupciones (500 ms)
        LATBbits.LATB11 ^= 1; // Invertir LED
        contador_100ms = 0;   // Reiniciar contador
    }
    
    _T1IF = 0; // Limpiar flag
}

void init_Timer1(void) {
    T1CONbits.TON = 0;      // Apagar Timer 
    T1CONbits.TCS = 0;      // Usar reloj interno (FCY = 40MHz) - reloj derivado de cristal externo 
    T1CONbits.TCKPS = 3;    // Prescaler 1:256
    
    // Frecuencia del Timer = 40,000,000 / 256 = 156,250 Hz
    // Una interrupción cada 100ms, 156,250 * 0.1 = 15625
    PR1 = 15625;            // Period Register
    
    _T1IP = 3;              // Prioridad 
    _T1IF = 0;              // Limpiar flag
    _T1IE = 1;              // interrupción Timer 1
    
    T1CONbits.TON = 1;      // Encender Timer
}

int main(void) {
    PLLFBD = 38;    // PLL para 40 MIPS         
    CLKDIVbits.PLLPOST = 0;  
    CLKDIVbits.PLLPRE = 0;   
    
    TRISBbits.TRISB11 = 0;  // RB11 (LED)
    LATBbits.LATB11 = 0;
    
    init_Timer1();
    
    while(1) {
        // bucle principal libre
        // LED parpadeará en el fondo por el Timer.
    }
    return 0;
}