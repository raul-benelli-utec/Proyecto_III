/*
 * pryecto_III_v1.c
 *
 * Created: 21/10/2022 16:28:30
 * Author : Raul
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "comunicacion_serial.h"
#include "funciones_str.h"

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

//configuration
void USART_Init(unsigned int ubrr);
void configuracion_inicial();
void configuracion_msg();
#define F_CPU 16000000MH 
#define s_pres_der PORTB4 
#define s_pres_izq PORTB5
#define pwm_servo PORTD6

enum mensajes{Msg_at_ok,Msg_at_n,Msg_at_y,Msg_ok_clear,Msg_error,Msg_vacio,total_mensajes};
char cadena[20];

int numero;
char cadena_descartable[20];
char comando_recibido[20];

char msg_at_ok[]="AT+OK";
char msg_at_n[]="AT+N";
char msg_at_y[]="AT+Y";
char msg_ok_clear[]="OK_CLEAR";
char msg_abb_vacio[]="EMPTY_ABB";
char msg_error[]="ERROR";
char (*mensajes[total_mensajes]);

//variables 
char s1=0;
int estado_actual=0;
char contador = 1;
char posicion=0;
int posicion_minima=5;
int rango_trabajo=20;
volatile unsigned char received_data;
volatile int flag = 0;
unsigned char str[20];
unsigned char i=0;

enum estados {OBJ_ON,OBJ_FAIL,OBJ_OFF,OBJ_MOV,OBJ_CATCH,OBJ_RELEASED,EFECT_ON,CAPTURA_OBJ,EFECT_RESTART,BRAZO_MOV,total_estados};
	
//estados

void Obj_on();
void Obj_fail();
void Obj_off();
void Obj_mov();
void Obj_cath();
void Obj_released();
void Efect_on();
void Captura_obj();
void Efect_restart();
void Brazo_mov();

//funciones 
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char data);
void enviar_msg_p_serie(int mensaje);
void enviar_texto(char text[]);
void cerrar();
void abrir();



int main(void)
{
	void (*vector_estados[total_estados])();
	vector_estados[OBJ_ON]=Obj_on;
	vector_estados[OBJ_FAIL]=Obj_fail;
	vector_estados[OBJ_OFF]=Obj_off;
	vector_estados[OBJ_MOV]=Obj_mov;
	vector_estados[OBJ_CATCH]=Obj_cath;
	vector_estados[OBJ_RELEASED]=Obj_released;
	vector_estados[EFECT_ON]=Efect_on;
	vector_estados[EFECT_RESTART]=Efect_restart;
	vector_estados[BRAZO_MOV]=Brazo_mov;
	configuracion_inicial();
	configuracion_msg();
	sei();
    /* Replace with your application code */
    while (1) 
    {
		vector_estados[estado_actual]();
    }
}

//configuracion
void configuracion_inicial(){
	USART_Init(MYUBRR);
	//configuracion sensores presión-entradas
	DDRB = DDRB | (1<<s_pres_der) | (1<<s_pres_izq);
	//configuracion pwm servo- 
	posicion=posicion_minima;
	DDRD = DDRD & ~(1<<pwm_servo);
	//configuracion timer 0
	TCCR0A = 0;
	TCCR0B = 0b00000010;
	TCNT0=0b00111010;
	TIMSK0= 0b00000001;
	
	};
void USART_Init(unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B =(1<< RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C |= (3<<UCSZ00);
}
void configuracion_msg(){
	mensajes[Msg_at_ok]=msg_at_ok;
	mensajes[Msg_at_n]=msg_at_n;
	mensajes[Msg_at_y]=msg_at_y;
	mensajes[Msg_ok_clear]=msg_ok_clear;
	mensajes[Msg_error]=msg_error;
	mensajes[Msg_vacio]=msg_abb_vacio;
};
//Funciones
void cerrar(){
	if (posicion>posicion_minima){
		posicion--;
	}
}
void abrir(){
	if(posicion<posicion_minima+rango_trabajo){
		posicion++;
	}
}
unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)))
	;
	/* Get and return received data from buffer */
	return UDR0;
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)))
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void enviar_msg_p_serie(int mensaje){
	int i=0;
	while(mensajes[mensaje][i]!='\0'){
		USART_Transmit(mensajes[mensaje][i]);
		i++;
	}
	USART_Transmit('\n');
	};
void enviar_texto(char text[]){
	int n=0;
	while(text[n]!='\0'){
		USART_Transmit(text[n]);
		n++;
	};
	USART_Transmit('\n');
};

//Estados
void Obj_on(){
	
	};
void Obj_fail(){
	
	};
void Obj_off(){
	
	};
void Obj_mov(){
	
	};
void Obj_cath(){
	
	};
void Captura_obj(){
	
	};
void Obj_released(){
	
	};
void Efect_on(){
	
	};
void Efect_restart(){
	
	};
void Brazo_mov(){
	
	};

//interrupciones
ISR (USART_RX_vect)
{
	received_data = USART_Receive();
	if(received_data!='\n' && received_data!='\r'){
		str[i]=received_data;
		i++;
		}else{
		str[i]='\0';
		i=0;
		flag = 1;
	}
}
ISR (TIMER0_OVF_vect)
{
	TCNT0=0b00111010;//100 us
	
	if(contador>posicion){
		PORTB=PORTB &~(1<<pwm_servo);
		}else{
		PORTB=PORTB |(1<<pwm_servo);
	}
	contador++;
	if(contador>=200){
		contador=1;
	};

}

/*
char ciclo_trabajo = 30;
char estado_actual_pwm=0;
void configurar_pwm();
void ciclo_alto();
void ciclo_bajo();
int contador=0;
enum estados_pwm {alto,bajo};

int main(void)

{

	configurar_pwm();
	void (*vector_estados_pwm[2])();
	vector_estados_pwm[0]=ciclo_alto;
	vector_estados_pwm[0]=ciclo_bajo;
	while (1)
	{
		vector_estados_pwm[estado_actual_pwm]();

	}
}

void configurar_pwm(){
	DDRB = DDRB | (1<<salida_pwm);
};
void ciclo_alto(){
	PORTB = PORTB | (1<<salida_pwm);
	_delay_ms(1);
	contador++;
	if(contador>ciclo_trabajo){
		estado_actual_pwm=bajo;
		contador=0;
	}
};
void ciclo_bajo(){
	PORTB &= ~(1<<salida_pwm);
	_delay_ms(1);
	contador++;
	if(contador>(100-ciclo_trabajo)){
		estado_actual_pwm=alto;
		contador=0;
	}
};
*/