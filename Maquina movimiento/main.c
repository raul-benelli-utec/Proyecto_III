/*
 * pryecto_III_v1.c
 *
 * Created: 21/10/2022 16:28:30
 * Author : Raul
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "comunicacion_serial.h"


//configuration
void configuracion_inicial();
#define F_CPU 16000000MH 
#define s_pres_der PORTB4 
#define s_pres_izq PORTB5
#define pwm_servo PORTD6


//variables 
char s1=0;
int estado_actual=0;

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

void enviar_msg_p_serie(char mensaje);



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
	
    /* Replace with your application code */
    while (1) 
    {
		vector_estados[estado_actual]();
    }
}

//configuracion
void configuracion_inicial(){
	//configuracion sensores presión-entradas
	DDRB = DDRB | (1<<s_pres_der) | (1<<s_pres_izq);
	//configuracion pwm servo- salida
	DDRD = DDRD & ~(1<<pwm_servo);
	
	};
	
//Funciones
void enviar_msg_p_serie(char mensaje){
	
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