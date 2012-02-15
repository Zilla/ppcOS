/* Copyright (c) 2012, Joakim Östlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test_proc.h"
#include "log/log.h"

OS_PROC(test1)
{
     U32 num = 0;

     INFO("Test process 1 starting");

     while(1)
     {
	  num++;

	  if( (num % 1000) == 0 )
	       INFO(STR("Test process 1: loop %u", num));

	  sleep(500);
     }
}

OS_PROC(test2)
{
     U32 num = 0;

     INFO("Test process 2 starting");

     while(1)
     {
	  num++;

	  if( (num % 1000) == 0 )
	       INFO(STR("Test process 2: loop %u", num));

	  sleep(400);
     }

}

OS_PROC(test3)
{
     U32 num = 0;

     INFO("Test process 3 starting");

     while(1)
     {
	  num++;

	  if( (num % 1000) == 0 )
	       INFO(STR("Test process 3: loop %u", num));

	  sleep(1000);
     }

}
