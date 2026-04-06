#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <libpic30.h>

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH, GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

#define EEPROM_ADDR 0xA0

// --- DRIVERS I2C Y LCD ---
void I2C_Init(void) { I2C1BRG = 95; I2C1CONbits.I2CEN = 1; }
void I2C_Start(void) { I2C1CONbits.SEN = 1; while(I2C1CONbits.SEN); }
void I2C_Stop(void)  { I2C1CONbits.PEN = 1; while(I2C1CONbits.PEN); }
void I2C_Write(uint8_t data) { I2C1TRN = data; while(I2C1STATbits.TRSTAT); }

void LCD_Nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | (rs ? 1 : 0) | 0x08; 
    I2C_Start(); I2C_Write(0x4E); I2C_Write(data | 0x04); __delay_us(1); I2C_Write(data & ~0x04); I2C_Stop(); __delay_us(50);
}
void LCD_Cmd(uint8_t cmd) { LCD_Nibble(cmd >> 4, 0); LCD_Nibble(cmd & 0x0F, 0); __delay_ms(2); }
void LCD_Char(char c)     { LCD_Nibble(c >> 4, 1);   LCD_Nibble(c & 0x0F, 1);   __delay_us(50); }
void LCD_String(char *s)  { while(*s) LCD_Char(*s++); }
void LCD_Init(void) {
    __delay_ms(20); LCD_Nibble(0x03, 0); __delay_ms(5); LCD_Nibble(0x03, 0); __delay_us(150);
    LCD_Nibble(0x03, 0); __delay_ms(2); LCD_Nibble(0x02, 0); __delay_ms(2); 
    LCD_Cmd(0x28); LCD_Cmd(0x0C); LCD_Cmd(0x06); LCD_Cmd(0x01); 
}

// --- DRIVER UART (Alta Velocidad 115200) ---
void init_UART(void) {
    AD1PCFGL = 0xFFFF; // ¡LA CURA! Todo el puerto B es 100% digital
    
    __builtin_write_OSCCONL(OSCCON & ~(1<<6)); 
    RPINR18bits.U1RXR = 0;                     
    RPOR0bits.RP1R = 3;                        
    __builtin_write_OSCCONL(OSCCON | (1<<6));  
    
    U1MODEbits.BRGH = 1; // ALTA VELOCIDAD
    U1BRG = 86;          // 115200 Baudios a 40 MIPS
    
    U1MODEbits.UARTEN = 1; 
    U1STAbits.UTXEN = 1;   
}

void UART_Write(char c) { while(U1STAbits.UTXBF); U1TXREG = c; }
void UART_Write_String(char *texto) { while(*texto) UART_Write(*texto++); }

// --- RUTINA I2C DE ESCRITURA BLINDADA ---
char EEPROM_Write_Byte(uint16_t addr, uint8_t data) {
    I2C_Start(); I2C_Write(EEPROM_ADDR); I2C_Write(addr >> 8); I2C_Write(addr & 0xFF); I2C_Write(data); I2C_Stop();
    int timeout = 0;
    while(timeout < 2000) {
        I2C_Start(); I2C1TRN = EEPROM_ADDR; while(I2C1STATbits.TRSTAT);
        if(I2C1STATbits.ACKSTAT == 0) { I2C_Stop(); return 'K'; }
        I2C_Stop(); timeout++; __delay_us(5);
    }
    return 'E';
}

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;
    
    init_UART();
    I2C_Init();
    LCD_Init();
    
    TRISBbits.TRISB11 = 0; 
    LATBbits.LATB11 = 0;   
    
    __delay_ms(100); 
    
    // Barrer basura del USB
    while(U1STAbits.URXDA) { char basura = U1RXREG; }
    if (U1STAbits.OERR) U1STAbits.OERR = 0; 
    
    LCD_Cmd(0x80); LCD_String("  Prog. EEPROM  ");
    LCD_Cmd(0xC0); LCD_String("Esperando PC... ");

    // --- BUCLE DE SINCRONIZACIÓN AUTOMÁTICA ---
    while(1) {
        if (U1STAbits.OERR) U1STAbits.OERR = 0; 
        
        if (U1STAbits.URXDA) {
            if (U1RXREG == '?') {
                LCD_Cmd(0xC0); LCD_String("Grabando...     ");
                UART_Write_String("START");
                break; 
            }
        }
    }

    // --- BUCLE DE GRABACIÓN DE AUDIO ---
    uint32_t address = 0; 
    char lcd_buffer[16];

    while(1) {
        if (U1STAbits.OERR) U1STAbits.OERR = 0; 
        
        if (U1STAbits.URXDA) {
            uint8_t data_in = U1RXREG;
            
            LATBbits.LATB11 ^= 1; // LED Parpadeando a toda velocidad
            
            if (address < 65536) {
                char resultado = EEPROM_Write_Byte((uint16_t)address, data_in);
                UART_Write(resultado); // Responder 'K' a Python
                
                if(resultado == 'K') {
                    address++;
                    if (address % 1024 == 0) {
                        sprintf(lcd_buffer, "Grabado: %lu KB  ", (address / 1024));
                        LCD_Cmd(0xC0); LCD_String(lcd_buffer);
                    }
                    if (address == 65536) {
                        LCD_Cmd(0xC0); LCD_String("Grabado Exitoso!");
                        LATBbits.LATB11 = 0; 
                    }
                } else {
                    LCD_Cmd(0xC0); LCD_String("Error Hardware  ");
                }
            }
        }
    }
    return 0;
}