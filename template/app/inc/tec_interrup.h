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

#ifndef _TEC_INTERRUP_H_
#define _TEC_INTERRUP_H_

/*==================[inclusions]=============================================*/
#include "board.h"
#include "os_core.h"
#include "sapi.h"
/*==================[macros]=================================================*/
#define	TEC1_CANAL		0
#define	TEC1_PUERTO		0
#define	TEC1_PIN		4
#define	TEC1_FLANCO		3
#define TEC1_IRQn_TIPO	PIN_INT0_IRQn

#define PRIORIDAD		4
#define FLANCOSUBIDA	1
#define FLANCOBAJADA	2
#define FLANCOS			3

#define	TEC2_CANAL		1
#define	TEC2_PUERTO		0
#define	TEC2_PIN		8
#define	TEC2_FLANCO		3
#define TEC2_IRQn_TIPO	PIN_INT1_IRQn
/*==================[typedef]================================================*/
typedef enum{UP,FALLING,DOWN,RISING}estado_tec;
typedef enum{NINGUNO,SEC1,SEC2,SEC3}estado_sec;
typedef struct{
	estado_tec estado;
	gpioMap_t gpiotec;
	uint32_t t_falling;
	uint32_t t_rising;
}tecla;
/*==================[external data declaration]==============================*/
extern tecla tecla1;
extern tecla tecla2;
/*==================[external functions declaration]=========================*/
void ConfiguracionInterrupciones(uint8_t canal,uint8_t puerto,uint8_t pin,LPC43XX_IRQn_Type IRQn_Tipo,uint8_t prioridad,uint8_t flanco);
void GPIO0_IRQHandler(void);
void GPIO1_IRQHandler(void);
void MEF_tecla(tecla * tec);
void MEF_secuencia(tecla * tec);
char* itoa(int value, char* result, int base);
/*==================[end of file]============================================*/
#endif /* #ifndef _TEC_INTERRUP_H_ */
