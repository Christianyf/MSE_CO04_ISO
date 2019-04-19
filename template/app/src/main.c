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
#include "os_core.h"
#include "semaphore.h"
#include "sapi.h"

/*==================[macros and definitions]=================================*/
#define DELAY_T1 0x500000
#define DELAY_T2 0x120000

/*==================[internal data declaration]==============================*/
//asignar memoria
uint32_t stack1[STACK_SIZE/4];
uint32_t stack2[STACK_SIZE/4];
uint32_t stack3[STACK_SIZE/4];


uint32_t sp1,sp2,sp3;

uint32_t semaforo1;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

void * task1(void *arg){
	uint32_t i;

	while(1){

		t_delay(500);
		Board_LED_Toggle(LED_1);
		liberar_semaforo(semaforo1);
	}
	return NULL;
}
void * task2(void *arg){
	uint32_t i;

	while(1){
		tomar_semaforo(semaforo1);
		t_delay(500);
		gpioToggle(LED2);
	}
	return NULL;
}
void * task3(void *arg){

	float pi=3.1416;

	while(pi!=0){
		uint32_t i;
		t_delay(500);
		Board_LED_Toggle(LED_3);
		pi*=1.1;
	}
	return NULL;
}

/*==================[external functions definition]==========================*/

int main(void)
{

	NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS)-1);

	iniciar_vtareas();

	crear_tarea(stack1,task1,"tarea1",STACK_SIZE,1,(void *)0x11223344);
	crear_tarea(stack2,task2,"tarea2",STACK_SIZE,1,(void *)0x11223344);
	crear_tarea(stack3,task3,"tarea3",STACK_SIZE,1,(void *)0x11223344);

	semaforo1=crear_semaforo_bin();

	initHardware();

	while (1)
	{
	__WFI();
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
