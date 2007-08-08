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
/**   NAME       : kgraph_map_rb.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the Dual Recursive Bipartitioning   **/
/**                mapping algorithm.                      **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 23 mar 1993     **/
/**                                 to     12 may 1993     **/
/**                # Version 1.3  : from : 06 apr 1994     **/
/**                                 to     09 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     04 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     30 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 15 nov 1995     **/
/**                                 to     15 nov 1995     **/
/**                # Version 3.2  : from : 01 oct 1996     **/
/**                                 to     10 jun 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     17 may 1999     **/
/**                # Version 3.4  : from : 12 sep 2001     **/
/**                                 to     06 nov 2001     **/
/**                # Version 4.0  : from : 29 nov 2003     **/
/**                                 to     05 may 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Job selection policy types. +*/

typedef enum KgraphMapRbPolicy_ {
  KGRAPHMAPRBPOLIRANDOM = 0,                      /*+ Random job selection policy                       +*/
  KGRAPHMAPRBPOLILEVEL,                           /*+ Select job with highest level                     +*/
  KGRAPHMAPRBPOLISIZE,                            /*+ Select job with largest size                      +*/
  KGRAPHMAPRBPOLINEIGHBOR,                        /*+ Priority level computed with respect to neighbors +*/
  KGRAPHMAPRBPOLINGLEVEL,                         /*+ Select job with most neighbors of higher level    +*/
  KGRAPHMAPRBPOLINGSIZE,                          /*+ Select job with most neighbors of smaller size    +*/
  KGRAPHMAPRBPOLIOLD                              /*+ Select job in old style (version 2.x)             +*/
} KgraphMapRbPolicy;

/*+ Method parameters. +*/

typedef struct KgraphMapRbParam_ {
  int                       flagjobtie;           /*+ Flag set of job pools are tied +*/
  int                       flagmaptie;           /*+ Flag set if mappings are tied  +*/
  KgraphMapRbPolicy         poli;                 /*+ Job selection policy           +*/
  Strat *                   strat;                /*+ Bipartitioning strategy used   +*/
} KgraphMapRbParam;

/*+ Job pool structures. +*/

typedef struct KgraphMapRbPoolLink_ {
  struct KgraphMapRbPoolLink_ * prev;             /*+ Pointer to previous link +*/
  struct KgraphMapRbPoolLink_ * next;             /*+ Pointer to next link     +*/
} KgraphMapRbPoolLink;

typedef struct KgraphMapRbPool_ {
  KgraphMapRbPoolLink       poollink;             /*+ List of jobs in pool; TRICK: FIRST +*/
  KgraphMapRbPolicy         polival;              /*+ Job selection policy               +*/
} KgraphMapRbPool;

/*+ This structure defines a job to be
    performed with respect to a partial
    mapping of a source graph.          +*/

typedef struct KgraphMapRbJob_ {
  KgraphMapRbPoolLink       poollink;             /*+ Link to job pool ; TRICK : FIRST         +*/
  KgraphMapRbPool *         poolptr;              /*+ Pointer to last/current job pool         +*/
  int                       poolflag;             /*+ Flag set if job in pool                  +*/
  Gnum                      prioval;              /*+ Job priority value by policy             +*/
  Gnum                      priolvl;              /*+ Priority level computed for this job     +*/
  ArchDom                   domorg;               /*+ Domain to which the vertices belong      +*/
  ArchDom                   domsub[2];            /*+ Subdomains of the domain                 +*/
  Graph                     grafdat;              /*+ Job graph data (may be clone of another) +*/
} KgraphMapRbJob;

/*
**  The function prototypes.
*/

#ifndef KGRAPH_MAP_RB
#define static
#endif

static void                 kgraphMapRbPoolInit (KgraphMapRbPool * restrict const, const KgraphMapRbPolicy);
static void                 kgraphMapRbPoolExit (KgraphMapRbPool * restrict const);
static void                 kgraphMapRbPoolAdd  (KgraphMapRbPool * const, KgraphMapRbJob * const);
static void                 kgraphMapRbPoolDel  (KgraphMapRbPool * const, KgraphMapRbJob * const);
static KgraphMapRbJob *     kgraphMapRbPoolGet  (KgraphMapRbPool * const);
static void                 kgraphMapRbPoolNew  (KgraphMapRbPool * const, KgraphMapRbJob * const);
static void                 kgraphMapRbPoolUpdt (KgraphMapRbPool * const, const Graph * restrict const, const Mapping * const, KgraphMapRbJob * const, KgraphMapRbJob * const, KgraphMapRbJob * const);
static void                 kgraphMapRbPoolRemv (KgraphMapRbPool * const, const Graph * restrict const, Mapping * const, KgraphMapRbJob * const, GraphPart * const, GraphPart, KgraphMapRbJob * const);

int                         kgraphMapRb         (Kgraph * const, const KgraphMapRbParam * const);

static void                 kgraphMapRbExit     (const Anum, KgraphMapRbJob * const, KgraphMapRbPool ** const, Mapping ** const, Kgraph * const);
static int                  kgraphMapRbResize   (Anum * const, KgraphMapRbJob ** const, KgraphMapRbPool ** const, Mapping * restrict * const);

#undef static

/*
**  The macro definitions.
*/

#define kgraphMapRbPoolEmpty(poolptr) ((poolptr)->poollink.next == &kgraphmaprbpooldummy)
