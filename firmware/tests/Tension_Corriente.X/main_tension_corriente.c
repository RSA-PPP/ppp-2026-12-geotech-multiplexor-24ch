//Programa que permite hacer funcionar los motores para medir sus voltajes y corrrientes en reposos y en uso

#include <xc.h>

#pragma config FOSC = INTOSCIO  
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config MCLRE = OFF      
#pragma config BOREN = ON       
#pragma config LVP = OFF        
#pragma config CPD = OFF        
#pragma config CP = OFF         

#define _XTAL_FREQ 4000000      

// Definición exacta según etiquetas de red en el esquemático
#define DIR_M1      PORTBbits.RB3   // Pin 9  -> M1 (Entradas 1A y 3A del L293D)
#define DIR_M2      PORTAbits.RA1   // Pin 18 -> M2 (Entradas 2A y 4A del L293D)
#define ENABLE_M1   PORTBbits.RB5   // Pin 11 -> M1_EN (Pin 1 EN1,2 del L293D)
#define ENABLE_M2   PORTBbits.RB4   // Pin 10 -> M2_EN (Pin 9 EN3,4 del L293D)

void main(void) {
    CMCON = 0x07;       // Desactiva comparadores analógicos en PORTA
    
    // Configuración de direcciones de pines
    TRISBbits.TRISB3 = 0; // M1 salida
    TRISBbits.TRISB4 = 0; // M2_EN salida
    TRISBbits.TRISB5 = 0; // M1_EN salida
    TRISAbits.TRISA1 = 0; // M2 salida
    
    // Estado inicial: Todo apagado
    ENABLE_M1 = 0;
    ENABLE_M2 = 0;
    DIR_M1 = 0;
    DIR_M2 = 0;

    while(1) {
        // =====================================
        // 1. ESTADO DE REPOSO (10 segundos)
        // =====================================
        ENABLE_M1 = 0;
        ENABLE_M2 = 0;
        DIR_M1 = 0;
        DIR_M2 = 0;
        __delay_ms(10000);

        // ==========================================
        // 2. SOLO MOTOR 1 EN MOVIMIENTO (10 segundos)
        // ==========================================
        DIR_M1 = 1;         // 1A = 1
        DIR_M2 = 0;         // 2A = 0 (Giro en sentido directo)
        ENABLE_M1 = 1;      // Habilita M1
        ENABLE_M2 = 0;
        __delay_ms(10000);

        // Pausa breve
        ENABLE_M1 = 0;
        __delay_ms(1500);

        // ==========================================
        // 3. SOLO MOTOR 2 EN MOVIMIENTO (10 segundos)
        // ==========================================
        DIR_M1 = 1;         // 3A = 1
        DIR_M2 = 0;         // 4A = 0
        ENABLE_M1 = 0;
        ENABLE_M2 = 1;      // Habilita M2
        __delay_ms(10000);

        // Pausa breve
        ENABLE_M2 = 0;
        __delay_ms(1500);

        // ==========================================
        // 4. AMBOS MOTORES AL MISMO TIEMPO (10 segundos)
        // ==========================================
        DIR_M1 = 1;
        DIR_M2 = 0;
        ENABLE_M1 = 1;
        ENABLE_M2 = 1;
        __delay_ms(10000);
    }
}