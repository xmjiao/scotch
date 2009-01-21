/* Copyright 2004,2007,2008 ENSEIRB, INRIA & CNRS
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
/**   NAME       : gord.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a sparse matrix graph ordering  **/
/**                software.                               **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to   : 21 aug 1998     **/
/**                # Version 3.3  : from : 07 oct 1998     **/
/**                                 to   : 31 may 1999     **/
/**                # Version 3.4  : from : 07 oct 1998     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 02 feb 2002     **/
/**                                 to   : 08 jan 2006     **/
/**                # Version 5.0  : from : 26 jan 2007     **/
/**                                 to   : 16 mar 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GORD

#include "module.h"
#include "common.h"
#include "scotch.h"
#include "gord.h"

/*
**  The static and global definitions.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gord [<input source file> [<output ordering file> [<output log file>]]] <options>",
  "  -h         : Display this help",
  "  -m<file>   : Save column block mapping data to <file>",
  "  -o<strat>  : Set ordering strategy (see user's manual)",
  "  -t<file>   : Save partitioning tree data to <file>",
  "  -V         : Print program version and copyright",
  "  -v<verb>   : Set verbose mode to <verb> :",
  "                 s  : strategy information",
  "                 t  : timing information",
  "",
  "See default strategy with option '-vs'",
  NULL };

/******************************/
/*                            */
/* This is the main function. */
/*                            */
/******************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Num          vertnbr;                    /* Number of vertices */
  SCOTCH_Graph        grafdat;                    /* Source graph       */
  SCOTCH_Strat        ordestrat;                  /* Ordering strategy  */
  SCOTCH_Ordering     ordedat;                    /* Graph ordering     */
  SCOTCH_Num *        permtab;                    /* Permutation array  */
  Clock               runtime[2];                 /* Timing variables   */
  int                 flagval;
  int                 i, j;

  errorProg ("gord");

  intRandInit ();

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flagval = C_FLAGNONE;
  SCOTCH_stratInit (&ordestrat);

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes                        */
    if ((argv[i][0] != '-') || (argv[i][1] == '\0') || (argv[i][1] == '.')) { /* If found a file name */
      if (C_fileNum < C_FILEARGNBR)               /* File name has been given                         */
        C_fileTab[C_fileNum ++].name = argv[i];
      else
        errorPrint ("main: too many file names given");
    }
    else {                                        /* If found an option name */
      switch (argv[i][1]) {
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'M' :                                /* Output separator mapping */
        case 'm' :
          flagval |= C_FLAGMAPOUT;
          if (argv[i][2] != '\0')
            C_filenamemapout = &argv[i][2];
          break;
        case 'O' :                                /* Ordering strategy */
        case 'o' :
          SCOTCH_stratExit (&ordestrat);
          SCOTCH_stratInit (&ordestrat);
          SCOTCH_stratGraphOrder (&ordestrat, &argv[i][2]);
          break;
        case 'T' :                                /* Output separator tree */
        case 't' :
          flagval |= C_FLAGTREOUT;
          if (argv[i][2] != '\0')
            C_filenametreout = &argv[i][2];
          break;
        case 'V' :
          fprintf (stderr, "gord, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004,2007,2008 ENSEIRB, INRIA & CNRS, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
        case 'v' :                               /* Output control info */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'S' :
              case 's' :
                flagval |= C_FLAGVERBSTR;
                break;
              case 'T' :
              case 't' :
                flagval |= C_FLAGVERBTIM;
                break;
              default :
                errorPrint ("main: unprocessed parameter \"%c\" in \"%s\"", argv[i][j], argv[i]);
            }
          }
          break;
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
      }
    }
  }

  fileBlockOpen (C_fileTab, C_FILENBR);           /* Open all files */

  clockInit  (&runtime[0]);
  clockStart (&runtime[0]);

  SCOTCH_graphInit (&grafdat);                    /* Create graph structure    */
  SCOTCH_graphLoad (&grafdat, C_filepntrsrcinp, -1, 2); /* Read source graph   */
  SCOTCH_graphSize (&grafdat, &vertnbr, NULL);    /* Get graph characteristics */

  clockStop  (&runtime[0]);                       /* Get input time */
  clockInit  (&runtime[1]);
  clockStart (&runtime[1]);

  if ((permtab = (SCOTCH_Num *) memAlloc (vertnbr * sizeof (SCOTCH_Num))) == NULL) {
    errorPrint ("main: out of memory");
    return     (1);
  }
  SCOTCH_graphOrderInit    (&grafdat, &ordedat, permtab, NULL, NULL, NULL, NULL); /* Create ordering */
  SCOTCH_graphOrderCompute (&grafdat, &ordedat, &ordestrat); /* Perform ordering */

  clockStop (&runtime[1]);                        /* Get ordering time */

#ifdef SCOTCH_DEBUG_ALL
  if (SCOTCH_graphOrderCheck (&grafdat, &ordedat) != 0)
    return (1);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[0]);

  SCOTCH_graphOrderSave (&grafdat, &ordedat, C_filepntrordout);  /* Write ordering    */
  if (flagval & C_FLAGMAPOUT)                     /* If mapping wanted                */
    SCOTCH_graphOrderSaveMap (&grafdat, &ordedat, C_filepntrmapout); /* Write mapping */
  if (flagval & C_FLAGTREOUT)                     /* If separator tree wanted         */
    SCOTCH_graphOrderSaveTree (&grafdat, &ordedat, C_filepntrtreout); /* Write tree   */

  clockStop (&runtime[0]);                        /* Get output time */

  if (flagval & C_FLAGVERBSTR) {
    fprintf (C_filepntrlogout, "S\tStrat=");
    SCOTCH_stratSave (&ordestrat, C_filepntrlogout);
    putc ('\n', C_filepntrlogout);
  }
  if (flagval & C_FLAGVERBTIM) {
    fprintf (C_filepntrlogout, "T\tOrder\t\t%g\nT\tI/O\t\t%g\nT\tTotal\t\t%g\n",
             (double) clockVal (&runtime[1]),
             (double) clockVal (&runtime[0]),
             (double) clockVal (&runtime[0]) +
             (double) clockVal (&runtime[1]));
  }

  fileBlockClose (C_fileTab, C_FILENBR);          /* Always close explicitely to end eventual (un)compression tasks */

  SCOTCH_graphOrderExit (&grafdat, &ordedat);
  SCOTCH_stratExit      (&ordestrat);
  SCOTCH_graphExit      (&grafdat);
  memFree               (permtab);

#ifdef COMMON_PTHREAD
  pthread_exit ((void *) 0);                      /* Allow potential (un)compression tasks to complete */
#endif /* COMMON_PTHREAD */
  return (0);
}
