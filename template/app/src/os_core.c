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
 
/*==================[inclusions]=============================================*/

#include "main.h"
#include "board.h"
#include <strings.h>
#include "os_core.h"

/*==================[macros and definitions]=================================*/
#define MAX_TICK_TIME_FOR_DELAY 1000
#define MAX_VECTOR_SIZE 5
#define V_CAMBIO 2
/*==================[internal data declaration]==============================*/

data_tarea vector_p1[MAX_VECTOR_SIZE];
data_tarea vector_p2[MAX_VECTOR_SIZE];
data_tarea vector_p3[MAX_VECTOR_SIZE];
data_tarea vector_p_idle[MAX_VECTOR_SIZE];
data_tarea vector_cambio[V_CAMBIO];

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint32_t next_sp;
uint32_t indice_p1=0,indice_p2=0,indice_p3=0;
uint32_t cont_uno=0,cont_dos=0,cont_tres=0;
prioridad cont_prioridad=alta;
uint32_t stack0[STACK_SIZE/4];
uint32_t sp0;
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / MAX_TICK_TIME_FOR_DELAY);
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
	actualizar_cuenta();					//actualiza los contadores de los delays
	schedule();
}

void t_delay(uint32_t tiempo){
	uint32_t prioridad,id;
	prioridad=vector_cambio[0].prioridad;
	id=vector_cambio[0].id;
	switch(prioridad){
	case 1:
		vector_p1[id].contador=tiempo;
		vector_p1[id].estado=blocked;
		break;
	case 2:
		vector_p2[id].contador=tiempo;
		vector_p2[id].estado=blocked;
		break;
	case 3:
		vector_p3[id].contador=tiempo;
		vector_p3[id].estado=blocked;
		break;
	default:
		break;
	}

	schedule();//llama al cambio de contexto
}

void actualizar_cuenta(void){
	uint32_t i=1,j;

	for(i=1;i<MAX_VECTOR_SIZE-1;i++){
		switch(i){
		case 1:
			for(j=0;j<MAX_VECTOR_SIZE;j++){
				if(vector_p1[j].contador>0){
					vector_p1[j].contador--;
					if(vector_p1[j].contador==0){//
						MEF_tareas(i,j);//
					}//
				}else{

				}
			}
			break;
		case 2:
			for(j=0;j<MAX_VECTOR_SIZE;j++){
				if(vector_p2[j].contador>0){
					vector_p2[j].contador--;
					if(vector_p2[j].contador==0){//
						MEF_tareas(i,j);//
					}//
				}else{

				}
			}
			break;
		case 3:
			for(j=0;j<MAX_VECTOR_SIZE;j++){
				if(vector_p3[j].contador>0){
					vector_p3[j].contador--;
					if(vector_p3[j].contador==0){//
						MEF_tareas(i,j);//
					}//
				}else{

				}
			}
			break;
		default:
			break;
		}
	}


}

void MEF_tareas(uint32_t prio,uint32_t ind){
	switch(prio){
	case 1:
		switch(vector_p1[ind].estado){
		case ready:
			vector_p1[ind].estado=running;
			break;
		case running:
			vector_p1[ind].estado=ready;
			break;
		case blocked:
			//revisar la condicion de bloqueo y cambiar de estado
			if(vector_p1[ind].contador==0){
				vector_p1[ind].estado=ready;
			}
			break;
		default:
			break;
		}
		break;
	case 2:
		switch(vector_p2[ind].estado){
		case ready:
			vector_p2[ind].estado=running;
			break;
		case running:
			vector_p2[ind].estado=ready;
			break;
		case blocked:
			//revisar la condicion de bloqueo y cambiar de estado
			if(vector_p2[ind].contador==0){
				vector_p2[ind].estado=ready;
			}
			break;
		default:
			break;
		}
		break;
	case 3:
		switch(vector_p3[ind].estado){
		case ready:
			vector_p3[ind].estado=running;
			break;
		case running:
			vector_p3[ind].estado=ready;
			break;
		case blocked:
			//revisar la condicion de bloqueo y cambiar de estado
			if(vector_p3[ind].contador==0){
				vector_p3[ind].estado=ready;
			}
			break;
		default:
			break;
		}
		break;
	case 4:
		vector_p_idle[0].estado=ready;
		break;
	default:
		break;

	}

}

uint32_t get_next_context(uint32_t current_sp){
//----Cambio de contexto bajo politica de prioridades----
	uint32_t libre=0;

	MEF_tareas(vector_cambio[0].prioridad,vector_cambio[0].id);

	selec_prioridad();

	switch(cont_prioridad){

	case alta:
		while(libre==0){
			if(vector_p1[indice_p1].estado==ready){
				libre=1;
				//-------------------------------------------------------
				vector_cambio[0].sp=current_sp;
				actualizar_vInicio(vector_cambio[0].id,vector_cambio[0].prioridad);
				//-------------------------------------------------------
				vector_cambio[1].id=vector_p1[indice_p1].id;
				vector_cambio[1].prioridad=vector_p1[indice_p1].prioridad;
				vector_cambio[1].sp=vector_p1[indice_p1].sp;
				//-------------------------------------------------------
				next_sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				MEF_tareas(vector_cambio[1].prioridad,vector_cambio[1].id);
				//-------------------------------------------------------
				vector_cambio[0].id=vector_cambio[1].id;
				vector_cambio[0].prioridad=vector_cambio[1].prioridad;
				vector_cambio[0].sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				actualizar_vInicio(vector_cambio[1].id,vector_cambio[1].prioridad);
				//-------------------------------------------------------

			}else{

			}

			if(indice_p1<cont_uno-1){
				indice_p1++;
			}else{
				indice_p1=0;
			}

		}
		break;
	case media:
		while(libre==0){
			if(vector_p2[indice_p2].estado==ready){
				libre=1;
				//-------------------------------------------------------
				vector_cambio[0].sp=current_sp;
				actualizar_vInicio(vector_cambio[0].id,vector_cambio[0].prioridad);
				//-------------------------------------------------------
				vector_cambio[1].id=vector_p2[indice_p2].id;
				vector_cambio[1].prioridad=vector_p2[indice_p2].prioridad;
				vector_cambio[1].sp=vector_p2[indice_p2].sp;
				//-------------------------------------------------------
				next_sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				MEF_tareas(vector_cambio[1].prioridad,vector_cambio[1].id);
				//-------------------------------------------------------
				vector_cambio[0].id=vector_cambio[1].id;
				vector_cambio[0].prioridad=vector_cambio[1].prioridad;
				vector_cambio[0].sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				actualizar_vInicio(vector_cambio[1].id,vector_cambio[1].prioridad);
				//-------------------------------------------------------

			}else{

			}

			if(indice_p2<cont_dos-1){
				indice_p2++;
			}else{
				indice_p2=0;
			}

		}
		break;
	case baja:
		while(libre==0){
			if(vector_p3[indice_p3].estado==ready){
				libre=1;
				//-------------------------------------------------------
				vector_cambio[0].sp=current_sp;
				actualizar_vInicio(vector_cambio[0].id,vector_cambio[0].prioridad);
				//-------------------------------------------------------
				vector_cambio[1].id=vector_p3[indice_p3].id;
				vector_cambio[1].prioridad=vector_p3[indice_p3].prioridad;
				vector_cambio[1].sp=vector_p3[indice_p3].sp;
				//-------------------------------------------------------
				next_sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				MEF_tareas(vector_cambio[1].prioridad,vector_cambio[1].id);
				//-------------------------------------------------------
				vector_cambio[0].id=vector_cambio[1].id;
				vector_cambio[0].prioridad=vector_cambio[1].prioridad;
				vector_cambio[0].sp=vector_cambio[1].sp;
				//-------------------------------------------------------
				actualizar_vInicio(vector_cambio[1].id,vector_cambio[1].prioridad);
				//-------------------------------------------------------

			}else{

			}

			if(indice_p3<cont_tres-1){
				indice_p3++;
			}else{
				indice_p3=0;
			}

		}
		break;
	case idle:
		//-------------------------------------------------------
		vector_cambio[0].sp=current_sp;
		MEF_tareas(vector_cambio[0].prioridad,vector_cambio[0].id);
		actualizar_vInicio(vector_cambio[0].id,vector_cambio[0].prioridad);
		//-------------------------------------------------------
		vector_cambio[1].id=vector_p_idle[0].id;
		vector_cambio[1].prioridad=vector_p_idle[0].prioridad;
		vector_cambio[1].sp=vector_p_idle[0].sp;
		//-------------------------------------------------------
		next_sp=vector_cambio[1].sp;
		//-------------------------------------------------------
		MEF_tareas(vector_cambio[1].prioridad,vector_cambio[1].id);
		//-------------------------------------------------------
		vector_cambio[0].id=vector_cambio[1].id;
		vector_cambio[0].prioridad=vector_cambio[1].prioridad;
		vector_cambio[0].sp=vector_cambio[1].sp;
		//-------------------------------------------------------
		actualizar_vInicio(vector_cambio[1].id,vector_cambio[1].prioridad);
		//-------------------------------------------------------
		break;
	default:
		break;
	}

	return next_sp;
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

uint32_t crear_tarea(
		uint32_t stack[],
		task_type p_tarea,
		const char *const nombre,
		uint32_t size,
		uint32_t prioridad,
		void* arg)
{
	switch(prioridad){
	case 1:
		init_stack(stack,size,&vector_p1[cont_uno].sp,p_tarea,(void *)arg);
		vector_p1[cont_uno].prioridad=prioridad;
		vector_p1[cont_uno].tamano=size;
		vector_p1[cont_uno].id=cont_uno;
		vector_p1[cont_uno].estado=ready;

		if(cont_uno<MAX_VECTOR_SIZE-1){
			cont_uno++;
		}else{
			//err se pasa del numero maximo del vector
		}
		break;
	case 2:
		init_stack(stack,size,&vector_p2[cont_dos].sp,p_tarea,(void *)arg);
		vector_p2[cont_dos].prioridad=prioridad;
		vector_p2[cont_dos].tamano=size;
		vector_p2[cont_dos].id=cont_dos;
		vector_p2[cont_dos].estado=ready;

		if(cont_dos<MAX_VECTOR_SIZE-1){
			cont_dos++;
		}else{
			//err se pasa del numero maximo del vector
		}
		break;
	case 3:

		init_stack(stack,size,&vector_p3[cont_tres].sp,p_tarea,(void *)arg);
		vector_p3[cont_tres].prioridad=prioridad;
		vector_p3[cont_tres].tamano=size;
		vector_p3[cont_tres].id=cont_tres;
		vector_p3[cont_tres].estado=ready;

		if(cont_tres<MAX_VECTOR_SIZE-1){
			cont_tres++;
		}else{
			//err se pasa del numero maximo del vector
		}

		break;
	default:
		break;
	}

	return 0;
}

void * task_idle(void *arg){
	uint32_t i;

	while(1){
		for(i=0;i<500000;i++){
			//delay por software basico
		}
		Board_LED_Toggle(LED_BLUE);
	}
	return NULL;
}

void iniciar_vtareas(void){
	uint32_t i;

	for(i=0;i<MAX_VECTOR_SIZE;i++)
	{
		vector_p1[i].contador=0;
		vector_p1[i].estado=suspended;
		vector_p1[i].prioridad=1;
		vector_p1[i].sp=0;
		vector_p1[i].tamano=0;
		vector_p1[i].id=i;
	}
	for(i=0;i<MAX_VECTOR_SIZE;i++)
	{
		vector_p2[i].contador=0;
		vector_p2[i].estado=suspended;
		vector_p2[i].prioridad=2;
		vector_p2[i].sp=0;
		vector_p2[i].tamano=0;
		vector_p2[i].id=i;
	}
	for(i=0;i<MAX_VECTOR_SIZE;i++)
	{
		vector_p3[i].contador=0;
		vector_p3[i].estado=suspended;
		vector_p3[i].prioridad=3;
		vector_p3[i].sp=0;
		vector_p3[i].tamano=0;
		vector_p3[i].id=i;
	}
	//inicializar tarea idle
	init_stack(stack0,STACK_SIZE,&sp0,task_idle,(void *)0x11223344);
	vector_p_idle[0].contador=0;
	vector_p_idle[0].estado=ready;
	vector_p_idle[0].prioridad=4;
	vector_p_idle[0].sp=sp0;
	vector_p_idle[0].id=0;

}
uint32_t selec_prioridad(void){
	uint32_t i,j;
	uint32_t ready_p1=0,ready_p2=0,ready_p3=0;

	for(j=0;j<MAX_VECTOR_SIZE;j++){
		if(vector_p1[j].estado==ready){
			ready_p1++;
		}
	}

	if(ready_p1>0){
		cont_prioridad=alta;
	}else{
			for(j=0;j<MAX_VECTOR_SIZE;j++){
				if(vector_p2[j].estado==ready){
				ready_p2++;
				}
			}
			if(ready_p2>0){
				cont_prioridad=media;
			}else{
					for(j=0;j<MAX_VECTOR_SIZE;j++){
						if(vector_p3[j].estado==ready){
							ready_p3++;
						}
					}
					if(ready_p3>0){
						cont_prioridad=baja;
					}else{
						cont_prioridad=idle;
					}
			}
	}

	return 0;
}
void actualizar_vInicio(uint32_t id,uint32_t prioridad){


	switch(prioridad){
	case 1:
		vector_p1[id].sp=vector_cambio[0].sp;
		break;
	case 2:
		vector_p2[id].sp=vector_cambio[0].sp;
		break;
	case 3:
		vector_p3[id].sp=vector_cambio[0].sp;
		break;
	case 4:
		vector_p_idle[id].sp=vector_cambio[0].sp;
		break;
	default:
		break;
	}

}
/*==================[external functions definition]==========================*/


/*==================[end of file]============================================*/
