/* Copyright 2004,2007 INRIA
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : library_error.c                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module provides an error handling  **/
/**                routines to process errors generated by **/
/**                the routines of the libSCOTCH library.  **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     02 oct 1998     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to     01 nov 2001     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY_ERROR

#include "module.h"
#include "common.h"
#include "scotch.h"

/********************************/
/*                              */
/* The error handling routines. */
/*                              */
/********************************/

static char                 SCOTCH_errorProgName[32] = "";

/* This routine sets the program name for
** error reporting.
** It returns:
** - VOID  : in all cases.
*/

void
SCOTCH_errorProg (
const char * const          progstr)              /*+ Program name +*/
{
  strncpy (SCOTCH_errorProgName, progstr, 29);
  SCOTCH_errorProgName[29] = '\0';
  strcat  (SCOTCH_errorProgName, ": ");
}

/* This routine prints an error message with
** a variable number of arguments, as printf ()
** does, and exits.
** It returns:
** - EXIT  : in all cases.
*/

void
SCOTCH_errorPrint (
const char * const          errstr,               /*+ printf-like variable argument list */
...)
{
  va_list             errlist;                    /* The argument list of the call */

#ifdef SCOTCH_PTSCOTCH
  int                 proclocnum;

  MPI_Comm_rank (MPI_COMM_WORLD, &proclocnum);
  fprintf  (stderr, "%s(%d) ERROR: ", SCOTCH_errorProgName, proclocnum);
#else /* SCOTCH_PTSCOTCH */
  fprintf  (stderr, "%sERROR: ", SCOTCH_errorProgName);
#endif /* SCOTCH_PTSCOTCH */
  va_start (errlist, errstr);
  vfprintf (stderr, errstr, errlist);             /* Print arguments */
  va_end   (errlist);
  fprintf  (stderr, "\n");
  fflush   (stderr);                              /* In case it has been set to buffered mode */
}

/* This routine prints a warning message with
** a variable number of arguments, as printf ()
** does.
** It returns:
** - VOID  : in all cases.
*/

void
SCOTCH_errorPrintW (
const char * const          errstr,               /*+ printf-like variable argument list */
...)
{
  va_list             errlist;                    /* The argument list of the call */

  fprintf  (stderr, "%sWARNING: ", SCOTCH_errorProgName);
  va_start (errlist, errstr);
  vfprintf (stderr, errstr, errlist);             /* Print arguments */
  va_end   (errlist);
  fprintf  (stderr, "\n");
  fflush   (stderr);                              /* In case it has been set to buffered mode */
}
