/*
 * Laboratorio_2_parte_1.c
 *
 * Created: 25/10/2022 17:58:02
 * Author : Raul 
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

// para usar delays definir frecuancia de trabajo ej #define F_CPU 16000000UL

#define pin_pwm PORTB4
char contador = 1;
char posicion=7;
int posicion_minima=7;
char sentido=1;
int contador_pos=0;
int rango_trabajo=20;
int pasos=10;
int factor_mult=0;
unsigned char lectura=0;
void configuracion();



int main(void)
{
	configuracion();
    /* Replace with your application code */
    while (1) 
    {
		if(contador_pos>=100){
			if(sentido){
				if(posicion<=25){
					posicion++;
				}else{sentido=0;};
			}else{
				if(posicion>=7){
					posicion--;
				}else{sentido=1;}
				}
				
			contador_pos=0;
		}
		
    }
}

void configuracion(){
	//configuracion pwm

	cli();
	//portb5 como salida 
	DDRB = DDRB |( 1<<pin_pwm);
	DDRC = DDRC & ~(1<<PORTC0);
	
	//configuracion timer 0
	
	//para TCCR0A 
	// COM0A1 en 0 y COM0A0 en 0 
	TCCR0A = 0;
	TCCR0B = 0b00000010;
	TCNT0=0b00111010;
	TIMSK0= 0b00000001;
	
	sei();
	};

ISR (TIMER0_OVF_vect)
{
	TCNT0=0b00111010;
	if(contador>posicion){
		PORTB=PORTB &~(1<<pin_pwm);
	}else{
		PORTB=PORTB |(1<<pin_pwm);
	}	
	contador++;
	if(contador>=200){
		contador=1;
		contador_pos++;
	};

}


