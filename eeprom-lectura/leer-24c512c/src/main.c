/* * Proyecto: DAR-CPU - Reproductor de Audio (Leer EEPROM)
 * Descripción: Lee audio PCM (16kHz, 8-bit) de la EEPROM y lo reproduce por PWM en RB14.
 */

#define FOSC  80000000UL
#define FCY   40000000UL

#include <xc.h>
#include <stdint.h>
#include <libpic30.h>

#pragma config BWRP = WRPROTECT_OFF, BSS = NO_FLASH, GWRP = OFF, GSS = OFF
#pragma config FNOSC = PRIPLL, IESO = ON, POSCMD = XT
#pragma config OSCIOFNC = OFF, IOL1WAY = ON, FCKSM = CSDCMD 
#pragma config WDTPOST = PS32768, WDTPRE = PR128, WINDIS = OFF, FWDTEN = OFF
#pragma config FPWRT = PWR128, ALTI2C = OFF, LPOL = ON, HPOL = ON, PWMPIN = ON
#pragma config ICS = PGD1, JTAGEN = OFF

#define EEPROM_ADDR_WRITE 0xA0
#define EEPROM_ADDR_READ  0xA1 

// RING BUFFER
#define BUFFER_SIZE 256
volatile uint8_t audio_buffer[BUFFER_SIZE];
volatile uint8_t head = 0; 
volatile uint8_t tail = 0; 

// DRIVERS I2C Y LCD
void I2C_Init(void) { I2C1BRG = 95; I2C1CONbits.I2CEN = 1; }
void I2C_Start(void) { I2C1CONbits.SEN = 1; while(I2C1CONbits.SEN); }
void I2C_Restart(void) { I2C1CONbits.RSEN = 1; while(I2C1CONbits.RSEN); }
void I2C_Stop(void)  { I2C1CONbits.PEN = 1; while(I2C1CONbits.PEN); }
void I2C_Write(uint8_t data) { I2C1TRN = data; while(I2C1STATbits.TRSTAT); }
uint8_t I2C_Read(uint8_t send_ack) {
    I2C1CONbits.RCEN = 1; while(I2C1CONbits.RCEN); 
    uint8_t data = I2C1RCV; 
    I2C1CONbits.ACKDT = (send_ack) ? 0 : 1; 
    I2C1CONbits.ACKEN = 1; while(I2C1CONbits.ACKEN);
    return data;
}

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

// CONFIGURACIÓN DE AUDIO
void init_Audio_PWM(void) {
    PTCONbits.PTEN = 0;      
    PTPER = 639;             // 62.5 kHz de portadora PWM
    PWMCON1bits.PMOD1 = 1;   // PWM Independiente
    PWMCON1bits.PEN1H = 1;   // Salida de audio en RB14
    PWMCON1bits.PEN1L = 0;   
    PDC1 = 0;                
    PTCONbits.PTEN = 1;      
}

void init_Audio_Timer(void) {
    T1CONbits.TON = 0;      
    T1CONbits.TCS = 0;      
    T1CONbits.TCKPS = 0;   
    // FCY = 40M, 40M/16000 = 2500, PR1 = (-1) = 2499
    PR1 = 2499;              // Interrupción exacta a 16,000 Hz 
    _T1IP = 6;               // Alta prioridad para evitar chasquidos
    _T1IF = 0;              
    _T1IE = 1;              
    T1CONbits.TON = 1;      
}

// AUDIO 
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    if(head != tail) {
        uint8_t sample = audio_buffer[tail++]; // Tomar una muestra del buffer
        PDC1 = (unsigned int)sample * 5; // PWM va de 0 a 1278. El audio va de 0 a 255, (255 * 5 = 1275)
    } 
    _T1IF = 0;
}

// INICIO DE LA MEMORIA
void EEPROM_Set_Read_Address(uint16_t addr) {
    I2C_Start();
    I2C_Write(EEPROM_ADDR_WRITE);
    I2C_Write(addr >> 8);         
    I2C_Write(addr & 0xFF);       
    I2C_Restart();
    I2C_Write(EEPROM_ADDR_READ);  // Lectura Infinita
}

int main(void) {
    PLLFBD = 38; CLKDIVbits.PLLPOST = 0; CLKDIVbits.PLLPRE = 0;
    AD1PCFGL = 0xFFFF; // Pines digitales
    __delay_ms(100);
    
    I2C_Init();
    LCD_Init();
    init_Audio_PWM();
    
    LCD_Cmd(0x80); LCD_String(" Reproduciendo! ");
    LCD_Cmd(0xC0); LCD_String("  DAR-CPU Audio ");
    
    uint32_t memory_address = 0;
    EEPROM_Set_Read_Address(0);

    init_Audio_Timer();    // Timer1 consume el buffer

    while(1) {         // Llenar buffer mientras el Timer lo consume
        uint8_t next_head = (uint8_t)(head + 1);

        if(next_head != tail) {
            memory_address++;
            
            if(memory_address >= 64000) {                
                audio_buffer[head] = I2C_Read(0);  // Leer último byte con NACK
                I2C_Stop();
                head = next_head;                
                memory_address = 0; // Bucle Infinito
                EEPROM_Set_Read_Address(0);
            } else {
                audio_buffer[head] = I2C_Read(1);  // Leer byte y pedir el siguiente automáticamente con ACK
                head = next_head;
            }
        }
    }
    return 0;
}