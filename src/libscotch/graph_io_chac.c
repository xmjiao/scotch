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
/**   NAME       : graph_io_chac.c                         **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the I/O routines   **/
/**                for handling the Chaco graph format.    **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 06 nov 1997     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.3  : from : 13 dec 1998     **/
/**                                 to     24 dec 1998     **/
/**                # Version 3.4  : from : 05 oct 1999     **/
/**                                 to   : 04 feb 2000     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     19 jan 2004     **/
/**                # Version 4.0  : from : 04 feb 2007     **/
/**                                 to     04 feb 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_IO_CHAC

#include "module.h"
#include "common.h"
#include "geom.h"
#include "graph.h"

/* This routine loads the geometrical graph
** in the Chaco graph format, and allocates
** the proper structures.
** - 0   : on success.
** - !0  : on error.
*/

int
graphGeomLoadChac (
Graph * restrict const      grafptr,              /* Graph to load    */
Geom * restrict const       geomptr,              /* Geometry to load */
FILE * const                filesrcptr,           /* Topological data */
FILE * const                filegeoptr,           /* No use           */
const char * const          dataptr)              /* No use           */
{
  long              chavertnbr;                   /* Number of vertices         */
  Gnum              chavertnum;                   /* Number of vertex read      */
  long              chaedgenbr;                   /* Number of edges            */
  int               chaflagnum;                   /* Flag on numeric form       */
  char              chaflagstr[4];                /* Flag for optional data     */
  char              chabuffcar;                   /* Buffer for line processing */
  Gnum              edgenum;
  Gnum              vertnum;
  Gnum              velosum;
  Gnum              vlblmax;
  Gnum              degrmax;

  do {                                            /* Skip comment lines   */
    chabuffcar = fgetc (filesrcptr);              /* Read first character */
    if (chabuffcar == '%') {                      /* If comment line      */
      fscanf (filesrcptr, "%*[^\n]");             /* Purge line           */
      fgetc  (filesrcptr);                        /* Purge newline        */
    }
  } while (chabuffcar == '%');
  ungetc (chabuffcar, filesrcptr);

  if (fscanf (filesrcptr, "%ld%ld%*[ \t]",        /* Read graph header */
              &chavertnbr,
              &chaedgenbr) < 2) {
    errorPrint ("graphGeomLoadChac: bad input (1)");
    return     (1);
  }
  chaflagstr[0] =                                 /* Pre-set flag array */
  chaflagstr[1] =
  chaflagstr[2] =
  chaflagstr[3] = '\0';
  fscanf (filesrcptr, "%3[0-9]%*[^\n]",           /* Read flag string */
          chaflagstr);
  fgetc (filesrcptr);                             /* Purge newline */
  chaflagnum    = atoi (chaflagstr);
  chaflagstr[0] = '0' + ((chaflagnum / 100) % 10); /* Set the flags */
  chaflagstr[1] = '0' + ((chaflagnum / 10)  % 10);
  chaflagstr[2] = '0' + ((chaflagnum)       % 10);

  grafptr->flagval = GRAPHFREETABS;
  grafptr->baseval = 1;                         /* Chaco graphs are based */
  grafptr->vertnbr = chavertnbr;
  grafptr->vertnnd = chavertnbr + 1;
  grafptr->edgenbr = chaedgenbr * 2;            /* We are counting arcs */
  if (((grafptr->verttax = (Gnum *) memAlloc (grafptr->vertnnd * sizeof (Gnum))) == NULL) ||
      ((grafptr->edgetax = (Gnum *) memAlloc (grafptr->edgenbr * sizeof (Gnum))) == NULL)) {
    errorPrint ("graphGeomLoadChac: out of memory (1)");
    if (grafptr->verttax != NULL)
      memFree (grafptr->verttax);
    return     (1);
  }
  grafptr->verttax -= grafptr->baseval;
  grafptr->vendtax  = grafptr->verttax + 1;

  if (chaflagstr[0] != '0') {
    if ((grafptr->vlbltax = (Gnum *) memAlloc (chavertnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("graphGeomLoadChac: out of memory (2)");
      memFree    (grafptr);
      return     (1);
    }
    grafptr->vlbltax -= grafptr->baseval;
  }

  velosum = grafptr->vertnbr;                     /* Assume no vertex loads */
  if (chaflagstr[1] != '0') {
    if ((grafptr->velotax = (Gnum *) memAlloc (chavertnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("graphGeomLoadChac: out of memory (3)");
      memFree    (grafptr);
      return     (1);
    }
    grafptr->velotax -= grafptr->baseval;
    velosum = 0;
  }

  if (chaflagstr[2] != '0') {
    if ((grafptr->edlotax = (Gnum *) memAlloc (grafptr->edgenbr * sizeof (Gnum))) == NULL) {
      errorPrint ("graphGeomLoadChac: out of memory (4)");
      memFree    (grafptr);
      return     (1);
    }
    grafptr->edlotax -= grafptr->baseval;
  }

  for (vertnum = edgenum = grafptr->baseval, degrmax = vlblmax = 0;
       vertnum < grafptr->vertnnd; vertnum ++) {
    do {                                          /* Skip comment lines   */
      chabuffcar = fgetc (filesrcptr);            /* Read first character */
      if (chabuffcar == '%') {                    /* If comment line      */
        fscanf (filesrcptr, "%*[^\n]");           /* Purge line           */
        fgetc  (filesrcptr);                      /* Purge newline        */
      }
    } while (chabuffcar == '%');
    ungetc (chabuffcar, filesrcptr);              /* Put character back to filesrcptr */

    if (grafptr->vlbltax != NULL) {
      if ((intLoad (filesrcptr, &grafptr->vlbltax[vertnum]) != 1) ||
          (grafptr->vlbltax[vertnum] < 1)                         ||
          (grafptr->vlbltax[vertnum] > chavertnbr)) {
        errorPrint ("graphGeomLoadChac: bad input (2)");
        graphFree  (grafptr);
        return     (1);
      }
      if (grafptr->vlbltax[vertnum] > vlblmax)
        vlblmax = grafptr->vlbltax[vertnum];
    }
    if (grafptr->velotax != NULL) {
      if ((intLoad (filesrcptr, &grafptr->velotax[vertnum]) != 1) ||
          (grafptr->velotax[vertnum] < 1)) {
        errorPrint ("graphGeomLoadChac: bad input (3)");
        graphFree  (grafptr);
        return     (1);
      }
      velosum += grafptr->velotax[vertnum];
    }
    grafptr->verttax[vertnum] = edgenum;          /* Set based edge array index */

    do {                                          /* Read graph edges             */
      fscanf (filesrcptr, "%*[ \t]");             /* Skip white spaces            */
      chabuffcar = fgetc (filesrcptr);            /* Read next char               */
      if (chabuffcar != '\n') {                   /* If line not complete         */
        ungetc (chabuffcar, filesrcptr);          /* Put character back to stream */

        if ((intLoad (filesrcptr, &chavertnum) != 1) ||
            (chavertnum < 1)                         ||
            (chavertnum > chavertnbr)                ||
            ((grafptr->edlotax != NULL) &&
             ((intLoad (filesrcptr, &grafptr->edlotax[edgenum]) != 1) ||
              (grafptr->edlotax[edgenum] < 1)))) {
          errorPrint ("graphGeomLoadChac: bad input (4)");
          graphFree  (grafptr);
          return     (1);
        }
        if (edgenum > (grafptr->edgenbr + grafptr-> baseval)) { /* Test edge array overflow */
          errorPrint ("graphGeomLoadChac: bad input (5)");
          graphFree  (grafptr);
          return     (1);
        }
        grafptr->edgetax[edgenum ++] = chavertnum;
      }
    } while (chabuffcar != '\n');                 /* Till line complete */

    if ((edgenum - grafptr->verttax[vertnum]) > degrmax)
      degrmax = edgenum - grafptr->verttax[vertnum];
  }
  grafptr->verttax[vertnum] = edgenum;            /* Set end of based vertex array */
  grafptr->velosum = velosum;
  grafptr->degrmax = degrmax;

  if (grafptr->vlbltax != NULL) {                 /* If graph has labels       */
    if (graphLoad2 (grafptr->baseval, grafptr->vertnnd, /* Un-label graph data */
                    grafptr->verttax, grafptr->vendtax,
                    grafptr->edgetax, vlblmax, grafptr->vlbltax) != 0) {
      errorPrint ("graphGeomLoadChac: cannot relabel graph");
      graphFree  (grafptr);
      return     (1);
    }
  }

#ifdef SCOTCH_DEBUG_GRAPH2
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphGeomLoadChac: inconsistent graph data");
    graphFree  (grafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_GRAPH2 */

  return (0);
}

/* This routine saves the geometrical graph
** in the Chaco graph format.
** It returns:
** - 0   : on succes
** - !0  : on error.
*/

int
graphGeomSaveChac (
const Graph * restrict const  grafptr,            /* Graph to save    */
const Geom * restrict const   geomptr,            /* Geometry to save */
FILE * const                  filesrcptr,         /* Topological data */
FILE * const                  filegeoptr,         /* No use           */
const char * const            dataptr)            /* No use           */
{
  Gnum              baseadj;                      /* Base adjustment  */
  Gnum              vertnum;                      /* Current vertex   */
  Gnum              edgenum;                      /* Current edge     */
  char *            sepaptr;                      /* Separator string */
  int               o;

  baseadj = 1 - grafptr->baseval;                 /* Output base is always 1 */

  o = (fprintf (filesrcptr, "%ld\t%ld\t%c%c%c\n", /* Write graph header */
                (long)  grafptr->vertnbr,
                (long) (grafptr->edgenbr / 2),
                ((grafptr->vlbltax != NULL) ? '1' : '0'),
                ((grafptr->velotax != NULL) ? '1' : '0'),
                ((grafptr->edlotax != NULL) ? '1' : '0')) == EOF);

  for (vertnum = grafptr->baseval; (o == 0) && (vertnum < grafptr->vertnnd); vertnum ++) {
    sepaptr = "";                                 /* Start lines as is */

    if (grafptr->vlbltax != NULL) {
      o |= (fprintf (filesrcptr, "%ld", (long) (grafptr->vlbltax[vertnum] + baseadj)) == EOF);
      sepaptr = "\t";
    }
    if (grafptr->velotax != NULL) {
      o |= (fprintf (filesrcptr, "%s%ld",
                     sepaptr,
                     (long) grafptr->velotax[vertnum]) == EOF);
      sepaptr = "\t";
    }

    for (edgenum = grafptr->verttax[vertnum];
         (o == 0) && (edgenum < grafptr->vendtax[vertnum]); edgenum ++) {
      if (grafptr->vlbltax != NULL)
        o |= (fprintf (filesrcptr, "%s%ld",
                       sepaptr,
                       (long) (grafptr->vlbltax[grafptr->edgetax[edgenum]] + baseadj)) == EOF);
      else
        o |= (fprintf (filesrcptr, "%s%ld",
                       sepaptr,
                       (long) (grafptr->edgetax[edgenum] + baseadj)) == EOF);

      if (grafptr->edlotax != NULL)
        o |= fprintf (filesrcptr, " %ld", (long) grafptr->edlotax[edgenum]);

      sepaptr = "\t";
    }
    o |= (fprintf (filesrcptr, "\n") == EOF);
  }
  if (o != 0)
    errorPrint ("graphGeomSaveChac: bad output");

  return (o);
}
