#ifndef _sgd_h
#define _sgd_h
void stochasticGD(int k, int d,double gamma, double lambda, DLIST* users, DLIST* movies);
double dotprod(double *p, double* q, int n);
int contains(int vector[], int n, int query);
#endif
