#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <stdio.h>
#include <stdlib.h>

// Structures de base pour le graphe
typedef struct cellule {
    int sommet_arrivee;
    float proba;
    struct cellule *suiv;
} cellule;

typedef struct {
    cellule *head;
} liste;

typedef struct {
    int taille;
    liste *tab;
} liste_adjacence;

// Fonctions de base
cellule* creerCellule(int arrivee, float proba);
liste creerListeVide();
void ajouterCellule(liste *L, int arrivee, float proba);
void afficherListe(liste L);

// Fonctions sur le graphe
liste_adjacence creerListeAdjacenceVide(int taille);
liste_adjacence readGraph(const char *filename);
void afficherListeAdjacence(liste_adjacence G);

void verifierGrapheMarkov(liste_adjacence G);

// écriture du graphe en Mermaid
void ecrireMermaid(liste_adjacence G, const char *nomFichier);

#endif
