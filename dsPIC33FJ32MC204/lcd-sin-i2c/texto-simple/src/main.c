/* * Proyecto: DAR-CPU - Demo LCD 16x2 (Modo 4-bits)
 * Descripción: Escribe un mensaje de bienvenida en la pantalla.
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

#define LCD_RS LATBbits.LATB4
#define LCD_EN LATBbits.LATB5

void LCD_Pulse_EN(void) {
    LCD_EN = 1;
    __delay_us(50);
    LCD_EN = 0;
    __delay_us(50);
}

void LCD_Nibble(unsigned char data) {
    LATB = (LATB & 0xFC3F) | ((data & 0x0F) << 6);  // 4 bits bajos a RB6 a RB9
    LCD_Pulse_EN();
}

void LCD_Cmd(unsigned char cmd) {
    LCD_RS = 0; // Modo Comando
    LCD_Nibble(cmd >> 4); // Parte Alta
    LCD_Nibble(cmd);      // Parte Baja
    __delay_ms(2);
}

void LCD_Char(char data) {
    LCD_RS = 1; // Modo Datos
    LCD_Nibble(data >> 4);
    LCD_Nibble(data);
    __delay_us(50);
}

void LCD_String(char *str) {
    while(*str) {
        LCD_Char(*str++);
    }
}

void LCD_Init(void) {
    __delay_ms(20);
    LCD_RS = 0;

    LCD_Nibble(0x03); __delay_ms(5);   // inicialización a 4 bits
    LCD_Nibble(0x03); __delay_us(150);
    LCD_Nibble(0x03); __delay_ms(2);
    LCD_Nibble(0x02); __delay_ms(2); // Entrar en modo 4 bits
    
    LCD_Cmd(0x28); // 4-bits, 2 líneas, 5x8
    LCD_Cmd(0x0C); // Display ON, Cursor OFF
    LCD_Cmd(0x06); // Auto-incremento
    LCD_Cmd(0x01); // Limpiar pantalla
}

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;
    AD1PCFGL = 0xFFFF; // Pines digitales
    TRISB &= 0xFC0F;  // RB4 a RB9 
    
    LCD_Init();
    LCD_Cmd(0x80); // Fila 1, Columna 1
    LCD_String("DAR-CPU dsPIC33F");
    LCD_Cmd(0xC0); // Fila 2, Columna 1
    LCD_String("Lista para usar!");

    while(1) {
        // bucle limpio 
    }
    return 0;
}