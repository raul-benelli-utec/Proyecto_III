#ifndef funciones_str
#define funciones_str
#include <stdio.h>

int longitud_str(char arreglo[]) {
	int contador = 0;
	while (arreglo[contador] != '\0') {
		contador++;
	}
	return contador;
}
int string_a_num(char cadena[]){
	int num;
	num=(cadena[0]-48)*1000;
	num=num+(cadena[1]-48)*100;
	num=num+(cadena[2]-48)*10;
	num=num+(cadena[3]-48);
	
	return num;
};
char num_a_string(int numero, char cadena_descartable[]){
	int aux;
	aux=(numero/1000);
	cadena_descartable[0]=aux+48;
	numero=numero-(aux*1000);
	aux=(numero/100);
	cadena_descartable[1]=aux+48;
	numero=numero-(aux*100);
	aux=(numero/10);
	cadena_descartable[2]=aux+48;
	numero=numero-(aux*10);
	cadena_descartable[3]=numero+48;
	cadena_descartable[4]='\0';
};

char validar_digito(char numero_str){
	char valido=1;
	if(numero_str<'0' | numero_str>'9'){
			valido=0;
	}
	return valido;
};
char validar_numero(char numero_str[]) {
	if(validar_digito(numero_str[0])&&validar_digito(numero_str[1])&&validar_digito(numero_str[2])&&validar_digito(numero_str[3])){
		return 1;
	}else{
		return 0;
	}
}

void sub_string(char inicio, char fin, char original[],
char cadena_descartable[]) {
	char pos = 0;
	char max=longitud_str(original);
	if ((fin-inicio)>=max)
	{
		fin=(inicio+max);
	}
	for (int i = inicio; i < fin; i++) {
		cadena_descartable[i-inicio] = original[i];
		pos = i;
	}
	cadena_descartable[(pos+1)-inicio] ='\0';	
}

char comparar_letra(char a, char b) {
	if (a == b) {
		return 1;
		} else {
		return 0;
	}
};
char comparar_string(char referencia[], char cadena_rec[]) {
	char i = 0;
	char valido = 1;
	if (longitud_str(referencia) != longitud_str(cadena_rec)) {
		return 0;
	};
	while (referencia[i] != '\0') {
		if (!comparar_letra(referencia[i], cadena_rec[i])) {
			valido = 0;
		}
		i++;
	}
	return valido;
};

#endif
