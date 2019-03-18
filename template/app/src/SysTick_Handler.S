/** Ejemplo de archivo de assembler */

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
	.syntax unified
	.text
	.global SysTick_Handler

	//.extern sp1//para poder acceder a sp1 desde codigo c
	.extern get_next_context

	.thumb_func

SysTick_Handler:
	push {r4-r11,lr}

	mrs r0, msp			//guardo el valor de msp actual(envio msp mediante r0)
	bl get_next_context	//llamo a la funcion get_next_context

	//msp a sp1
	//ldr r0,=sp1			//r0=&sp1 (apunto a sp1)
	//ldr r0,[r0]			//r0=sp1  (cargo el valor de sp1)

	msr msp,r0			//msp es el stack pointer principal (cargo el siguiente sp devuelto por get_next_context)

	pop {r4-r11,lr}
return:
	bx lr
	/* otras alternativas para el retorno */
	/* 1. mov pc,lr
	/  2. bx lr */
	/* pop {pc} */
///////////////////////////////////////////////////
//	.extern sp1
//	push{r4-r11,lr}
//	ldr r0,=sp1//r0=&sp1
//	ldr r0,[r0]//r0=*r0

//	msr msp,r0//msp=sp1

//	push{r4-r11,lr}
//return:
//	bx lr



