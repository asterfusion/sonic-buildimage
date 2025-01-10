/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2022, congatec GmbH. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-clause license which 
 * accompanies this distribution. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the BSD 2-clause license for more details.
 *
 * The full text of the license may be found at:        
 * http://opensource.org/licenses/BSD-2-Clause   
 *
 *---------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OsAL.h"
#include "generic.h"

FILE *fpLogfile = NULL;

/* print out string via console */
void report(char *pBuf)
{
  printf("%s",pBuf);
}
  
/* converts ascii to wide string */
wchar_t *A2W(wchar_t *d, const char *s, unsigned int cnt)
{
  wchar_t *ret=d;
  if (!s || !d || !cnt) return NULL;
  while (cnt-- && *s) *d++=(wchar_t)*s++;
  if (cnt) *d=0;
  return ret;
}

/* logfile functions */


/* writes a zero terminated buffer to the logfile */
void LogBuf(char *pBuf)
{
    if(fpLogfile)
    {
        fprintf(fpLogfile,"%s",pBuf);
    }    
}

/*---------------------------------------------------------------------------
 * Name: LogFlushReopen
 * Desc: Force log file update and write to file system.
 * Inp:  none
 * Outp: TRUE   - SUCCESS
 *       FALSE  - Failed to write/re-open
 *---------------------------------------------------------------------------
 */
unsigned char LogFlushReopen(char *szLogFile)
{
    /* Ensure log file is written to file system */
    fflush(fpLogfile);
    fclose(fpLogfile);
    fpLogfile = NULL;
    /* Re-open and append to existing file */
    if ((fpLogfile = fopen(szLogFile, "a+t"))== NULL)
    {
        return FALSE;
    }
    return TRUE;
}

unsigned char LogOpen(char *szLogFile, char *mode)
{
	fpLogfile = NULL;
	if((fpLogfile = fopen(&szLogFile[0], mode))!=NULL)
	  {
  		return TRUE;
	}
	return FALSE;
}

void LogClose()
{
	fclose(fpLogfile);
	fpLogfile = NULL;
}
