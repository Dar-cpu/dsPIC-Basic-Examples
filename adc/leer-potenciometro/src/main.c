/* * Proyecto: DAR-CPU - Demo ADC 
 * Descripción: Lee un potenciómetro en AN0 y ajusta la velocidad del LED en RB11.
 */

#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>
#include <libpic30.h> 

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH
#pragma config GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

void init_ADC(void) {
    AD1CON1bits.ADON = 0;       // Apagar ADC
    AD1PCFGLbits.PCFG0 = 0;     // AN0 entrada analógica
    
    AD1CON1bits.FORM = 0;       // salida 0 a 1023
    AD1CON1bits.SSRC = 7;       // Auto-conversión
    AD1CON2bits.VCFG = 0;       // Referencias: VDD y VSS
    AD1CON3bits.SAMC = 15;      // Tiempo de muestreo (15 TAD)
    AD1CON3bits.ADCS = 63;      // Reloj ADC
    
    AD1CON1bits.ADON = 1;       // Encender ADC
}

unsigned int leer_ADC_AN0(void) {
    AD1CHS0bits.CH0SA = 0;      // Seleccionar AN0
    AD1CON1bits.SAMP = 1;       // Iniciar muestreo
    while(!AD1CON1bits.DONE);   // Esperar a que termine la conversión
    return ADC1BUF0;            // Valor (0 a 1023)
}

int main(void) {
    PLLFBD = 38;       // PLL 40 MIPS        
    CLKDIVbits.PLLPOST = 0;  
    CLKDIVbits.PLLPRE = 0;   
    
    TRISAbits.TRISA0 = 1;   // AN0 entrada
    TRISBbits.TRISB11 = 0;  // RB11 (LED)
    init_ADC();
    
    unsigned int valor_pot;
    
    while(1) {   // Variar tiempo de encendido de LED
        valor_pot = leer_ADC_AN0(); 
        
        LATBbits.LATB11 = 1;
        for(int i = 0; i < (valor_pot / 10) + 1; i++){
            __delay_ms(10); 
        }
        LATBbits.LATB11 = 0;
        for(int i = 0; i < (valor_pot / 10) + 1; i++){
            __delay_ms(10); 
        }
    }
    return 0;
}