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
#define F_CPU 16000000MH 
#define trigger PORTD6
#define led PORTD5
#define buzz PORTD4
#define sensor_d PORTC0
#define sensor_i PORTC1

//variables

int distancia=0;//mm
long int t_inicio_obj_on=0;
int pulso_max=25500;
int pulso_minimo=500;
unsigned int contador = 1;//
long int us=0;
long int t_eco;
long int t_obj_on=0;
long int t_inicio_alarma=0;
long int t_alarma_on=0;
char dist_max=20;
char dist_min=10;
char actuar=0;
char comando_recibido[20];
char medir_distancia=0;
char enviar_trigger=1;
char obj_presente=0;
char encendido=0;
char soltar_obj=0;

volatile unsigned char received_data;
volatile int flag = 0;
unsigned char str[20];
unsigned char i=0;

//variables maq efectores
char estadoActual_maq_str=0;
enum estados_pinza {ESPERA,OBJ_ON,OBJ_FAIL,OBJ_OFF,OBJ_MOV,OBJ_CATCH,OBJ_RELEASED,EFECT_ON,CAPTURA_OBJ,EFECT_RESTART,total_estados_pinza};
enum estados_comandos {ADQUIR_STR,CONF_AT,EV_COMANDO,COM_ON,COM_OBJ,COM_STATE,COM_FORCE,FORZ_ALARMA,total_estados_comandos};
	
//variables maq mensajes
enum msg{Msg_obj_on,Msg_obj_fail,Msg_obj_off,Msg_obj_mov,Msg_obj_catch,Msg_obj_released,Msg_efect_on,Msg_efect_restart,Msg_ok_on,Msg_ok_off,total_mensajes};
char* mensajes[total_mensajes];
char msg_obj_on[]="OBJ_ON";
char msg_obj_fail[]="OBJ_FAIL";
char msg_obj_off[]="OBJ_OFF";
char msg_obj_mov[]="OBJ_MOV";
char msg_obj_catch[]="OBJ_CATCH";
char msg_obj_released[]="OBJ_RELEASED";
char msg_efect_on[]="EFECT_ON";
char msg_efect_restart[]="EFECT_RESTART";
char msg_ok_on[]="Ok_ON";
char msg_ok_off[]="OK_OFF";

char estado_actual_pinza=ESPERA;
char estado_actual_comandos=ADQUIR_STR;
void sensor_distancia();
int detectar_objeto();
void actualizar_t_obj();

//estados_pinza
void Espera();
void Obj_on();
void Obj_fail();
void Obj_off();
void Obj_mov();
void Obj_cath();
void Obj_released();
void Efect_on();
void Captura_obj();
void Efect_restart();

//estados_comandos
void adquirir_str();
void conf_at();
void ev_comando();
void com_on();
void com_obj();
void com_state();
void com_force();
void forz_alarma();
//funciones 
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char data);
void enviar_msg_p_serie(int mensaje);
void enviar_texto(char text[]);
void cerrar();
void abrir();

int main(void)
{
	mensajes[Msg_obj_on]=msg_obj_on;
	mensajes[Msg_obj_off]=msg_obj_off;
	mensajes[Msg_obj_catch]=msg_obj_catch;
	mensajes[Msg_obj_mov]=msg_obj_mov;
	mensajes[Msg_obj_fail]=msg_obj_fail;
	mensajes[Msg_obj_released]=msg_obj_released;
	mensajes[Msg_efect_on]=msg_efect_on;
	mensajes[Msg_efect_restart]=msg_efect_restart;
	mensajes[Msg_ok_on]=msg_ok_on;
	mensajes[Msg_ok_off]=msg_ok_off;
	void (*vector_estados_comandos[total_estados_comandos])();
	vector_estados_comandos[ADQUIR_STR]=adquirir_str;
	vector_estados_comandos[CONF_AT]=conf_at;
	vector_estados_comandos[EV_COMANDO]=ev_comando;
	vector_estados_comandos[FORZ_ALARMA]=forz_alarma;

	void (*vector_estados_pinza[total_estados_pinza])();
	vector_estados_pinza[ESPERA]=Espera;
	vector_estados_pinza[OBJ_ON]=Obj_on;
	vector_estados_pinza[OBJ_FAIL]=Obj_fail;
	vector_estados_pinza[OBJ_OFF]=Obj_off;
	vector_estados_pinza[OBJ_MOV]=Obj_mov;
	vector_estados_pinza[OBJ_CATCH]=Obj_cath;
	vector_estados_pinza[OBJ_RELEASED]=Obj_released;
	vector_estados_pinza[EFECT_ON]=Efect_on;
	vector_estados_pinza[EFECT_RESTART]=Efect_restart;
	vector_estados_pinza[CAPTURA_OBJ]=Captura_obj;
	
	configuracion_inicial();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		vector_estados_pinza[estado_actual_pinza]();
		vector_estados_comandos[estado_actual_comandos]();	
    }
}

//Estados

void Espera(){
	if ((dist_min<distancia) & (distancia<dist_max) & (!obj_presente))
	{
		estado_actual_pinza=OBJ_ON;
		t_inicio_obj_on=us;
		obj_presente=1;
		enviar_msg_p_serie(Msg_obj_on);
	}
}
void Obj_on(){
	if (distancia>dist_max)
	{
		obj_presente=0;
		estado_actual_pinza=ESPERA;//o fail no sé
		enviar_msg_p_serie(Msg_efect_restart);
	}else
	{
		actualizar_t_obj();
	}
	if (obj_presente & (4001000>t_obj_on) & (t_obj_on>4000000))
	{
		PORTD=PORTD & (1<<buzz);
	}
	if (obj_presente & (5001000>t_obj_on) & (t_obj_on>5000000))
	{
		PORTD=PORTD & ~ (1<<buzz);
	}
	if (obj_presente & (t_obj_on>9000000) & encendido)
	{
		estado_actual_pinza=CAPTURA_OBJ;
	}
};
void Obj_fail(){
	enviar_msg_p_serie(Msg_obj_fail);
	estado_actual_pinza=ESPERA;
	};
	
void Obj_off(){
	estado_actual_pinza=OBJ_OFF;
	abrir();
	if (OCR1A==pulso_minimo)
	{
		enviar_msg_p_serie(Msg_obj_released);
		estado_actual_pinza=OBJ_RELEASED;
	}
	};
void Obj_mov(){
	if (soltar_obj)
	{
		estado_actual_pinza=OBJ_OFF;
	}
	};
void Obj_cath(){
	estado_actual_pinza=OBJ_MOV;
	enviar_msg_p_serie(Msg_obj_mov);
	};
void Captura_obj(){
	if (distancia>dist_max)
	{
		obj_presente=0;
		estado_actual_pinza=OBJ_FAIL;
	}else if (detectar_objeto())
	{
		enviar_msg_p_serie(Msg_obj_catch);
		estado_actual_pinza=OBJ_CATCH;
	}else{
		if (actuar>11)
		{
			estado_actual_pinza=OBJ_FAIL;
		}else{
			cerrar();
		}
	}
	};
void Obj_released(){
	if (distancia>dist_max)
	{
		enviar_msg_p_serie(Msg_obj_off);
		t_obj_on=us;
		estado_actual_pinza=EFECT_RESTART;
	}
	};
void Efect_on(){
	
	};
void Efect_restart(){
	actualizar_t_obj();
		if (t_obj_on>3000000)
		{
			estado_actual_pinza=ESPERA;
		}
	};

//estados maquina mensajes
void adquirir_str(){
	if(flag){
		//quizas sería bueno desactivar las interrupciones cuadno se ejecute esta parte de la funcion
		int n=0;
		while( str[n]!='\0'){
			comando_recibido[n]=str[n];
			n++;
		}
		comando_recibido[n]='\0';
		flag=0;
		estadoActual_maq_str=CONF_AT;
	}
}

void conf_at(){
	if (comparar_str("AT",comando_recibido,0)){
		estado_actual_comandos=EV_COMANDO;
		}else{
		enviar_msg_p_serie("El comando no es valido");
		estado_actual_comandos=ADQUIR_STR;
	}
}
void ev_comando(){
	if (comparar_str("$ON=",comando_recibido,2)){
		estado_actual_comandos=COM_ON;
	}else if (comparar_str("$OBJ=?",comando_recibido,2))
	{
		estado_actual_comandos=COM_OBJ;
	}else if (comparar_str("$STATE=?",comando_recibido,2))
	{
		estado_actual_comandos=COM_STATE;
	}else if (comparar_str("$FORCE=",comando_recibido,2))
	{
		estado_actual_comandos=COM_FORCE;
	}else{
		enviar_msg_p_serie("El comando no es valido");
		estado_actual_comandos=ADQUIR_STR;
	}
}
void com_on(){
	if (comparar_str("1",comando_recibido,6)){
		encendido=1;
	}else if (comparar_str("0",comando_recibido,6))
	{
		encendido=0;
	}else{
	enviar_msg_p_serie("El comando no es valido");
	}
	estado_actual_comandos=ADQUIR_STR;
}
void com_obj(){
	if (obj_presente)
	{
		enviar_msg_p_serie("1");
	}else{
		enviar_msg_p_serie("0");
	}
	estado_actual_comandos=ADQUIR_STR;
}
void com_state(){
	if (obj_presente)
	{
		if (estado_actual_pinza==EFECT_RESTART)
		{
			enviar_msg_p_serie("3");
		}else{
			if (t_obj_on>4000000)
			{
				enviar_msg_p_serie("2");
			}else{
				enviar_msg_p_serie("1");
			}
		}
	}else{
		enviar_msg_p_serie("0");
	}
}
void com_force(){
	if (comparar_str("1",comando_recibido,9))
	{
		if (!detectar_objeto() | actuar >11)
		{
			cerrar();
		}else{
			estado_actual_comandos=ADQUIR_STR;
		}
	}else if (comparar_str("2",comando_recibido,9))
	{
		soltar_obj=1;
		estado_actual_comandos=ADQUIR_STR;
	}else if (comparar_str("3",comando_recibido,9))
	{
		estado_actual_comandos=FORZ_ALARMA;
		t_inicio_alarma=us;
	}
}
void forz_alarma(){
	actualizar_t_alarma_on();
	if(t_alarma_on<2000000){
		PORTD=PORTD & (1<<buzz);
	}else{
		PORTD=PORTD & ~ (1<<buzz);
		estado_actual_comandos=ADQUIR_STR;
	}	
}

//configuracion
void configuracion_inicial(){
	//fast pwm
	DDRB |= ( 1<< PORTB1 );  // Configuramos el PB1 como salida.
	TCNT1 = 0; // Reiniciamos el contador inicial
	ICR1 = 42999; // Configuramos el periodo de la señal TOP

	TCCR1A =  (1 << COM1A1) | (0 << COM1A0) ; // Ponemos a 'bajo' el OCR1A cuando coincida el Compare Match
	TCCR1A |=  (1 << WGM11) | (0 << WGM10) ; // Fast PWM: TOP: ICR1
	TCCR1B = (1 << WGM13) | (1 << WGM12); // // Fast PWM: TOP: ICR1
	TCCR1B |= (0 << CS12) | (1 << CS11) | ( 0 << CS10 ); // Preesc = 8

	OCR1A = 500; // ancho del pulso
	
	USART_Init(MYUBRR);
	//configuracion sensores presión-entradas e interrupcion en pd2
	DDRC = DDRC &~ (1<<sensor_d) &~ (1<<sensor_i);
	DDRD = DDRD &~ (1<<PORTD2);
	//configuracion  trigger, led y buzzer
	PORTD = PORTD | (1<<led);
	PORTD = PORTD | (1<<buzz);
	DDRD = DDRD | (1<<trigger);
	
	//configuracion timer 0
	TCCR0A = 0;
	TCCR0B = 0b00000010;
	//TCNT0=0b00111010; //100us
	TCNT0=237;//10us
	TIMSK0= 0b00000001;
	EICRA=0b00000010;//interrupcion 0 en falling
	EIMSK=0b00000001;//habilita interrupcion 0
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

//Funciones
void abrir(){
	if (OCR1A>pulso_minimo & !(actuar%10)){
		OCR1A--;
		actuar=0;
	}
	actuar++;
}
void cerrar(){
	if(OCR1A<pulso_max & !(actuar%10)){
		OCR1A++;
		actuar=0;
	}
	actuar++;
}
void sensor_distancia(){
	if (medir_distancia)
	{
		t_eco=us-t_eco;
		if (t_eco<0)
		{t_eco=+20000000000;
		}
		distancia=0.1715*t_eco;
		medir_distancia=0;
	}
	if (enviar_trigger)
	{
		PORTD=PORTD & (1<<trigger);
		t_eco=us;
		enviar_trigger=0;
		}else{
		PORTD=PORTD & ~(1<<trigger);
	}
}
int detectar_objeto(){
	return (PINC & ((1<<sensor_d) | (1<<sensor_i)));
}
void actualizar_t_obj(){
	t_obj_on=us-t_inicio_obj_on;
	if (t_obj_on<0)
	{
		t_obj_on=+20000000000;
	}
}
void actualizar_t_alarma_on(){
	t_alarma_on=us-t_inicio_alarma;
	if (t_alarma_on<0)
	{
		t_alarma_on=+20000000000;
	}
}
unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
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
	cli();
	int i=0;
	while(mensajes[mensaje][i]!='\0'){
		USART_Transmit(mensajes[mensaje][i]);
		i++;
	}
	USART_Transmit('\n');
	sei();
};
void enviar_texto(char text[]){
	cli();
	int n=0;
	while(text[n]!='\0'){
		USART_Transmit(text[n]);
		n++;
	};
	USART_Transmit('\n');
	sei();
};

//interrupciones
ISR (USART_RX_vect)
{
	received_data = USART_Receive();
	if (((received_data<='z') && (received_data>='$')) || (received_data=='\n'))
	{
		if(received_data!='\n'){// && received_data!='\r'
			str[i]=received_data;
			i++;
			}else{
			str[i]='\0';
			i=0;
			flag = 1;
		}
	}
}
ISR (TIMER0_OVF_vect)//10us
{
	TCNT0=239;
	us+=10;
	if (us>20000000000){
		us=0;
	}
};
ISR (INT0_vect){
	medir_distancia=1;
	enviar_trigger=1;
	sensor_distancia();
}