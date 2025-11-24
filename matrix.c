#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

//Création et gestion de base
t_matrix createEmptyMatrix(int n) {
    t_matrix mat;
    mat.rows = n;
    mat.data = (float **)malloc(n * sizeof(float *));
    if (!mat.data) {
        perror("Erreur allocation matrice");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < n; i++) {
        mat.data[i] = (float *)calloc(n, sizeof(float));
        if (!mat.data[i]) {
            perror("Erreur allocation ligne matrice");
            exit(EXIT_FAILURE);
        }
    }
    
    return mat;
}

t_matrix createMatrixFromGraph(liste_adjacence G) {
    int n = G.taille;
    t_matrix mat = createEmptyMatrix(n);
    
    // Parcourir chaque sommet
    for (int i = 0; i < n; i++) {
        cellule *courant = G.tab[i].head;
        
        while (courant != NULL) {
            int j = courant->sommet_arrivee - 1;
            mat.data[i][j] = courant->proba;
            courant = courant->suiv;
        }
    }
    
    return mat;
}

void copyMatrix(t_matrix dest, t_matrix src) {
    if (dest.rows != src.rows) {
        fprintf(stderr, "Erreur: matrices de tailles différentes\n");
        return;
    }
    
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.rows; j++) {
            dest.data[i][j] = src.data[i][j];
        }
    }
}

void freeMatrix(t_matrix *mat) {
    if (!mat || !mat->data) return;
    
    for (int i = 0; i < mat->rows; i++) {
        free(mat->data[i]);
    }
    free(mat->data);
    mat->data = NULL;
    mat->rows = 0;
}

void afficherMatrix(t_matrix mat) {
    printf("Matrice %dx%d:\n", mat.rows, mat.rows);
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.rows; j++) {
            printf("%.4f ", mat.data[i][j]);
        }
        printf("\n");
    }
}

//Opérations matricielles

void multiplyMatrices(t_matrix m1, t_matrix m2, t_matrix result) {
    if (m1.rows != m2.rows || m1.rows != result.rows) {
        fprintf(stderr, "Erreur: tailles incompatibles pour multiplication\n");
        return;
    }
    
    int n = m1.rows;
    
    // Initialiser result à 0
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result.data[i][j] = 0.0f;
        }
    }
    
    // Multiplication matricielles
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result.data[i][j] += m1.data[i][k] * m2.data[k][j];
            }
        }
    }
}

float diffMatrices(t_matrix m1, t_matrix m2) {
    if (m1.rows != m2.rows) {
        fprintf(stderr, "Erreur: matrices de tailles différentes\n");
        return -1.0f;
    }
    
    float diff = 0.0f;
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m1.rows; j++) {
            diff += fabsf(m1.data[i][j] - m2.data[i][j]);
        }
    }
    
    return diff;
}

//Puissance et convergence

t_matrix powerMatrix(t_matrix M, int n) {
    if (n < 0) {
        fprintf(stderr, "Erreur: puissance négative\n");
        exit(EXIT_FAILURE);
    }
    
    if (n == 0) {
        // Matrice identité
        t_matrix identity = createEmptyMatrix(M.rows);
        for (int i = 0; i < M.rows; i++) {
            identity.data[i][i] = 1.0f;
        }
        return identity;
    }
    
    if (n == 1) {
        t_matrix result = createEmptyMatrix(M.rows);
        copyMatrix(result, M);
        return result;
    }

    t_matrix result = createEmptyMatrix(M.rows);
    t_matrix temp = createEmptyMatrix(M.rows);
    
    copyMatrix(result, M);
    
    for (int i = 1; i < n; i++) {
        multiplyMatrices(result, M, temp);
        copyMatrix(result, temp);
    }
    
    freeMatrix(&temp);
    return result;
}

int findConvergence(t_matrix M, float epsilon, t_matrix *result) {
    int n = 1;
    t_matrix M_n = createEmptyMatrix(M.rows);
    t_matrix M_n_minus_1 = createEmptyMatrix(M.rows);

    copyMatrix(M_n, M);
    
    while (n < 1000) {
        copyMatrix(M_n_minus_1, M_n);
        
        // Calculer M^n = M^(n-1) * M
        t_matrix temp = createEmptyMatrix(M.rows);
        multiplyMatrices(M_n, M, temp);
        copyMatrix(M_n, temp);
        freeMatrix(&temp);
        
        // Vérifier la convergence
        float diff = diffMatrices(M_n, M_n_minus_1);
        
        if (diff < epsilon) {
            copyMatrix(*result, M_n);
            freeMatrix(&M_n);
            freeMatrix(&M_n_minus_1);
            return n;
        }
        
        n++;
    }
    
    // Pas de convergence
    copyMatrix(*result, M_n);
    freeMatrix(&M_n);
    freeMatrix(&M_n_minus_1);
    return -1;
}

//Sous-matrices pour les classes

t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index) {
    if (compo_index < 0 || compo_index >= part.size) {
        fprintf(stderr, "Erreur: index de classe invalide\n");
        exit(EXIT_FAILURE);
    }
    
    t_classe classe = part.classes[compo_index];
    int size = classe.size;
    
    t_matrix sub = createEmptyMatrix(size);
    
    // Copier les valeurs correspondantes
    for (int i = 0; i < size; i++) {
        int sommet_i = classe.vertices[i] - 1; // Index 0..n-1
        
        for (int j = 0; j < size; j++) {
            int sommet_j = classe.vertices[j] - 1; // Index 0..n-1
            sub.data[i][j] = matrix.data[sommet_i][sommet_j];
        }
    }
    
    return sub;
}

//Distributions

void afficherDistribution(float *dist, int size, const char *label) {
    printf("%s: [", label);
    for (int i = 0; i < size; i++) {
        printf("%.4f", dist[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

void calculerDistribution(float *pi_0, t_matrix M, int n, float *pi_n) {

    t_matrix M_power = powerMatrix(M, n);
    
    // Multiplier le vecteur ligne pi_0 par M^n
    for (int j = 0; j < M.rows; j++) {
        pi_n[j] = 0.0f;
        for (int i = 0; i < M.rows; i++) {
            pi_n[j] += pi_0[i] * M_power.data[i][j];
        }
    }
    
    freeMatrix(&M_power);
}

//Périodicité (BONUS)

int gcd(int *vals, int nbvals) {
    if (nbvals == 0) return 0;
    
    int result = vals[0];
    for (int i = 1; i < nbvals; i++) {
        int a = result;
        int b = vals[i];
        
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        result = a;
    }
    
    return result;
}

int getPeriod(t_matrix sub_matrix) {
    int n = sub_matrix.rows;
    int *periods = (int *)malloc(n * sizeof(int));
    int period_count = 0;
    
    t_matrix power_matrix = createEmptyMatrix(n);
    t_matrix result_matrix = createEmptyMatrix(n);
    
    copyMatrix(power_matrix, sub_matrix);
    
    for (int cpt = 1; cpt <= n; cpt++) {
        int diag_nonzero = 0;
        
        // Vérifier si la diagonale contient des valeurs non nulles
        for (int i = 0; i < n; i++) {
            if (power_matrix.data[i][i] > 0.0f) {
                diag_nonzero = 1;
                break;
            }
        }
        
        if (diag_nonzero) {
            periods[period_count] = cpt;
            period_count++;
        }
        
        multiplyMatrices(power_matrix, sub_matrix, result_matrix);
        copyMatrix(power_matrix, result_matrix);
    }
    
    int period = gcd(periods, period_count);
    
    free(periods);
    freeMatrix(&power_matrix);
    freeMatrix(&result_matrix);
    
    return period;
}