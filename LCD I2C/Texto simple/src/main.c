/* * Proyecto: DAR-CPU - Demo LCD 16x2 (Modo I2C)
 * Descripción: Escribe un mensaje de bienvenida usando el módulo I2C (PCF8574)
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

#define LCD_ADDR 0x4E  // PCF8574T usa 0x4E (0x27 desplazado)

// DRIVERS I2C BÁSICOS
void I2C_Init(void) {
    I2C1BRG = 95;   // baudios para 400 kHz a 40 MIPS
    I2C1CONbits.I2CEN = 1; // Encender I2C
}

void I2C_Start(void) { I2C1CONbits.SEN = 1; while(I2C1CONbits.SEN); }
void I2C_Stop(void)  { I2C1CONbits.PEN = 1; while(I2C1CONbits.PEN); }
void I2C_Write(unsigned char data) { I2C1TRN = data; while(I2C1STATbits.TRSTAT); }

// DRIVERS LCD I2C
void LCD_Nibble(unsigned char nibble, unsigned char rs) {
    unsigned char data_byte = (nibble << 4) | (rs ? 1 : 0) | 0x08;  // PCF8574: [D7][D6][D5][D4] [Backlight][EN][RW][RS], 0x08 - Backlight ON
    
    I2C_Start();
    I2C_Write(LCD_ADDR);
    I2C_Write(data_byte | 0x04); // EN = 1 (0x04), pulso EN 
    __delay_us(1);
    I2C_Write(data_byte & ~0x04); // EN = 0
    I2C_Stop();
    __delay_us(50);
}

void LCD_Cmd(unsigned char cmd) {
    LCD_Nibble(cmd >> 4, 0); // Parte Alta, RS=0 
    LCD_Nibble(cmd & 0x0F, 0); // Parte Baja, RS=0 
    __delay_ms(2);
}

void LCD_Char(char data) {
    LCD_Nibble(data >> 4, 1); // Parte Alta, RS=1 
    LCD_Nibble(data & 0x0F, 1); // Parte Baja, RS=1 
    __delay_us(50);
}

void LCD_String(char *str) {
    while(*str) {
        LCD_Char(*str++);
    }
}

void LCD_Init(void) {
    __delay_ms(20);
    LCD_Nibble(0x03, 0); __delay_ms(5); // inicialización a 4 bits I2C
    LCD_Nibble(0x03, 0); __delay_us(150);
    LCD_Nibble(0x03, 0); __delay_ms(2);
    LCD_Nibble(0x02, 0); __delay_ms(2); // Entrar en modo 4 bits
    
    LCD_Cmd(0x28); // 4-bits, 2 líneas, 5x8
    LCD_Cmd(0x0C); // Display ON, Cursor OFF
    LCD_Cmd(0x06); // Auto-incremento
    LCD_Cmd(0x01); // Limpiar LCD
    __delay_ms(2);
}

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;    // reloj a 40 MIPS
    AD1PCFGL = 0xFFFF;   __delay_ms(100);  // Pines digitales

    I2C_Init(); LCD_Init();
    LCD_Cmd(0x80); // Fila 1, Columna 1
    LCD_String("DAR-CPU I2C");
    LCD_Cmd(0xC0); // Fila 2, Columna 1
    LCD_String("Pines liberados!");

    while(1) {
        // bucle libre
    }
    return 0;
}