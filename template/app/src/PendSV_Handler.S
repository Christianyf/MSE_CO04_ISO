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
	.global PendSV_Handler
	.extern get_next_context

	.thumb_func


PendSV_Handler:
	//-----Contexto FPU-----
	tst lr,0x10			//lr (EXc_RETURN) and 0x10 comparación
	it eq				//si el resultado es igual a cero
	vpusheq {s16-s31}	//pusheo s16 a s32 a la pila fuerzo al staking s0-s15 fpscr
	//----------------------
	push {r4-r11,lr}

	mrs r0, msp			//guardo el valor de msp actual(envio msp mediante r0)
	bl get_next_context	//llamo a la funcion get_next_context
	msr msp,r0			//msp es el stack pointer principal (cargo el siguiente sp devuelto por get_next_context)

	pop {r4-r11,lr}

	//----- Contexto FPU-----
	tst lr,0x10
	it eq
	vpopeq {s16-s31}
return:
	bx lr




