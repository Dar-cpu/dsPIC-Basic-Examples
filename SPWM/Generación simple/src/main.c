/* Guía Rápida DAR-CPU  */

#define FOSC  80000000UL
#define FCY   40000000UL  // 40 MIPS 

#include <xc.h>
#include <stdio.h>
#include <libpic30.h> 

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH
#pragma config GWRP = OFF, GSS = OFF

#pragma config FNOSC = PRIPLL           // Usar Cristal Primario + PLL
#pragma config IESO = ON                // Two-Speed Start-up 
#pragma config POSCMD = XT              // Modo XT para cristal de 8MHz
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 

#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

// TABLA DE SENO SPWM 
const unsigned int sine_table[] = {
    512, 612, 709, 799, 878, 944, 992, 1021, 1023, 1005, 966, 907, 832, 745, 650, 550,
    449, 349, 254, 167, 92, 33, 7, 0, 9, 41, 91, 157, 237, 327, 424, 524
};
volatile int sine_index = 0;

// INTERRUPCIÓN DEL PWM
void __attribute__((interrupt, no_auto_psv)) _MPWM1Interrupt(void) {
    PDC1 = sine_table[sine_index]; 
    sine_index++;
    if(sine_index >= 32) sine_index = 0; 
    _PWM1IF = 0; 
}

// CONFIGURACIONES HARDWARE 
void init_PWM(void) {
    PTPER = 1023;            // Frecuencia portadora ~19.5 kHz
    
    PWMCON1bits.PMOD1 = 0;   // Modo Complementario
    PWMCON1bits.PEN1H = 1;   // Salida SPWM en pin RB14
    PWMCON1bits.PEN1L = 1;   // Salida SPWM en pin RB15
    
    PDC1 = 512;              // 50% de Duty Cycle inicial
    
    _PWM1IP = 4;             // Prioridad de interrupción
    _PWM1IE = 1;             // Habilitar interrupción PWM
    
    PTCONbits.PTEN = 1;      // Encender el módulo PWM
}
 
// PROGRAMA PRINCIPAL 
int main(void) {
    // Ajustar PLL para el cristal de 8MH
    // Fórmula VCO: 8MHz / N1(2) * M(40) = 160MHz
    // Fórmula Fosc: 160MHz / N2(2) = 80MHz o 40 MIPS
    PLLFBD = 38;             // M = 40
    CLKDIVbits.PLLPOST = 0;  // N2 = 2
    CLKDIVbits.PLLPRE = 0;   // N1 = 2
    
    // Con IESO=ON, el hardware estabiliza el cristal y hace el cambio automáticamente en segundo plano

    init_PWM();              

    TRISBbits.TRISB11 = 0;        // Configurar el pin del LED como salida
    LATBbits.LATB11 = 0;
    
    while(1) {
        LATBbits.LATB11 ^= 1;
        __delay_ms(50);      
    }
    return 0;
}