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
/**   NAME       : gord.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a sparse matrix graph orderer.  **/
/**                This module contains the data declara-  **/
/**                tions for the main routine.             **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to   : 20 oct 1997     **/
/**                # Version 3.3  : from : 07 oct 1998     **/
/**                                 to   : 31 may 1999     **/
/**                # Version 4.0  : from : 11 dec 2002     **/
/**                                 to   : 27 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   5             /* Number of files in list                */
#define C_FILEARGNBR                3             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph input file name */
#define C_filenameordout            C_fileTab[1].name /* Ordering output file name    */
#define C_filenamelogout            C_fileTab[2].name /* Log file name                */
#define C_filenamemapout            C_fileTab[3].name /* Separator mapping file name  */
#define C_filenametreout            C_fileTab[4].name /* Separator tree file name     */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file */
#define C_filepntrordout            C_fileTab[1].pntr /* Ordering output file    */
#define C_filepntrlogout            C_fileTab[2].pntr /* Log file                */
#define C_filepntrmapout            C_fileTab[3].pntr /* Separator mapping file  */
#define C_filepntrtreout            C_fileTab[4].pntr /* Separator tre file      */

/*+ Process flags. +*/

#define C_FLAGNONE                  0x0000        /* No flags                   */
#define C_FLAGMAPOUT                0x0001        /* Output mapping data        */
#define C_FLAGTREOUT                0x0002        /* Output separator tree data */
#define C_FLAGVERBSTR               0x0004        /* Output strategy string     */
#define C_FLAGVERBTIM               0x0008        /* Output timing information  */
