/* Copyright 2018
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



/*==================[macros and definitions]=================================*/
#define STACK_SIZE 512
#define DELAY_T1 0x500000
#define DELAY_T2 0x120000
#define NTAREAS 2

typedef  unsigned long int uint32_t;
typedef unsigned char uint8_t;
typedef void *(*task_type)(void *);//puntero a función que devuelve un puntero y recibe
								   //de parametro un puntero

typedef enum state_task{ready,running,blocked}state;
typedef struct{
	uint32_t tamano;
	uint32_t sp;
	uint32_t prioridad;
	state estado;
	uint32_t contador;
}data_tarea;
/*==================[internal data declaration]==============================*/

uint32_t stack1[STACK_SIZE/4];
uint32_t stack2[STACK_SIZE/4];
uint32_t stack3[STACK_SIZE/4];

data_tarea vector_tareas[4];

uint32_t sp1,sp2,sp3;

uint32_t current_task=0;

uint32_t indice_inicio=0,indice_final=0,libre;
uint32_t indice=0, ID_tarea_running;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}
void task_return_hook(void * ret_val){
	while(1){
		__WFI();
	}

}


void schedule(void){
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	__ISB();
	__DSB();
}

void SysTick_Handler(void){
	schedule();
}

void t_delay(uint32_t tiempo){
	vector_tareas[ID_tarea_running].contador=tiempo;
	vector_tareas[ID_tarea_running].estado=blocked;
	schedule();//llama al cambio de contexto
}
void actualizar_cuenta(void){
	uint32_t i;
	for(i=1;i<NTAREAS+1;i++){
		if(vector_tareas[i].contador>0){
			vector_tareas[i].contador--;
		}else{
			vector_tareas[i].contador=0;//para asegurar un valor positivo en el contador
		}
	}
}

void * task1(void *arg){
	uint32_t i;

	while(1){
		t_delay(500);
		//for(i=0;i<DELAY_T1;i++){
			//delay por software basico
		//}
		Board_LED_Toggle(LED_1);
	}
	return NULL;
}
void * task2(void *arg){
	uint32_t i;

	while(1){
		t_delay(50);
		//for(i=0;i<DELAY_T2;i++){
			//delay por software basico
		//}
		Board_LED_Toggle(LED_2);
	}
	return NULL;
}
//Tarea 3 es mi tarea idle (nunca bloquear)

void * task3(void *arg){
	while(1){
		uint32_t i;
		for(i=0;i<DELAY_T2;i++){

		}
		Board_LED_Toggle(LED_3);
	}
	return NULL;
}

void MEF_tareas(uint32_t indi){
	switch(vector_tareas[indi].estado){
	case ready:
		vector_tareas[indi].estado=running;
		break;
	case running:
		vector_tareas[indi].estado=ready;
		break;
	case blocked:
		//revisar la condicion de bloqueo y luego...
		//vector_tareas[indi].estado=ready;
		if (vector_tareas[indi].contador==0){
			vector_tareas[indi].estado=ready;
		}
		break;
	default:
		break;
	}

}

void init_stack(
		uint32_t stack[],
		uint32_t stack_size,
		uint32_t * sp,
		task_type entry_point,
		void * arg)
{
	bzero(stack,stack_size);							//se inicializa(limpia) el stack
	stack[stack_size/4-1]=1<<24;						//xpsr en 1 modo tumb
	stack[stack_size/4-2]=(uint32_t)entry_point;		//PC
	stack[stack_size/4-3]=(uint32_t)task_return_hook;	//LR task_return_hook
	stack[stack_size/4-8]=(uint32_t)arg;				//R0, se apunta a R0
	stack[stack_size/4-9]=0xFFFFFFF9;					//LR IRQ

	*sp=(uint32_t)&(stack[stack_size/4-17]);			//se apunta(guarda) a 8 registros mas luego de R0
}

uint32_t get_next_context(uint32_t current_sp){

	uint32_t next_sp;

	actualizar_cuenta();

	libre=0;
	indice_inicio=indice_final;

	MEF_tareas(indice);

	while(libre==0){
		if(indice<NTAREAS+1){
			indice++;
		}else{
			indice=1;
		}
		if(vector_tareas[indice].estado==ready){
			indice_final=indice;
			libre=1;
		}else if(vector_tareas[indice].estado==blocked){
			MEF_tareas(indice);
		}//cambiar las condiciones if por elseif
	}
	vector_tareas[indice_inicio].sp=current_sp;
	next_sp=vector_tareas[indice_final].sp;

	MEF_tareas(indice);
	ID_tarea_running=indice;

	return next_sp;
}



/*==================[external functions definition]==========================*/

int main(void)
{

	NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS)-1);

	init_stack(stack1,STACK_SIZE,&sp1,task1,(void *)0x11223344);
	init_stack(stack2,STACK_SIZE,&sp2,task2,(void *)0x55667788);
	init_stack(stack3,STACK_SIZE,&sp3,task3,(void *)0x11227788);

	initHardware();

	vector_tareas[1].sp=sp1;
	vector_tareas[1].estado=ready;
	vector_tareas[2].sp=sp2;
	vector_tareas[2].estado=ready;
	vector_tareas[3].sp=sp3;//idle
	vector_tareas[3].estado=ready;

	while (1)
	{
	__WFI(); //wfi
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
