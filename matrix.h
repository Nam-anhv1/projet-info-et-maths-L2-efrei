#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "fonction.h"
#include "tarjan.h"

// Structure pour une matrice carrée n x n
typedef struct {
    int rows;
    float **data;
} t_matrix;
t_matrix createEmptyMatrix(int n);
t_matrix createMatrixFromGraph(liste_adjacence G);
void copyMatrix(t_matrix dest, t_matrix src);
void freeMatrix(t_matrix *mat);
void afficherMatrix(t_matrix mat);

//Opérations matricielles
void multiplyMatrices(t_matrix m1, t_matrix m2, t_matrix result);
float diffMatrices(t_matrix m1, t_matrix m2);

//Puissances et convergence
t_matrix powerMatrix(t_matrix M, int n);
int findConvergence(t_matrix M, float epsilon, t_matrix *result);

//Sous-matrices pour les classes
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index);

//Distributions
void afficherDistribution(float *dist, int size, const char *label);
void calculerDistribution(float *pi_0, t_matrix M, int n, float *pi_n);

//Périodicité
int gcd(int *vals, int nbvals);
int getPeriod(t_matrix sub_matrix);

#endif