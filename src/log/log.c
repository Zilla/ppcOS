 /* Copyright (c) 2011, Joakim Östlund
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

#include "log.h"
#include "timer/timer.h"
#include "arch/ppc440.h"
#include "uart/uart.h"

#include <stdio.h>
#include <stdarg.h>

char *msgTypes[] = { "INFO", "WARNING", "ERROR" };
char *strNull = "NULL";
char msgBuffer[MAX_LOG_STR_LEN];

void write_log(char *filename, U32 lineno, const char *function, const char *message, U8 type)
{
     U32 msr;
     char logBuffer[MAX_LOG_STR_LEN];
     int logLen;

     if( !filename )
	  filename = strNull;
     if( !function )
	  function = strNull;
     if( !message )
	  message = strNull;
     if( type > LOG_TYPE_MAX )
	  type = LOG_TYPE_MAX;

     MFMSR(msr);
     WRTEEI(0);
     ISYNC;

     logLen = snprintf(logBuffer, MAX_LOG_STR_LEN, "[%u] %s %s:%u %s: %s\n",
		       get_ticks(), function, filename, lineno, msgTypes[type], message);
     uart_write( logBuffer, logLen );

     MTMSR(msr);
     ISYNC;
}

char *format_log_string(char *str, size_t size, const char *format, ...)
{
     U32 msr;

     MFMSR(msr);
     WRTEEI(0);
     ISYNC;

     va_list ap;
     va_start(ap, format);
     vsnprintf(str, size, format, ap);
     va_end(ap);

     MTMSR(msr);
     ISYNC;

     return str;
}
