#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hasse.h"
#include "utils.h"

//Gestion du tableau de liens

t_link_array createLinkArray(int initial_capacity) {
    t_link_array arr;
    arr.size = 0;
    arr.capacity = (initial_capacity > 0) ? initial_capacity : 4;
    arr.links = (t_link *)malloc(arr.capacity * sizeof(t_link));
    if (!arr.links) {
        perror("Erreur allocation t_link_array");
        exit(EXIT_FAILURE);
    }
    return arr;
}

int linkExists(const t_link_array *arr, int from, int to) {
    for (int i = 0; i < arr->size; i++) {
        if (arr->links[i].from_class == from && arr->links[i].to_class == to) {
            return 1;
        }
    }
    return 0;
}

void addLink(t_link_array *arr, int from, int to) {
    // Vérifier si le lien existe déjà
    if (linkExists(arr, from, to)) {
        return;
    }

    // Agrandir si nécessaire
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->links = (t_link *)realloc(arr->links, arr->capacity * sizeof(t_link));
        if (!arr->links) {
            perror("Erreur realloc t_link_array");
            exit(EXIT_FAILURE);
        }
    }

    // Ajouter le lien
    arr->links[arr->size].from_class = from;
    arr->links[arr->size].to_class = to;
    arr->size++;
}

void afficherLinkArray(const t_link_array *arr) {
    if (arr->size == 0) {
        printf("Aucun lien entre classes\n");
        return;
    }

    for (int i = 0; i < arr->size; i++) {
        printf("C%d -> C%d\n",
               arr->links[i].from_class + 1,
               arr->links[i].to_class + 1);
    }
}

//Construction des liens entre classe

t_link_array buildHasseLinks(const t_partition *part,
                              const liste_adjacence *G,
                              const int *sommet_vers_classe) {
    t_link_array liens = createLinkArray(G->taille);

    for (int i = 0; i < G->taille; i++) {
        int Ci = sommet_vers_classe[i];

        cellule *courant = G->tab[i].head;
        while (courant != NULL) {
            int j = courant->sommet_arrivee - 1;
            int Cj = sommet_vers_classe[j];

            if (Ci != Cj) {
                addLink(&liens, Ci, Cj);
            }

            courant = courant->suiv;
        }
    }

    return liens;
}

//Suppression des liens transitifs (OPTIONNEL)

void removeTransitiveLinks(t_link_array *arr) {
    int nb_classes = 0;

    for (int i = 0; i < arr->size; i++) {
        if (arr->links[i].from_class > nb_classes)
            nb_classes = arr->links[i].from_class;
        if (arr->links[i].to_class > nb_classes)
            nb_classes = arr->links[i].to_class;
    }
    nb_classes++;

    // Créer une matrice d'adjacence
    int **mat = (int **)malloc(nb_classes * sizeof(int *));
    for (int i = 0; i < nb_classes; i++) {
        mat[i] = (int *)calloc(nb_classes, sizeof(int));
    }

    // Remplir la matrice avec les liens existants
    for (int i = 0; i < arr->size; i++) {
        mat[arr->links[i].from_class][arr->links[i].to_class] = 1;
    }

    // Fermeture transitive avec l algorithme de Warshall
    int **transitive = (int **)malloc(nb_classes * sizeof(int *));
    for (int i = 0; i < nb_classes; i++) {
        transitive[i] = (int *)calloc(nb_classes, sizeof(int));
        for (int j = 0; j < nb_classes; j++) {
            transitive[i][j] = mat[i][j];
        }
    }

    for (int k = 0; k < nb_classes; k++) {
        for (int i = 0; i < nb_classes; i++) {
            for (int j = 0; j < nb_classes; j++) {
                if (transitive[i][k] && transitive[k][j]) {
                    transitive[i][j] = 1;
                }
            }
        }
    }

    t_link_array new_arr = createLinkArray(arr->capacity);

    for (int i = 0; i < arr->size; i++) {
        int from = arr->links[i].from_class;
        int to = arr->links[i].to_class;
        int is_redundant = 0;

        // Vérifier s'il existe un chemin indirect
        for (int k = 0; k < nb_classes; k++) {
            if (k != from && k != to) {
                if (mat[from][k] && transitive[k][to]) {
                    is_redundant = 1;
                    break;
                }
            }
        }

        if (!is_redundant) {
            addLink(&new_arr, from, to);
        }
    }

    // Remplacer l'ancien tableau
    free(arr->links);
    arr->links = new_arr.links;
    arr->size = new_arr.size;
    arr->capacity = new_arr.capacity;

    // Libérer les matrices
    for (int i = 0; i < nb_classes; i++) {
        free(mat[i]);
        free(transitive[i]);
    }
    free(mat);
    free(transitive);
}

//Génération Mermaid pour Hasse

void ecrireMermaidHasse(const t_partition *part,
                        const t_link_array *liens,
                        const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erreur création fichier Mermaid Hasse");
        return;
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "  layout: elk\n");
    fprintf(file, "  theme: neo\n");
    fprintf(file, "  look: neo\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart TB\n");

    // Déclaration des classes
    for (int i = 0; i < part->size; i++) {
        fprintf(file, "C%d[\"C%d: {", i + 1, i + 1);
        for (int j = 0; j < part->classes[i].size; j++) {
            fprintf(file, "%d", part->classes[i].vertices[j]);
            if (j < part->classes[i].size - 1) fprintf(file, ",");
        }
        fprintf(file, "}\"]\n");
    }

    // Déclaration des liens
    for (int i = 0; i < liens->size; i++) {
        fprintf(file, "C%d --> C%d\n",
                liens->links[i].from_class + 1,
                liens->links[i].to_class + 1);
    }

    fclose(file);
    printf("Fichier Mermaid Hasse genere : %s\n", filename);
}

//Caractérisation du graphe

void caracteriserGraphe(const t_partition *part, const t_link_array *liens) {
    printf("\n=== CARACTERISTIQUES DU GRAPHE ===\n");

    // Vérifier si le graphe est irréductible
    if (part->size == 1) {
        printf("Le graphe est IRREDUCTIBLE (1 seule classe)\n");
    } else {
        printf("Le graphe N'EST PAS irreductible (%d classes)\n", part->size);
    }

    int *est_transitoire = (int *)calloc(part->size, sizeof(int));
    for (int i = 0; i < liens->size; i++) {
        est_transitoire[liens->links[i].from_class] = 1;
    }

    printf("\n--- Classes ---\n");
    for (int i = 0; i < part->size; i++) {
        printf("Classe C%d {", i + 1);
        for (int j = 0; j < part->classes[i].size; j++) {
            printf("%d", part->classes[i].vertices[j]);
            if (j < part->classes[i].size - 1) printf(",");
        }
        printf("} : ");

        if (est_transitoire[i]) {
            printf("TRANSITOIRE\n");
        } else {
            printf("PERSISTANTE");
            if (part->classes[i].size == 1) {
                printf(" (etat %d est ABSORBANT)", part->classes[i].vertices[0]);
            }
            printf("\n");
        }
    }

    free(est_transitoire);
}