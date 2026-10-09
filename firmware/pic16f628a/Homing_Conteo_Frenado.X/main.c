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

// Pines L293D
#define DIR_A       PORTBbits.RB3   
#define DIR_B       PORTAbits.RA1   
#define ENABLE_M1   PORTBbits.RB5   

// Sensores
#define SENS1_M1    PORTAbits.RA2   // Encoder de Paso (Ranuras continuas)
#define SENS2_M1    PORTAbits.RA3   // Encoder Home (Muesca única)

int posicion_actual_m1 = 0;

void inicializar() {
    CMCON = 0x07;       
    TRISBbits.TRISB3 = 0; 
    TRISAbits.TRISA1 = 0; 
    TRISBbits.TRISB5 = 0; 
    TRISAbits.TRISA2 = 1; 
    TRISAbits.TRISA3 = 1; 
    ENABLE_M1 = 0; DIR_A = 0; DIR_B = 0;
}

void frenado_dinamico_m1() {
    DIR_A = 1;
    DIR_B = 1;      
    ENABLE_M1 = 1;  
    __delay_ms(50); // Frenado exacto
    
    ENABLE_M1 = 0;  
    DIR_A = 0;
    DIR_B = 0;
}

// ========================================================
// 1. HOMING 
// ========================================================
void probar_homing_lento_m1() {
    DIR_A = 1;
    DIR_B = 0;
    
    int pulsos_on = 10;   
    int pulsos_off = 30; 

    while(1) {
        ENABLE_M1 = 1;
        for(int i = 0; i < pulsos_on; i++) {
            if(SENS2_M1 == 0) { // Busca el 0V
                frenado_dinamico_m1();
                posicion_actual_m1 = 1;
                return;
            }
            __delay_us(100);
        }
        
        ENABLE_M1 = 0;
        for(int i = 0; i < pulsos_off; i++) {
            if(SENS2_M1 == 0) { 
                frenado_dinamico_m1();
                posicion_actual_m1 = 1;
                return;
            }
            __delay_us(100);
        }
    }
}

// ========================================================
// 2. CONTEO (Nuevo, alta velocidad buscando flanco de subida)
// ========================================================
// Actividad 2: Conteo por lógica pura de flancos (Sin retardos de tiempo)
void ir_a_posicion_m1(int posicion_destino) {
    int pasos_a_dar = posicion_destino - posicion_actual_m1;
    if(pasos_a_dar < 0) pasos_a_dar += 12; 
    if(pasos_a_dar == 0) return; 
    
    // 60 muescas / 12 posiciones = 5 muescas físicas por paso
    int ranuras_objetivo = pasos_a_dar * 5; 
    int ranuras_contadas = 0;
    
    int estado_anterior = SENS1_M1; 
    
    DIR_A = 0;
    DIR_B = 1;
    int pulsos_on = 10;   
    int pulsos_off = 31; 
    int ciclo_pwm = 0;

    while(1) { 
        // 1. GENERADOR DE VELOCIDAD LENTA (PWM)
        ciclo_pwm++;
        if (ciclo_pwm <= pulsos_on) ENABLE_M1 = 1;
        else if (ciclo_pwm <= (pulsos_on + pulsos_off)) ENABLE_M1 = 0;
        else ciclo_pwm = 0;

        // 2. LÓGICA DE FLANCOS PURA
        int lectura_actual = SENS1_M1;

        if (lectura_actual != estado_anterior) {
            
            if (lectura_actual == 1) {
                // FLANCO DE SUBIDA: El sensor chocó con el plástico.
                // Registramos que acabamos de entrar a una nueva muesca.
                ranuras_contadas++;
            } 
            else if (lectura_actual == 0) {
                // FLANCO DE BAJADA: El sensor acaba de caer al hueco.
                if (ranuras_contadas >= ranuras_objetivo) {
                    frenado_dinamico_m1(); // ¡Freno instantáneo en el hueco!
                    posicion_actual_m1 = posicion_destino;
                    return; 
                }
            }
            estado_anterior = lectura_actual; // Actualizamos la "foto" del sensor
        }

        // 3. SEGURO DE VIDA PARA EL HOME
        // Respetamos la muesca única: Si vamos a la posición 1 y el sensor 2 cae al hueco, frena.
        if (posicion_destino == 1 && SENS2_M1 == 0) {
            frenado_dinamico_m1();
            posicion_actual_m1 = 1;
            return;
        }

        // Pausa minúscula solo para que el PWM funcione y el PIC no se sature
        __delay_us(100);
    }
}

void main(void) {
    inicializar();
    __delay_ms(2000); 
    
    probar_homing_lento_m1(); 
    __delay_ms(2000);
    
    while(1) { 
        ir_a_posicion_m1(4);
        __delay_ms(3000); 
        
        ir_a_posicion_m1(8);
        __delay_ms(3000);
        
        ir_a_posicion_m1(1);
        __delay_ms(3000);
    }
}