int velverlet_ndim_npart(const double dt, const double forceConst, const int nBodies, const int spatialDim, const double *masses,
double *coord, double *vel, double *force, double **f_o, void (*F)(const double *, const double *, const double, const int, double *));
