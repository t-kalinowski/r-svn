/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 2001   The R Development Core Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef R_MODREG_H
#define R_MODREG_H

#include <R.h>
/* for Sint .. */

void BDRksmooth(double *x, double *y, int *n,
		double *xp, double *yp, int *np,
		int *kern, double *bandwidth);

void
loess_raw(double *y, double *x, double *weights, double *robust, Sint *d,
	  Sint *n, double *span, Sint *degree, Sint *nonparametric,
	  Sint *drop_square, Sint *sum_drop_sqr, double *cell,
	  char **surf_stat, double *surface, Sint *parameter,
	  Sint *a, double *xi, double *vert, double *vval, double *diagonal,
	  double *trL, double *one_delta, double *two_delta, Sint *setLf);
void
loess_dfit(double *y, double *x, double *x_evaluate, double *weights,
	   double *span, Sint *degree, Sint *nonparametric,
	   Sint *drop_square, Sint *sum_drop_sqr,
	   Sint *d, Sint *n, Sint *m, double *fit);
void
loess_dfitse(double *y, double *x, double *x_evaluate, double *weights,
	     double *robust, Sint *family, double *span, Sint *degree,
	     Sint *nonparametric, Sint *drop_square,
	     Sint *sum_drop_sqr,
	     Sint *d, Sint *n, Sint *m, double *fit, double *L);
void
loess_ifit(Sint *parameter, Sint *a, double *xi, double *vert,
	   double *vval, Sint *m, double *x_evaluate, double *fit);
void
loess_ise(double *y, double *x, double *x_evaluate, double *weights,
	  double *span, Sint *degree, Sint *nonparametric,
	  Sint *drop_square, Sint *sum_drop_sqr, double *cell,
	  Sint *d, Sint *n, Sint *m, double *fit, double *L);


void F77_SUB(lowesw)(double *res, int *n, double *rw, int *pi);
void F77_SUB(lowesp)(int *n, double *y, double *yhat, double *pwgts,
		     double *rwgts, int *pi, double *ytilde);
void F77_SUB(bdrsetppr)(double *span1, double *alpha1,
	int *optlevel, int *ism, double *df1, double *gcvpen1);
void F77_SUB(bdrsmart)(int *m, int *mu, int *p, int * q, int *n,
		       double *w, double *x, double *y,
		       double *ww, double *smod, int *nsmod, double *sp,
		       int *nsp, double *dp, int *ndp, double *edf);
void F77_SUB(bdrpred)(int *np, double *x, double *smod,
		      double *y, double *sc);
void F77_SUB(qsbart)(double *penalt, double *dofoff,
		     double *xs, double *ys, double *ws, double *ssw,
		     int *n, double *knot, int *nk, double *coef,
		     double *sz, double *lev, double *crit, int *iparms,
		     double *spar, double *parms, int *isetup,
		     double *scrtch, int *ld4, int *ldnk, int *ier);

void F77_NAME(sbart)
    (double *penalt, double *dofoff,
     double *xs, double *ys, double *ws, double *ssw,
     long *n, double *knot, long *nk, double *coef,
     double *sz, double *lev, double *crit, long *icrit,
     double *spar, long *ispar, long *iter, double *lspar,
     double *uspar, double *tol, double *eps, long *isetup,
     double *xwy, double *hs0, double *hs1, double *hs2,
     double *hs3, double *sg0, double *sg1, double *sg2,
     double *sg3, double *abd, double *p1ip, double *p2ip,
     long *ld4, long *ldnk, long *ier);

void F77_NAME(sgram)(double *sg0, double *sg1, double *sg2, double *sg3,
		     double *tb, long *nb);
void F77_NAME(stxwx)(double *x, double *z, double *w,
		     long *k, double *xknot, long *n, double *y,
		     double *hs0, double *hs1, double *hs2, double *hs3);
void F77_NAME(sslvrg)(double *penalt, double *dofoff,
		      double *x, double *y, double *w, double *ssw, long *n,
		      double *knot, long *nk, double *coef, double *sz,
		      double *lev, double *crit, long *icrit, double *lambda,
		      double *xwy,
		      double *hs0, double *hs1, double *hs2, double *hs3,
		      double *sg0, double *sg1, double *sg2, double *sg3,
		      double *abd, double *p1ip, double *p2ip,
		      long *ld4, long *ldnk, long *info);

void F77_SUB(bvalus)(int *n, double *knot, double *coef,
		     int *nk, double *x, double *s, int *order);
void F77_SUB(bdrsupsmu)(int *n, double *x, double *y,
			double *w, int *iper, double *span, double *alpha,
			double *smo, double *sc, double *edf);
#endif
