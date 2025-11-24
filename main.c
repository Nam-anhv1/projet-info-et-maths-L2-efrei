#include <stdio.h>
#include <stdlib.h>

#include "fonction.h"
#include "tarjan.h"
#include "hasse.h"
#include "matrix.h"

void testerPartie3_Etape1(const char *filename);
void testerPartie3_Etape2(const char *filename);

int main(void) {
    printf("========================================\n");
    printf("        PROJET GRAPHES DE MARKOV\n");
    printf("========================================\n\n");

    // ========== PARTIE 1 & 2 ==========
    printf("========================================\n");
    printf("       PARTIE 1 & 2 : Analyse du graphe\n");
    printf("========================================\n\n");

    liste_adjacence G = readGraph("../data/exemple_valid_step3.txt");
    afficherListeAdjacence(G);
    verifierGrapheMarkov(G);

    // Tarjan
    int *sommet_vers_classe = (int *)malloc(G.taille * sizeof(int));
    if (!sommet_vers_classe) {
        perror("Erreur allocation sommet_vers_classe");
        exit(EXIT_FAILURE);
    }

    t_partition part = tarjan(G, sommet_vers_classe);
    printf("\nPartition (composantes fortement connexes) :\n");
    afficherPartition(&part);

    // Diagramme de Hasse
    t_link_array liens = buildHasseLinks(&part, &G, sommet_vers_classe);
    printf("\nLiens entre classes :\n");
    afficherLinkArray(&liens);

    // Génération Mermaid
    ecrireMermaid(G, "graphe_mermaid.txt");
    ecrireMermaidHasse(&part, &liens, "hasse_mermaid.txt");

    // Caractéristiques
    caracteriserGraphe(&part, &liens);

    printf("\n========================================\n");
    printf("    PARTIE 3 : CALCULS MATRICIELS\n");
    printf("========================================\n\n");

    // Test avec l'exemple météo
    printf(">>> TEST AVEC EXEMPLE METEO\n\n");
    testerPartie3_Etape1("../data/exemple_meteo.txt");

    printf("\n>>> TEST AVEC EXEMPLE VALID STEP 3\n\n");
    testerPartie3_Etape2("../data/exemple_valid_step3.txt");

    free(sommet_vers_classe);
    free(liens.links);
    libererPartition(&part);

    return 0;
}

void testerPartie3_Etape1(const char *filename) {
    liste_adjacence G = readGraph(filename);

    printf("--- Creation de la matrice M ---\n");
    t_matrix M = createMatrixFromGraph(G);
    afficherMatrix(M);

    printf("\n--- Calcul de M^3 ---\n");
    t_matrix M3 = powerMatrix(M, 3);
    afficherMatrix(M3);

    printf("\n--- Calcul de M^7 ---\n");
    t_matrix M7 = powerMatrix(M, 7);
    afficherMatrix(M7);

    printf("\n--- Test de convergence (epsilon = 0.01) ---\n");
    t_matrix M_conv = createEmptyMatrix(M.rows);
    int n_conv = findConvergence(M, 0.01f, &M_conv);

    if (n_conv > 0) {
        printf("Convergence atteinte a n = %d\n", n_conv);
        printf("Matrice M^%d :\n", n_conv);
        afficherMatrix(M_conv);
    } else {
        printf("Pas de convergence detectee (limite 1000 iterations)\n");
    }

    freeMatrix(&M);
    freeMatrix(&M3);
    freeMatrix(&M7);
    freeMatrix(&M_conv);
}


void testerPartie3_Etape2(const char *filename) {
    liste_adjacence G = readGraph(filename);

    // Tarjan
    int *sommet_vers_classe = (int *)malloc(G.taille * sizeof(int));
    t_partition part = tarjan(G, sommet_vers_classe);

    t_link_array liens = buildHasseLinks(&part, &G, sommet_vers_classe);

    printf("--- Analyse des classes ---\n\n");

    t_matrix M = createMatrixFromGraph(G);

    // Identifier les classes transitoires
    int *est_transitoire = (int *)calloc(part.size, sizeof(int));
    for (int i = 0; i < liens.size; i++) {
        est_transitoire[liens.links[i].from_class] = 1;
    }

    for (int i = 0; i < part.size; i++) {
        printf("Classe C%d {", i + 1);
        for (int j = 0; j < part.classes[i].size; j++) {
            printf("%d", part.classes[i].vertices[j]);
            if (j < part.classes[i].size - 1) printf(",");
        }
        printf("} : ");

        if (est_transitoire[i]) {
            printf("TRANSITOIRE (distribution limite = 0)\n\n");
        } else {
            printf("PERSISTANTE\n");

            t_matrix sub = subMatrix(M, part, i);

            printf("Sous-matrice :\n");
            afficherMatrix(sub);

            t_matrix sub_conv = createEmptyMatrix(sub.rows);
            int n = findConvergence(sub, 0.01f, &sub_conv);

            if (n > 0) {
                printf("Distribution stationnaire atteinte a n = %d\n", n);
                printf("Distribution limite (ligne 1 de la matrice) : [");
                for (int j = 0; j < sub.rows; j++) {
                    printf("%.4f", sub_conv.data[0][j]);
                    if (j < sub.rows - 1) printf(", ");
                }
                printf("]\n");
            } else {
                printf("Pas de convergence (possible periodicite)\n");

                // BONUS : Calculer la période
                int period = getPeriod(sub);
                printf("Periode de la classe : %d\n", period);
            }

            freeMatrix(&sub);
            freeMatrix(&sub_conv);
            printf("\n");
        }
    }

    free(est_transitoire);
    free(sommet_vers_classe);
    free(liens.links);
    libererPartition(&part);
    freeMatrix(&M);
}