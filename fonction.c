#include "fonction.h"
#include <math.h>
#include "utils.h"
#include <string.h>

cellule* creerCellule(int arrivee, float proba) {
    cellule* newCell = (cellule*) malloc(sizeof(cellule));
    if (!newCell) {
        perror("Erreur d'allocation mémoire pour une cellule");
        exit(EXIT_FAILURE);
    }
    newCell->sommet_arrivee = arrivee;
    newCell->proba = proba;
    newCell->suiv = NULL;
    return newCell;
}

liste creerListeVide() {
    liste L;
    L.head = NULL;
    return L;
}

void ajouterCellule(liste *L, int arrivee, float proba) {
    cellule *courant = L->head;

    // Si une arête vers le même sommet existe déjà, on additionne les probabilités
    while (courant != NULL) {
        if (courant->sommet_arrivee == arrivee) {
            courant->proba += proba;
            return;
        }
        courant = courant->suiv;
    }

    // Sinon on crée une nouvelle cellule
    cellule *newCell = creerCellule(arrivee, proba);
    newCell->suiv = L->head;
    L->head = newCell;
}

void afficherListe(liste L) {
    cellule* courant = L.head;
    printf("[head @]");
    while (courant != NULL) {
        printf(" -> (%d, %.2f)", courant->sommet_arrivee, courant->proba);
        courant = courant->suiv;
    }
    printf("\n");
}

liste_adjacence creerListeAdjacenceVide(int taille) {
    liste_adjacence G;
    G.taille = taille;
    G.tab = (liste*) malloc(taille * sizeof(liste));
    if (!G.tab) {
        perror("Erreur d'allocation mémoire pour la liste d'adjacence");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < taille; i++) {
        G.tab[i] = creerListeVide();
    }
    return G;
}

liste_adjacence readGraph(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        fprintf(stderr, "Erreur : impossible de lire le nombre de sommets\n");
        exit(EXIT_FAILURE);
    }

    liste_adjacence G = creerListeAdjacenceVide(nbvert);

    int depart, arrivee;
    float proba;

    // Lecture ligne par ligne sans compter les lignes vides.
    while (!feof(file)) {
        char buffer[128];
        if (!fgets(buffer, sizeof(buffer), file))
            break;

        if (sscanf(buffer, "%d %d %f", &depart, &arrivee, &proba) == 3) {
            if (depart < 1 || depart > nbvert || arrivee < 1 || arrivee > nbvert) {
                fprintf(stderr, "Sommet invalide (%d -> %d)\n", depart, arrivee);
                continue;
            }
            ajouterCellule(&G.tab[depart - 1], arrivee, proba);
        }
    }

    fclose(file);
    return G;
}


void afficherListeAdjacence(liste_adjacence G) {
    for (int i = 0; i < G.taille; i++) {
        printf("Liste pour le sommet %d: ", i + 1);
        afficherListe(G.tab[i]);
    }
}


void verifierGrapheMarkov(liste_adjacence G) {
    int est_markov = 1; // 1 = vrai, 0 = faux

    for (int i = 0; i < G.taille; i++) {
        float somme = 0.0f;
        cellule *courant = G.tab[i].head;

        while (courant != NULL) {
            somme += courant->proba;
            courant = courant->suiv;
        }

        if (somme < 0.99f || somme > 1.01f) {
            printf("La somme des probabilites du sommet %d est %.2f\n", i + 1, somme);
            est_markov = 0;
        }
    }

    if (est_markov)
        printf("Le graphe est un graphe de Markov\n");
    else
        printf("Le graphe n'est pas un graphe de Markov\n");
}

void ecrireMermaid(liste_adjacence G, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erreur lors de la création du fichier Mermaid");
        return;
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "  layout: elk\n");
    fprintf(file, "  theme: neo\n");
    fprintf(file, "  look: neo\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart LR\n");

    // Déclaration des sommets
    for (int i = 0; i < G.taille; i++) {
        fprintf(file, "%s((%d))\n", getID(i + 1), i + 1);
    }

    // Déclaration des arêtes
    for (int i = 0; i < G.taille; i++) {
        cellule *courant = G.tab[i].head;
        while (courant != NULL) {
            // COPIER les IDs dans des variables locales
            char id_depart[10];
            char id_arrivee[10];

            strcpy(id_depart, getID(i + 1));
            strcpy(id_arrivee, getID(courant->sommet_arrivee));

            fprintf(file, "%s -->|%.2f|%s\n",
                    id_depart,
                    courant->proba,
                    id_arrivee);

            courant = courant->suiv;
        }
    }

    fclose(file);
    printf("Fichier Mermaid genere : %s\n", filename);
}