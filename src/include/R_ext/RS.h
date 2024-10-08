/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 1999-2024 The R Core Team.
 *
 *  This header file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 * 
 *  This file is part of R. R is distributed under the terms of the
 *  GNU General Public License, either Version 2, June 1991 or Version 3,
 *  June 2007. See doc/COPYRIGHTS for details of the copyright status of R.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, a copy is available at
 *  https://www.R-project.org/Licenses/
 */

/* Included by R.h: nowadays almost all API */

#ifndef R_RS_H
#define R_RS_H

#if defined(__cplusplus) && !defined(DO_NOT_USE_CXX_HEADERS)
# include <cstring>
# include <cstddef>
# define R_SIZE_T std::size_t
#else
# include <string.h>		/* for memcpy, memset */
# include <stddef.h> /* for size_t */
# define R_SIZE_T size_t
#endif

#include <Rconfig.h>		/* for HAVE_F77_UNDERSCORE */

#ifdef  __cplusplus
extern "C" {
#endif

/* S Like Memory Management */

/* not of themselves API */
extern void *R_chk_calloc(R_SIZE_T, R_SIZE_T);
extern void *R_chk_realloc(void *, R_SIZE_T);
extern void R_chk_free(void *);

#ifndef STRICT_R_HEADERS
/* S-PLUS 3.x but not 5.x NULLed the pointer in Free.
   Not API.
*/
#define Calloc(n, t)   (t *) R_chk_calloc( (R_SIZE_T) (n), sizeof(t) )
#define Realloc(p,n,t) (t *) R_chk_realloc( (void *)(p), (R_SIZE_T)((n) * sizeof(t)) )
#define Free(p)        (R_chk_free( (void *)(p) ), (p) = NULL)
#endif

/* API */
#define R_Calloc(n, t)   (t *) R_chk_calloc( (R_SIZE_T) (n), sizeof(t) )
#define R_Realloc(p,n,t) (t *) R_chk_realloc( (void *)(p), (R_SIZE_T)((n) * sizeof(t)) )
#define R_Free(p)      (R_chk_free( (void *)(p) ), (p) = NULL)

/* Nowadays API: undocumented until 4.1.2: widely used. */
#define Memcpy(p,q,n)  memcpy( p, q, (R_SIZE_T)(n) * sizeof(*p) )

/* Nowadays API: added for 3.0.0 but undocumented until 4.1.2. */
#define Memzero(p,n)  memset(p, 0, (R_SIZE_T)(n) * sizeof(*p))

/* API: Added in R 2.6.0 */
#define CallocCharBuf(n) (char *) R_chk_calloc(((R_SIZE_T)(n))+1, sizeof(char))

/* S Like Fortran Interface */
/* These may not be adequate everywhere. Convex had _ prepending common
   blocks, and some compilers may need to specify Fortran linkage */

#ifdef HAVE_F77_UNDERSCORE
# define F77_CALL(x)	x ## _
#else
# define F77_CALL(x)	x
#endif
#define F77_NAME(x)    F77_CALL(x)
#define F77_SUB(x)     F77_CALL(x)
#define F77_COM(x)     F77_CALL(x)
#define F77_COMDECL(x) F77_CALL(x)

/* call_R was deprecated in R 2.15.0, removed in R 4.2.0 */

#ifdef  __cplusplus
}
#endif

#endif /* R_RS_H */
