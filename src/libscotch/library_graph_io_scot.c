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
/**   NAME       : library_graph_io_scot.c                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the Scotch   **/
/**                geometry and graph handling routines of **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 10 oct 1999     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     19 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "geom.h"
#include "graph.h"
#include "scotch.h"

/*************************************/
/*                                   */
/* These routines are the C API for  */
/* the Scotch graph and geometry     */
/* handling routines.                */
/*                                   */
/*************************************/

/*+ This routine loads the given opaque graph
*** structure with the data of the given stream.
*** - 0   : if loading succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_graphGeomLoadScot (
SCOTCH_Graph * restrict const grafptr,
SCOTCH_Geom * restrict const  geomptr,
FILE * const                  filegrfptr,
FILE * const                  filegeoptr,
const char * const            dataptr)            /* No use */
{
  return (graphGeomLoadScot ((Graph *) grafptr, (Geom *) geomptr, filegrfptr, filegeoptr, NULL));
}

/*+ This routine saves the contents of the given
*** opaque graph structure to the given stream.
*** It returns:
*** - 0   : if the saving succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_graphGeomSaveScot (
const SCOTCH_Graph * restrict const grafptr,
const SCOTCH_Geom * restrict const  geomptr,
FILE * const                        filegrfptr,
FILE * const                        filegeoptr,
const char * const                  dataptr)      /* No use */
{
  return (graphGeomSaveScot ((Graph *) grafptr, (Geom *) geomptr, filegrfptr, filegeoptr, NULL));
}
