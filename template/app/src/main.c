/* Copyright 2019
 * All rights reserved.
 *
 * This file is part of arquitecturaDeMicroprocesadores.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
/*==================[inclusions]=============================================*/

#include "main.h"
#include "board.h"
#include <strings.h>
#include <stdio.h>
#include "os_core.h"
#include "semaphore.h"
#include "sapi.h"
#include "tec_interrup.h"

/*==================[macros and definitions]=================================*/

#define UART_PC         UART_USB
#define BAUD_RATE 		115200

/*==================[internal data declaration]==============================*/
//asignar memoria
uint32_t stack1[STACK_SIZE/4];
uint32_t stack2[STACK_SIZE/4];
uint32_t stack3[STACK_SIZE/4];


uint32_t sp1,sp2,sp3;


uint32_t semaforo2;

uint32_t mutex1;

tecla tecla1={UP,TEC2};
tecla tecla2={UP,TEC3};

uint32_t time_tec=0;
uint32_t flag=0;
uint32_t contador_eventos=0;

s_timer timer_sistema;
s_timer timer_1=0;

//static char uartBuff[10];
/*==================[internal functions declaration]=========================*/
void imprimir_datos(uint32_t t,uint32_t t1,uint32_t t2);
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void * task1(void *arg){
	uint32_t i;

	while(1){

		if(tomar_mutex(mutex1)==1){

			MEF_tecla(&tecla1);
			if(tecla1.estado==FALLING){
				tecla1.t_falling=timer_sistema;
				MEF_secuencia(&tecla1);
			}
			if(tecla1.estado==RISING){
				tecla1.t_rising=timer_sistema;
				MEF_secuencia(&tecla1);
			}
			MEF_tecla(&tecla2);
			if(tecla2.estado==FALLING){
				tecla2.t_falling=timer_sistema;
				MEF_secuencia(&tecla2);
			}
			if(tecla2.estado==RISING){
				tecla2.t_rising=timer_sistema;
				MEF_secuencia(&tecla2);
			}
			liberar_mutex(mutex1);
		}

	}
	return NULL;
}
void * task2(void *arg){

	uint32_t caso,t1,t2;

	while(1){
		if(tomar_semaforo(semaforo2)==1){

			if(tecla1.t_falling<tecla2.t_falling && tecla1.t_rising<tecla2.t_rising){
				caso=1;
			}

			if(tecla1.t_falling<tecla2.t_falling && tecla1.t_rising>tecla2.t_rising){
				caso=2;
			}
			if(tecla1.t_falling>tecla2.t_falling && tecla1.t_rising<tecla2.t_rising){
				caso=3;
			}

			if(tecla1.t_falling>tecla2.t_falling && tecla1.t_rising>tecla2.t_rising){
				caso=4;
			}

			switch(caso){
			case 1:
				gpioWrite(LED3,ON);
				uartWriteString( UART_PC, "Led verde encendido\r\n" );
				t1=tecla2.t_falling-tecla1.t_falling;
				t2=tecla2.t_rising-tecla1.t_rising;
				timer_1=t1+t2;
				imprimir_datos(timer_1,t1,t2);
				while(timer_1>0){
					__WFI();
				}
				gpioWrite(LED3,OFF);
				break;
			case 2:
				gpioWrite(LED2,ON);
				uartWriteString( UART_PC, "Led rojo encendido\r\n" );
				t1=tecla2.t_falling-tecla1.t_falling;
				t2=tecla1.t_rising-tecla2.t_rising;
				timer_1=t1+t2;
				imprimir_datos(timer_1,t1,t2);

				while(timer_1>0){
					__WFI();
				}
				gpioWrite(LED2,OFF);
				break;
			case 3:
				gpioWrite(LED1,ON);
				uartWriteString( UART_PC, "Led amarillo encendido\r\n" );
				t1=tecla1.t_falling-tecla2.t_falling;
				t2=tecla2.t_rising-tecla1.t_rising;
				timer_1=t1+t2;
				imprimir_datos(timer_1,t1,t2);

				while(timer_1>0){
					__WFI();
				}
				gpioWrite(LED1,OFF);
				break;
			case 4:
				gpioWrite(LEDB,ON);
				uartWriteString( UART_PC, "Led azul encendido\r\n" );
				t1=tecla1.t_falling-tecla2.t_falling;
				t2=tecla1.t_rising-tecla2.t_rising;
				timer_1=t1+t2;
				imprimir_datos(timer_1,t1,t2);

				while(timer_1>0){
					__WFI();
				}
				gpioWrite(LEDB,OFF);
				break;
			default:
				break;
			}
		}
		liberar_mutex(mutex1);

	}
	return NULL;
}

/*==================[external functions definition]==========================*/

int main(void)
{

	NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS)-1);

	uartConfig( UART_PC, BAUD_RATE );
	uartWriteString( UART_PC, "UART PC configurada\r\n" );

	iniciar_vtareas();

	crear_tarea(stack1,task1,"tarea1",STACK_SIZE,1,(void *)0x11223344);
	crear_tarea(stack2,task2,"tarea2",STACK_SIZE,1,(void *)0x11223344);

	semaforo2=crear_semaforo_bin();
	mutex1=crear_mutex();

	initHardware();

	while (1)
	{
	__WFI();
	}
}

/** @} doxygen end group definition */
void imprimir_datos(uint32_t t,uint32_t t1,uint32_t t2){
	fprintf( stdout, "Tiempo encendido: %d ms\r\n", t );
	fprintf( stdout, "Tiempo entre flacos descendentes: %d ms\r\n", t1 );
	fprintf( stdout, "Tiempo entre flacos ascendentes: %d ms\r\n", t2 );
}
/*==================[end of file]============================================*/
