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
//#include "suma.h"
//#include "asm.h"
#include <strings.h>



/*==================[macros and definitions]=================================*/
#define STACK_SIZE 512

typedef  unsigned long int uint32_t;
typedef unsigned char uint8_t;
typedef void *(*task_type)(void *);//puntero a función que devuelve un puntero y recibe
								   //de parametro un puntero
/*==================[internal data declaration]==============================*/
uint32_t pausems_count;

uint32_t stack1[STACK_SIZE/4];
uint32_t stack2[STACK_SIZE/4];

uint32_t sp1,sp2;
uint32_t current_task=0;
/*==================[internal functions declaration]=========================*/

//static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
	//SysTick_Config(10);
}
void task_return_hook(void * ret_val){
	while(1){
		__WFI();
	}

}

void * task1(void *arg){
	while(1){
		__WFI();
	}
	return NULL;
}
void * task2(void *arg){
	while(1){
		__WFI();
	}
	return NULL;
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

	*sp=(uint32_t)&(stack[stack_size/4-17]);//se apunta(guarda) a 8 registros mas luego de R0
	//*sp=&(stack[stack_size/4-17]);
}

uint32_t get_next_context(uint32_t current_sp){

	uint32_t next_sp;

	switch(current_task){
	case 0:
		//contexto inicial
		next_sp=sp1;
		current_task=1;
		break;
	case 1:
		//contexto tarea 1
		sp1=current_sp;
		next_sp=sp2;
		current_task=2;
		break;
	case 2:
		//contexto tarea 2
		sp2=current_sp;
		next_sp=sp1;
		current_task=1;
		break;
	default:
		while(1){
			__WFI();
		}
		break;
	}
	return next_sp;
}

/*==================[external functions definition]==========================*/

int main(void)
{
	init_stack(stack1,STACK_SIZE,&sp1,task1,(void *)0x11223344);
	init_stack(stack2,STACK_SIZE,&sp2,task2,(void *)0x55667788);

	initHardware();

	while (1)
	{
	__WFI(); //wfi
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
