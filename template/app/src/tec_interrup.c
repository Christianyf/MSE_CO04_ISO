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
#include "tec_interrup.h"
#include "semaphore.h"
#include "sapi.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
estado_sec estado_secuencia=NINGUNO;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
extern uint32_t semaforo1;
extern uint32_t semaforo2;
/*==================[internal functions definition]==========================*/
void ConfiguracionInterrupciones(uint8_t canal,uint8_t puerto,uint8_t pin,LPC43XX_IRQn_Type IRQn_Tipo,uint8_t prioridad,uint8_t flanco)
{
	   Chip_SCU_GPIOIntPinSel( canal,puerto,pin );
	   //Borra el pending de la IRQ
	   Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( canal ) );
	   //Selecciona activo por flanco
	   Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( canal ) );
	   //Selecciona el tipo de flanco
	   if(flanco==FLANCOSUBIDA)
	   {
		   Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( canal ) );
	   }else if(flanco==FLANCOBAJADA)
	   {
		   Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( canal ) );
	   }else if(flanco==FLANCOS)
	   {
		   Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( canal ) );
		   Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( canal ) );
	   }
	   //Selecciona prioridad de la interrupcion
	   NVIC_SetPriority(IRQn_Tipo, prioridad);
	   //Borra el pending de la IRQ
	   NVIC_ClearPendingIRQ(IRQn_Tipo);
	   //Habilita la interrupcion
	   NVIC_EnableIRQ(IRQn_Tipo);
}

void GPIO0_IRQHandler(void){
	Board_LED_Toggle(LED_1);
	//liberar_semaforo(semaforo1);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
	Chip_PININT_ClearRiseStates( LPC_GPIO_PIN_INT, PININTCH( 0 ));
	Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, PININTCH( 0 ));
	 NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
}


void GPIO1_IRQHandler(void){
	Board_LED_Toggle(LED_2);
	//liberar_semaforo(semaforo1);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
}

void MEF_tecla(tecla *tec){
	//estado_tec estado;
	s_delay(250000);
	/*if(gpioRead( tec->gpiotec )==OFF)
	   {
			estado=FALLING;
			//tec->estado=DOWN;
	   }else if(gpioRead( tec->gpiotec )==ON){
		   estado=UP;
		   //tec->estado=UP;
	   }*/
	switch(tec->estado ){
	case UP:
		//s_delay(500000);
		if(gpioRead(tec->gpiotec)==ON){
			tec->estado=UP;
		}else{
			tec->estado=FALLING;
		}
		break;
		case FALLING:
			if(gpioRead(tec->gpiotec)==ON){
				tec->estado=UP;
			}else{
				tec->estado=DOWN;
			}
			break;
	case DOWN:
		//s_delay(500000);
		if(gpioRead(tec->gpiotec)==ON){
			tec->estado=RISING;
		}else{
			tec->estado=DOWN;
		}
		break;
	case RISING:
		if(gpioRead(tec->gpiotec)==ON){
			tec->estado=UP;
		}else{
			tec->estado=DOWN;
		}
		break;
	default:
		break;
	}

}

void MEF_secuencia(tecla * tec){

	switch(estado_secuencia){
	case NINGUNO:
		if(tec->estado==FALLING){
			estado_secuencia=SEC1;
		}else {
			estado_secuencia=NINGUNO;
		}
		break;
	case SEC1:
		if(tec->estado==FALLING){
			estado_secuencia=SEC2;
		}else{
			estado_secuencia=NINGUNO;
		}
		break;
	case SEC2:
		if(tec->estado==RISING){
			estado_secuencia=SEC3;
		}else{
			estado_secuencia=NINGUNO;
		}
		break;
	case SEC3:
		if(tec->estado==RISING){
			estado_secuencia=NINGUNO;
			//Board_LED_Toggle(LED_3);
			liberar_semaforo(semaforo2);
		}else{
			estado_secuencia=NINGUNO;
		}
		break;
	default:
		break;
	}
}

char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}
/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/
