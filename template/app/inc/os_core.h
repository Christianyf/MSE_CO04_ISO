/* Copyright 2018
 * All rights reserved.
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

#ifndef _OS_CORE_H_
#define _OS_CORE_H_

/*==================[inclusions]=============================================*/
#include "board.h"
/*==================[macros]=================================================*/
#define STACK_SIZE 512
/*==================[typedef]================================================*/
typedef  unsigned long int uint32_t;
typedef unsigned char uint8_t;
typedef void *(*task_type)(void *);//puntero a función que devuelve un puntero y recibe
								   //de parametro un puntero

typedef enum state_task{ready,running,blocked,suspended}state;
typedef enum priori{alta,media,baja,idle}prioridad;

typedef struct{
	uint32_t tamano;
	uint32_t sp;
	uint32_t prioridad;
	state estado;
	uint32_t contador;
	uint32_t id;
}data_tarea;

/*==================[external data declaration]==============================*/

extern uint32_t sp1,sp2,sp3;

extern uint32_t stack1[512/4];
extern uint32_t stack2[512/4];
extern uint32_t stack3[512/4];

/*==================[external functions declaration]=========================*/
void initHardware(void);
void task_return_hook(void * ret_val);
void schedule(void);
void SysTick_Handler(void);
void t_delay(uint32_t tiempo);
void actualizar_cuenta(void);
void MEF_tareas(uint32_t prio,uint32_t ind);
void iniciar_vtareas(void);
uint32_t get_next_context(uint32_t current_sp);

uint32_t selec_prioridad(void);
void actualizar_vInicio(uint32_t id,uint32_t prioridad);

void init_stack(
		uint32_t stack[],
		uint32_t stack_size,
		uint32_t * sp,
		task_type entry_point,
		void * arg);

uint32_t crear_tarea(
		uint32_t stack[],
		task_type p_tarea,
		const char *const nombre,
		uint32_t size,
		uint32_t prioridad,
		void* arg);

/*==================[end of file]============================================*/
#endif /* #ifndef _OS_CORE_H_ */
