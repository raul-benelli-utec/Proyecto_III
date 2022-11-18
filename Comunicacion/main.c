/*
 * comunicacion_usart.c
 *
 * Created: 1/11/2022 18:55:32
 * Author : Raul
 */ 


#if __GNUC__
#include "avr/io.h"
#include "avr/interrupt.h"
#else
#include "ioavr.h"
#endif

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

volatile unsigned char received_data;
volatile int flag = 0;
unsigned char str[20];

unsigned char USART_Receive(void);
void USART_Transmit(unsigned char data);
void USART_Init(unsigned int ubrr);
enum mensajes{Msg_on,Msg_off,Msg_si_obj,Msg_no_obj,Msg_state_0,Msg_state_1,Msg_state_2,Msg_state_3,Msg_obj_on,Msg_obj_fail,Msg_obj_off,Msg_obj_mov,Msg_obj_cath,Msg_obj_obj_released,Msg_efect_on,Msg_efect_restart,total_mensajes};
char cadena[20];
char msg_on[]="OK_ON";
char msg_off[]="OK_OFF";
char msg_si_obj[]="1";
char msg_no_obj[]="0";
char msg_state_0[]="0";
char msg_state_1[]="1";
char msg_state_2[]="2";
char msg_state_3[]="3";
char msg_obj_on[]="obj_on";
char msg_obj_fail[]="obj_fail";
char msg_obj_off[]="obj_off";
char msg_obj_mov[]="obj_mov";
char msg_obj_cath[]="obj_cath";
char msg_obj_obj_released[]="OBJ_RELEASED";
char msg_efect_on[]="EFEC_ON";
char msg_efect_restart[]="EFECT_RESTART";
char i=0;

char (*mensajes[total_mensajes]);



void configuracion_msg();
void enviar_msg();
int main(void)
{
	configuracion_msg();
   DDRD |=(1<<PORTD7);
   USART_Init(MYUBRR);
   /* Enable interrupts => enable UART interrupts */
   sei();
   USART_Transmit('a');
    while (1) 
    {
    }
   
}
void configuracion_msg(){
	mensajes[Msg_on]=msg_on;
	mensajes[Msg_off]=msg_off;
	mensajes[Msg_no_obj]=msg_no_obj;
	mensajes[Msg_si_obj]=msg_si_obj;
	mensajes[Msg_state_0]=msg_state_0;
	mensajes[Msg_state_1]=msg_state_1;
	mensajes[Msg_state_2]=msg_state_2;
	mensajes[Msg_state_3]=msg_state_3;
	mensajes[Msg_obj_cath]=msg_obj_cath;
	mensajes[Msg_obj_fail]=msg_obj_fail;
	mensajes[Msg_efect_on]=msg_efect_on;
	mensajes[Msg_obj_on]=msg_obj_on;
	mensajes[Msg_obj_mov]=msg_obj_mov;
	mensajes[Msg_obj_off]=msg_obj_off;
	mensajes[Msg_efect_restart]=msg_efect_restart;
	mensajes[Msg_obj_obj_released]=msg_obj_obj_released;
		
	};

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

ISR (USART_RX_vect)
{
    received_data = UDR0;
    
	if( received_data!='\n'){
		cadena[i]=received_data;
		i++;
	}else{
		cadena[i]='\0';
		enviar_msg(Msg_obj_cath);
	}
	
    flag = 1;
}
void enviar_msg(int mensaje){
	int i=0;
	while(mensajes[mensaje][i]!='\0'){
		USART_Transmit(mensajes[mensaje][i]);
		i++;
	}
	USART_Transmit('\n');
	};

