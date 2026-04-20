/*
 * ============================================================================
 * PLANTILLA UNIVERSAL: SIMULACIÓN en Proteus / Tarjeta física 
 * Frecuencia: 40 MIPS (Cristal Externo 8 MHz + PLL)
 * ============================================================================
 * Instrucciones:
 * 1. Para simular en Proteus: Seleccione el dsPIC33FJ32MC204 en MPLAB X
 * 2. Para grabar la placa física: Seleccione el dsPIC33EP32MC204 en MPLAB X
 * ============================================================================
 */

#include <xc.h>   
#define FCY 40000000UL  // 40 MIPS (Fosc/2)
#include <libpic30.h> 

#if defined(__dsPIC33FJ32MC204__) /* dsPIC33FJ - SIMULACIÓN PROTEUS */
    #pragma config POSCMD = HS      // Cristal de alta velocidad
    #pragma config FNOSC = PRIPLL   // Arranque Cristal + PLL
    #pragma config IESO = OFF       
    #pragma config FCKSM = CSDCMD   // Cambio de reloj deshabilitado
    #pragma config OSCIOFNC = OFF   // OSC2 retorno de cristal
    #pragma config FWDTEN = OFF     
    #pragma config JTAGEN = OFF     
    #pragma config ICS = PGD1       

#elif defined(__dsPIC33EP32MC204__) /* dsPIC33EP - PLACA FÍSICA */
    
    #pragma config ICS = PGD3       // No Cambiar el puerto de programación o podrías perder comunicación con tu dsPIC!!
    #pragma config JTAGEN = OFF     // No activar
    #pragma config ALTI2C1 = OFF    
    #pragma config ALTI2C2 = OFF
    #pragma config PLLKEN = ON           
    #pragma config FWDTEN = OFF     

    #pragma config POSCMD = HS      // Cristal de alta velocidad
    #pragma config FNOSC = PRIPLL   // Arranque Cristal + PLL
    #pragma config IESO = OFF       
    #pragma config OSCIOFNC = OFF   // OSC2 retorno de cristal
    #pragma config IOL1WAY = OFF    
    #pragma config FCKSM = CSDCMD   // Cambio de reloj deshabilitado
#else
    #error "Seleccione dsPIC33FJ32MC204 o dsPIC33EP32MC204 en el proyecto."
#endif

void Init_Hardware_Base(void) {  // Ajustar según registros de cada dsPIC, por defecto está configurado para pines digitales.
    #if defined(__dsPIC33FJ32MC204__)
        AD1PCFGL = 0xFFFF; // Pines a digital en FJ
    #elif defined(__dsPIC33EP32MC204__)
        ANSELA = 0x0000;   // Pines a digital en EP
        ANSELB = 0x0000;   
        ANSELC = 0x0000;   
    #endif
}

// PROGRAMA PRINCIPAL
int main(void) {
    // Cálculos del PLL (Para 40 MIPS con 8 MHz), se aplican directamente a los registros en ambos micros
    PLLFBD = 38;             // M = 40 
    CLKDIVbits.PLLPOST = 0;  // N2 = 2 
    CLKDIVbits.PLLPRE = 0;   // N1 = 2 
    
    Init_Hardware_Base();   // Iniciar pines analógicos/digitales
    
    // Código a ejecutar 
    TRISBbits.TRISB4 = 0;    // RB4 salida
    
    while(1) {
        LATBbits.LATB4 = 1;  
        __delay_ms(500);     
        LATBbits.LATB4 = 0;  
        __delay_ms(500);     
    }
    return 0;
}