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

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

/*==================[inclusions]=============================================*/
#include "board.h"
#include "os_core.h"
/*==================[macros]=================================================*/
#define MAX_SEMAFORO 5
/*==================[typedef]================================================*/
typedef enum estado_s{tomado,libre,no_operativo}estado_semaforo;
typedef struct{
	uint32_t id;
	estado_semaforo estado;
	uint32_t prioridad_tarea_tomada;
	uint32_t id_tarea_tomada;
}semaforo;
/*==================[external data declaration]==============================*/

extern data_tarea vector_p1[5];
extern data_tarea vector_p2[5];
extern data_tarea vector_p3[5];
//extern data_tarea vector_p_idle[1];
extern data_tarea vector_cambio[2];

/*==================[external functions declaration]=========================*/
uint32_t crear_semaforo_bin(void);
void tomar_semaforo(uint32_t id_semaforo);
uint32_t liberar_semaforo(uint32_t id_semaforo);

/*==================[end of file]============================================*/
#endif /* #ifndef _SEMAPHORE_H_ */
