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
#include "os_core.h"
#include "semaphore.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
semaforo vector_semaforos[MAX_SEMAFORO];
mutex vector_mutex[MAX_SEMAFORO];
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint32_t indice_vec_semaforo=0;
uint32_t indice_vec_mutex=0;
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
uint32_t crear_semaforo_bin(void){
	uint32_t id;

	if(indice_vec_semaforo<MAX_SEMAFORO){
		vector_semaforos[indice_vec_semaforo].estado=tomado;
		vector_semaforos[indice_vec_semaforo].id=indice_vec_semaforo;
		id=indice_vec_semaforo;
		indice_vec_semaforo++;
		return id;
	}else{
		//error no se puede crear el semáforo
		return MAX_SEMAFORO;
	}

}

uint32_t crear_mutex(void){
	uint32_t id;

	if(indice_vec_mutex<MAX_SEMAFORO){
		vector_mutex[indice_vec_mutex].estado=libre_m;
		vector_mutex[indice_vec_mutex].id=indice_vec_mutex;
		id=indice_vec_mutex;
		indice_vec_mutex++;
		return id;
	}else{
		//error no se puede crear el semáforo
		return MAX_SEMAFORO;
	}

}

uint32_t tomar_semaforo(uint32_t id_semaforo){

	uint32_t prioridad,id;
	prioridad=vector_cambio[0].prioridad;
	id=vector_cambio[0].id;

	switch(vector_semaforos[id_semaforo].estado){
	case tomado:
		vector_semaforos[id_semaforo].id_tarea_tomada=id;
		vector_semaforos[id_semaforo].prioridad_tarea_tomada=prioridad;

		switch(prioridad){
		case 1:
			vector_p1[id].estado=blocked;
			vector_p1[id].contador=-1;
			break;
		case 2:
			vector_p2[id].estado=blocked;
			vector_p2[id].contador=-1;
			break;
		case 3:
			vector_p3[id].estado=blocked;
			vector_p3[id].contador=-1;
			break;
		default:
			break;
		}
		schedule();
		return 0;
		break;
	case libre:
		vector_semaforos[id_semaforo].estado=tomado;
		vector_semaforos[id_semaforo].id_tarea_tomada=id;
		vector_semaforos[id_semaforo].prioridad_tarea_tomada=prioridad;
		return 1;
		break;
	default:
		return 3;
		break;
	}

}


uint32_t tomar_mutex(uint32_t id_mutex){

	uint32_t prioridad,id;
	prioridad=vector_cambio[0].prioridad;
	id=vector_cambio[0].id;

	switch(vector_mutex[id_mutex].estado){
	case tomado_m:
		vector_mutex[id_mutex].id_tarea_tomada=id;
		vector_mutex[id_mutex].prioridad_tarea_tomada=prioridad;

		switch(prioridad){
		case 1:
			vector_p1[id].estado=blocked;
			vector_p1[id].contador=-1;
			break;
		case 2:
			vector_p2[id].estado=blocked;
			vector_p2[id].contador=-1;
			break;
		case 3:
			vector_p3[id].estado=blocked;
			vector_p3[id].contador=-1;
			break;
		default:
			break;
		}
		schedule();
		return 0;
		break;
	case libre_m:
		vector_mutex[id_mutex].estado=tomado_m;
		vector_mutex[id_mutex].id_tarea_tomada=id;
		vector_mutex[id_mutex].prioridad_tarea_tomada=prioridad;
		return 1;
		break;
	default:
		return 3;
		break;
	}

}

uint32_t liberar_semaforo(uint32_t id_semaforo){
	uint32_t prioridad,id;


	vector_semaforos[id_semaforo].estado=libre;

	prioridad=vector_semaforos[id_semaforo].prioridad_tarea_tomada;
	id=vector_semaforos[id_semaforo].id_tarea_tomada;

	switch(prioridad){
	case 1:
		vector_p1[id].estado=ready;
		vector_p1[id].contador=0;
		break;
	case 2:
		vector_p2[id].estado=ready;
		vector_p2[id].contador=0;
		break;
	case 3:
		vector_p3[id].estado=ready;
		vector_p3[id].contador=0;
		break;
	default:
		break;
	}

	return 0;
}

uint32_t liberar_mutex(uint32_t id_mutex){
	uint32_t prioridad,id;


	vector_mutex[id_mutex].estado=libre_m;

	prioridad=vector_mutex[id_mutex].prioridad_tarea_tomada;
	id=vector_mutex[id_mutex].id_tarea_tomada;

	switch(prioridad){
	case 1:
		vector_p1[id].estado=ready;
		vector_p1[id].contador=0;
		break;
	case 2:
		vector_p2[id].estado=ready;
		vector_p2[id].contador=0;
		break;
	case 3:
		vector_p3[id].estado=ready;
		vector_p3[id].contador=0;
		break;
	default:
		break;
	}

	return 0;
}
/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/
