/*
 * dsPIC33FJ32MC204
 * Cristal externo 8 MHz + PLL
 * FOSC = 80 MHz
 * FCY  = 40 MHz
 */

#include <xc.h>

#pragma config FNOSC = PRIPLL
#pragma config IESO = OFF
#pragma config POSCMD = XT
#pragma config OSCIOFNC = OFF
#pragma config FCKSM = CSDCMD
#pragma config IOL1WAY = OFF

#pragma config FWDTEN = OFF
#pragma config WINDIS = OFF
#pragma config WDTPRE = PR128
#pragma config WDTPOST = PS32768

#pragma config FPWRT = PWR128
#pragma config PWMPIN = ON
#pragma config HPOL = OFF
#pragma config LPOL = OFF
#pragma config ALTI2C = OFF

#pragma config JTAGEN = OFF
#pragma config ICS = PGD1
