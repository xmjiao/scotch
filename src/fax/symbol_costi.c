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
/**   NAME       : symbol_costi.c                          **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module computes the block solving  **/
/**                cost of incomplete symbolic matrices,   **/
/**                using the cost functions of "Fonctions  **/
/**                de Complexite pour la Resolution par    **/
/**                Blocs de Systemes Lineaires Denses et   **/
/**                Creux", by Pierre Ramet.                **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 24 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_COSTI

#include "common.h"
#include "dof.h"
#include "symbol.h"
#include "symbol_costi.h"

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine computes the factorization
*** and solving cost of the given symbolic
*** block matrix, whose nodes hold the number
*** of DOFs given by the proper DOF structure.
*** To ensure maximum accuracy and minimum loss
*** of precision, costs are summed-up recursively.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolCosti (
const SymbolMatrix * const  symbptr,              /*+ Symbolic matrix to evaluate              +*/
const Dof * const           deofptr,              /*+ DOF structure associated with the matrix +*/
const SymbolCostType        typeval,              /*+ Type of cost computation                 +*/
const INT                   levfval,              /*+ Level of fill                            +*/
double * const              nnzptr,               /*+ Size of the structure, to be filled      +*/
double * const              opcptr)               /*+ Operation count, to be filled            +*/
{
  if (typeval != SYMBOLCOSTLDLT) {
    errorPrint ("symbolCosti: cost function not supported");
    return     (1);
  }

  *opcptr = 0.0L;
  *nnzptr = 0.0L;

  symbolCosti2 (symbptr->cblktab - symbptr->baseval, /* Perform recursion on column blocks */
                symbptr->bloktab - symbptr->baseval,
                deofptr, levfval, nnzptr, opcptr, symbptr->baseval, symbptr->cblknbr);

  return (0);
}

static
void
symbolCosti2 (
const SymbolCblk * restrict const cblktax,        /*+ Based access to cblktab                  +*/
const SymbolBlok * restrict const bloktax,        /*+ Based access to bloktab                  +*/
const Dof * restrict const        deofptr,        /*+ DOF structure associated with the matrix +*/
const INT                         levfval,        /*+ Level of fill                            +*/
double * restrict const           nnzptr,         /*+ Size of the structure, to be filled      +*/
double * restrict const           opcptr,         /*+ Operation count, to be filled            +*/
const INT                         cblkmin,        /*+ Minimum column block index to consider   +*/
const INT                         cblknbr)        /*+ Number of column blocks to consider      +*/
{
  INT                 bloknum;                    /* Number of current extra-diagonal block             */
  INT                 cmednum;                    /* Median column block number                         */
  INT                 cfacnum;                    /* Number of facing column block                      */
  INT                 cdofnbr;                    /* Number of DOFs in column block (l_k)               */
  INT                 rdofsum;                    /* Number of DOFs in all row blocks (g_{ki} or g_{k}) */
  double              nnzval;                     /* Number of non-zeroes in subtree                    */
  double              opcval;                     /* Operation count in subtree                         */

  nnzval =                                        /* Initialize local values */
  opcval = 0.0L;

  if (cblknbr > 1) {                              /* If more than one column block, perform recursion */
    cmednum = cblknbr / 2;
    symbolCosti2 (cblktax, bloktax, deofptr, levfval, &nnzval, &opcval, cblkmin, cmednum);
    symbolCosti2 (cblktax, bloktax, deofptr, levfval, &nnzval, &opcval, cblkmin + cmednum, cblknbr - cmednum);

    *nnzptr += nnzval;                            /* Sum-up local values */
    *opcptr += opcval;
  }
  else {                                          /* Single column block                              */
    INT                 levffac;                  /* Minimum level of fill over facing block(s)       */
    INT                 rdounbr;                  /* Number of DOFs in undropped row blocks (h'_{ki}) */
    INT                 rdousum;                  /* Number of DOFs in undropped row blocks (h'_{ki}) */

    cdofnbr = noddVal (deofptr, cblktax[cblkmin].lcolnum + 1) -
              noddVal (deofptr, cblktax[cblkmin].fcolnum);

    bloknum = cblktax[cblkmin].bloknum + 1;       /* Get index of first extra-diagonal block */

    if (bloknum == cblktax[cblkmin + 1].bloknum)  /* If diagonal block only  */
      levffac = levfval;                          /* No contributions at all */
    else {
      levffac = bloktax[bloknum].levfval;         /* Get level of fill of first extra-diagonal block                */
      for (bloknum ++; (bloknum < cblktax[cblkmin + 1].bloknum) && /* For all first blocks facing same column block */
           (bloktax[bloknum].cblknum == bloktax[bloknum - 1].cblknum); bloknum ++) {
        if (bloktax[bloknum].levfval < levffac)   /* If facing block has smaller level of fill */
          levffac = bloktax[bloknum].levfval;     /* Keep smallest level of fill               */
      }
    }

    rdofsum =
    rdousum =
    rdounbr = 0;

    for (bloknum = cblktax[cblkmin + 1].bloknum - 1; /* Scan extra-diagonals, backwards */
         bloknum > cblktax[cblkmin].bloknum; ) {
      if (bloktax[bloknum].levfval > levfval) {   /* Skip dropped blocks */
        bloknum --;
        continue;
      }

      rdousum += rdounbr;
      rdounbr  = 0;

      cfacnum = bloktax[bloknum].cblknum;
      do {
        INT                 rdofblk;              /* Number of DOFs in local block */

        if (bloktax[bloknum].levfval > levfval)   /* Skip dropped blocks */
          continue;

        rdofblk  = noddVal (deofptr, bloktax[bloknum].lrownum + 1) -
                   noddVal (deofptr, bloktax[bloknum].frownum);
        rdofsum += rdofblk;                       /* Account for undropped blocks */

        if (MAX (bloktax[bloknum].levfval, levffac) < levfval) /* If contribution is not dropped */
          rdounbr += rdofblk;                     /* Add contribution to undropped set of blocks */
      } while (bloktax[-- bloknum].cblknum == cfacnum);

      opcval += ((double) (rdounbr)) *            /* Count C3'(k,i) + C3''(k,i) */
                ((double) (rdounbr + rdousum)) *
                ((double) (2 * cdofnbr + 1));
    }

    *nnzptr += ((double) (cdofnbr + rdofsum)) * ((double) cdofnbr); /* Sum-up stored coefficients */
    *opcptr += opcval +
               ((double) cdofnbr) *               /* Count C1(k) + C2(k) */
               (((double) cdofnbr) * ((double) (2 * cdofnbr + 6 * rdofsum + 3)) + 1.0L) / 6.0L;
  }
}
