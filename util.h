#ifndef _util_h
#define _util_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <acml.h>
#include <time.h>

#include "dlist.h"

#ifdef ALSON
#include "als.h"
#endif

#ifdef SGDON
#include "sgd.h"
#endif

MPI_Datatype MPI_USER;
MPI_Datatype MPI_ITEM;



int readInputFile(FILE* ifp, DLIST* users, DLIST* movies, int d, const char* rule);
void initMPIStructs();
double dotProduct (double *x, double *y, int n);
int contains(int vector[], int n, int query);
double dotprod(double *p,double *q, int n);
#endif
