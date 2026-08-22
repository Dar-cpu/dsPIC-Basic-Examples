/* DAR-CPU - Demo UART 
 * Envía un mensaje a la PC por puerto serial a 9600 baudios
 */

#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>
#include <libpic30.h> 

// Cristal 8MHz
#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH
#pragma config GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

// Función enviar texto 
void UART_Write_String(char *texto) {
    while(*texto) {
        while(U1STAbits.UTXBF); // Esperar a que el buffer esté libre
        U1TXREG = *texto++;     // Enviar caracter
    }
}
 
void init_UART(void) {
    __builtin_write_OSCCONL(OSCCON & ~(1<<6)); // Configurar Pines Remapeables - Desbloquear PPS
    RPINR18bits.U1RXR = 0;                     // U1RX al pin RP0 (RB0)
    RPOR0bits.RP1R = 3;                        // U1TX al pin RP1 (RB1) 
    __builtin_write_OSCCONL(OSCCON | (1<<6));  // Bloquear PPS
    
    // Baud Rate (9600 a 40 MIPS)
    // BRG = (FCY / (16 * Baud Rate)) - 1 = (40000000 / (16 * 9600)) - 1 = 259.4
    U1BRG = 259;
 
    U1MODEbits.UARTEN = 1;  // Habilitar UART (8b, sin paridad, 1 bit stop)
    U1STAbits.UTXEN = 1;   // Habilitar transmisión
}

int main(void) { 
    PLLFBD = 38;       // PLL para 40 MIPS       
    CLKDIVbits.PLLPOST = 0;  
    CLKDIVbits.PLLPRE = 0;   
    
    TRISBbits.TRISB11 = 0; // Led
    init_UART();
    
    while(1) {
        UART_Write_String("Hola desde la tarjeta DAR-CPU a 40 MIPS!\r\n");
        LATBbits.LATB11 ^= 1;
        __delay_ms(1000);    
    }
    return 0;
}