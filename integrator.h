#ifndef INTEGRATOR_H
#define INTEGRATOR_H

int velverlet_ndim_npart(const long double dt, const long double forceConst, const int nBodies, const int spatialDim, const long double *masses,
                         long double *coord, long double *vel, long double *force, long double **f_o, void (*F)(const long double *, const long double *, const long double, const int, long double *));

#endif
