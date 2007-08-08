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
/**   NAME       : common_integer.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This module handles the generic integer **/
/**                type.                                   **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 07 sep 1998     **/
/**                                 to     22 sep 1998     **/
/**                # Version 0.1  : from : 07 jan 2002     **/
/**                                 to     17 jan 2003     **/
/**                # Version 1.0  : from : 23 aug 2005     **/
/**                                 to   : 19 dec 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_INTEGER

#include "common.h"
#include "common_integer.h"

/********************************/
/*                              */
/* Basic routines for fast I/O. */
/*                              */
/********************************/

/* Fast read for INT values.
** It returns:
** - 1  : on success.
** - 0  : on error.
*/

int
intLoad (
FILE * const                stream,               /*+ Stream to read from     +*/
INT * const                 valptr)               /*+ Area where to put value +*/
{
  int                 sign;                       /* Sign flag      */
  int                 car;                        /* Character read */
  INT                 val;                        /* Value          */

  sign = 0;                                       /* Assume positive constant     */
  for ( ; ; ) {                                   /* Consume whitespaces and sign */
    car = getc (stream);
    if (isspace (car))
      continue;
    if ((car >= '0') && (car <= '9'))
      break;
    if (car == '-') {
      sign = 1;
      car  = getc (stream);
      break;
    }
    if (car == '+') {
      car = getc (stream);
      break;
    }
    return (0);
  }
  if ((car < '0') || (car > '9'))                 /* If first char is non numeric */
    return (0);                                   /* Then it is an error          */
  val = car - '0';                                /* Get first digit              */
  for ( ; ; ) {
    car = getc (stream);
    if ((car < '0') || (car > '9')) {
      ungetc (car, stream);
      break;
    }
    val = val * 10 + (car - '0');                 /* Accumulate digits */
  }
  *valptr = (sign != 0) ? (- val) : val;          /* Set result */

  return (1);
}

/* Write routine for INT values.
** It returns:
** - 1  : on success.
** - 0  : on error.
*/

int
intSave (
FILE * const                stream,               /*+ Stream to write to +*/
const INT                   val)                  /*+ Value to write     +*/
{
  return ((fprintf (stream, "%ld", (long) val) == EOF) ? 0 : 1);
}

/**********************************/
/*                                */
/* Permutation building routines. */
/*                                */
/**********************************/

/* This routine fills an array with
** consecutive INT values, in
** ascending order.
** It returns:
** - VOID  : in all cases.
*/

void
intAscn (
INT * restrict const        permtab,              /*+ Permutation array to build +*/
const INT                   permnbr,              /*+ Number of entries in array +*/
const INT                   baseval)              /*+ Base value                 +*/
{
  INT * restrict      permtax;
  INT                 permnum;
  INT                 permnnd;

  for (permnum = baseval, permnnd = baseval + permnbr, permtax = permtab - baseval;
       permnum < permnnd; permnum ++)
    permtax[permnum] = permnum;
}

/* This routine computes a random permutation
** of an array of INT values.
** It returns:
** - VOID  : in all cases.
*/

void
intPerm (
INT * restrict const        permtab,              /*+ Permutation array to build +*/
const INT                   permnbr)              /*+ Number of entries in array +*/
{
  INT * restrict      permptr;
  INT                 permrmn;

  for (permptr = permtab, permrmn = permnbr;      /* Perform random permutation */
       permrmn > 0; permptr ++, permrmn --) {
    INT                 permnum;
    INT                 permtmp;

    permnum          = intRandVal (permrmn);      /* Select index to swap       */
    permtmp          = permptr[0];                /* Swap it with current index */
    permptr[0]       = permptr[permnum];
    permptr[permnum] = permtmp;
  }
}

/********************/
/*                  */
/* Random routines. */
/*                  */
/********************/

/* This routine initializes the pseudo-random
** generator if necessary. In order for multi-sequential
** programs to have exactly the same behavior on any
** process, the random seed does not depend on the
** process rank.
** It returns:
** - VOID  : in all cases.
*/

void
intRandInit (void)
{
  static int          randflag = 0;               /*+ Flag set if generator already initialized +*/

  if (randflag == 0) {                            /* If generator not yet initialized */
#if ((defined COMMON_DEBUG) || (defined COMMON_RANDOM_FIXED_SEED))
    srandom (1);                                  /* Non-random seed */
#else
    srandom (time (NULL));                        /* Random seed */
#endif /* ((defined COMMON_DEBUG) || (defined COMMON_RANDOM_FIXED_SEED)) */
    randflag = 1;                                 /* Generator has been initialized */
  }
}

/*********************/
/*                   */
/* Sorting routines. */
/*                   */
/*********************/

/* This routine sorts an array of
** INT values by ascending order
** on their first value, used as key.
** It returns:
** - VOID  : in all cases.
*/

#define INTSORTNAME                 intSort1asc1
#define INTSORTSIZE                 (sizeof (INT))
#define INTSORTSWAP(p,q)            do { INT t; t = *((INT *) (p)); *((INT *) (p)) = *((INT *) (q)); *((INT *) (q)) = t; } while (0)
#define INTSORTCMP(p,q)             (*((INT *) (p)) < *((INT *) (q)))
#include "common_sort.c"
#undef INTSORTNAME
#undef INTSORTSIZE
#undef INTSORTSWAP
#undef INTSORTCMP

/* This routine sorts an array of pairs of
** INT values by ascending order on their
** first value, used as key.
** It returns:
** - VOID  : in all cases.
*/

#define INTSORTNAME                 intSort2asc1
#define INTSORTSIZE                 (2 * sizeof (INT))
#define INTSORTSWAP(p,q)            do { INT t, u; t = *((INT *) (p)); u = *((INT *) (p) + 1); *((INT *) (p)) = *((INT *) (q)); *((INT *) (p) + 1) = *((INT *) (q) + 1); *((INT *) (q)) = t; *((INT *) (q) + 1) = u; } while (0)
#define INTSORTCMP(p,q)             (*((INT *) (p)) < *((INT *) (q)))
#include "common_sort.c"
#undef INTSORTNAME
#undef INTSORTSIZE
#undef INTSORTSWAP
#undef INTSORTCMP

/* This routine sorts an array of pairs of
** INT values by ascending order on both
** of their values, used as primary and
** secondary keys.
** It returns:
** - VOID  : in all cases.
*/

#define INTSORTNAME                 intSort2asc2
#define INTSORTSIZE                 (2 * sizeof (INT))
#define INTSORTSWAP(p,q)            do { INT t, u; t = *((INT *) (p)); u = *((INT *) (p) + 1); *((INT *) (p)) = *((INT *) (q)); *((INT *) (p) + 1) = *((INT *) (q) + 1); *((INT *) (q)) = t; *((INT *) (q) + 1) = u; } while (0)
#define INTSORTCMP(p,q)             ((*((INT *) (p)) < *((INT *) (q))) || ((*((INT *) (p)) == *((INT *) (q))) && (*((INT *) (p) + 1) < *((INT *) (q) + 1))))
#include "common_sort.c"
#undef INTSORTNAME
#undef INTSORTSIZE
#undef INTSORTSWAP
#undef INTSORTCMP

/* Dichotomy routine
** It returns:
** - VOID  : in all cases.
*/

static void
intSearchDicho_2 (const INT * const tab,
		  INT * start, INT * stop, INT element)
{
  INT median;

  if ((tab [*start] > element) || (tab [*stop] < element)) {
    *stop = *start = -1;
    return;
  }

  if (*stop == *start + 1) { /* element is in *start */
    *stop = *start;
    return;
  }

  median = (*start + *stop)/2;
  if (tab [median] < element)
    *start = median;
  else if (tab [median] > element)
    *stop = median;
  else
    *stop = *start = median;
}

/**
 @brief
 This function search the owner of an element in a tab which
 contains sorted intervals.

 @param tab     The table of intervals.
 @param first   Indice of the first element (must be >= 0)
 @param last    Indice of the last element (corresponding to upper bound)
 @param element Element which is being searched in tab.

 @return
 - < 0 if an error occurs (element < tab[first] or element > tab[last])
 - the indice of the owner of element else.
*/
INT
intSearchDicho      (const INT * const tab,
		     const INT first, const INT last,
		     const INT element)
{
  INT start, stop;

  start = first;
  stop  = last;

  while ((stop >= 0) && (start != stop)) {        /* Loop to avoid recursion */
    intSearchDicho_2 (tab, &start, &stop, element);
  }

  while ((tab[stop] == element) && (stop < last)
         && (tab[stop] == tab[stop + 1]))         /* Avoid return empty interval */
    stop++;                                       /* this can occurs when processors set are empty */

  return (stop);
}
