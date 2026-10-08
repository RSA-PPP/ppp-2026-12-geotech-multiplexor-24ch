/*
 * Walter Andrés Calderón Luna
 * Dispositivo: PIC16F628A
 * Entorno:     MPLAB X + XC8 + PICkit 4
 * Propósito:   CP 1.1 - Prueba de placa pura y oscilador.
 * Pin usado:   RB5 (Pin 11) mapeado a M1_EN
 */

// CONFIGURACIÓN DE FUSIBLES (VITAL PARA EL PICKIT 4)
#pragma config FOSC = INTOSCIO  // Oscilador interno 4MHz. Pines RA6 y RA7 son I/O.
#pragma config WDTE = OFF       // Watchdog Timer APAGADO.
#pragma config PWRTE = ON       // Power-up Timer ACTIVADO.
#pragma config MCLRE = OFF       // Pin RA5 (VPP) como Reset externo. CRÍTICO para programar.
#pragma config BOREN = ON       // Brown-out Reset ACTIVADO.
#pragma config LVP = OFF        // Programación en bajo voltaje APAGADA. CRÍTICO.
#pragma config CPD = OFF        // Protección de datos EEPROM APAGADA.
#pragma config CP = OFF         // Protección de código APAGADA.

#include <xc.h>

#define _XTAL_FREQ 4000000      // Frecuencia del oscilador interno (4 MHz)

void main(void) {
    // 1. Apagar comparadores analógicos
    CMCON = 0x07;               // Todos los pines del PORTA como E/S digitales
    TRISA = 0xFF;  // Todos los pines del PORTA como entradas para sensores
    // 3. Bucle infinito: Señal cuadrada de 1 Hz
    while(1) {
        PORTBbits.RB5 = 1;      // Pin 11 a 5V
        __delay_ms(1);        // Esperar 500 milisegundos
        
        PORTBbits.RB5 = 0;      // Pin 11 a 0V
        __delay_ms(1);        // Esperar 500 milisegundos
    }
}