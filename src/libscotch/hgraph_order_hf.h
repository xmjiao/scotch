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
/**   NAME       : hgraph_order_hf.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the block-oriented Halo       **/
/**                Approximate (Multiple) Minimum Fill     **/
/**                graph ordering routine.                 **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 15 may 2001     **/
/**                                 to   : 15 may 2001     **/
/**                # Version 4.0  : from : 10 jan 2003     **/
/**                                 to   : 24 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

#define HGRAPHORDERHFCOMPRAT        1.2L          /*+ Compression ratio +*/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderHfParam_ {
  long                      colmin;               /*+ Minimum number of columns +*/
  long                      colmax;               /*+ Maximum number of columns +*/
  double                    fillrat;              /*+ Fill-in ratio             +*/
} HgraphOrderHfParam;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_HF
#define static
#endif

int                         hgraphOrderHf       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderHfParam * const);

#undef static
