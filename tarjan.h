#ifndef __TARJAN_H__
#define __TARJAN_H__

#include "fonction.h"

// Sommet pour l'algorithme de Tarjan
typedef struct {
    int id;         // numéro du sommet dans le graphe (1..n)
    int num;        // numéro de visite (index)
    int lowlink;    // numéro accessible
    int on_stack;   // booléen (0/1)
} t_tarjan_vertex;

// Une classe = une composante fortement connexe
typedef struct {
    int *vertices;  // tableau de numéros de sommets (1..n)
    int size;       // nombre de sommets
    int capacity;   // capacité allouée
} t_classe;

// Partition = ensemble de classes
typedef struct {
    t_classe *classes;
    int size;       // nombre de classes
    int capacity;   // capacité allouée
} t_partition;

// Initialisation des sommets pour Tarjan
t_tarjan_vertex *init_tarjan_vertices(liste_adjacence G);

// Gestion de partition
t_partition creerPartitionVide(int capacite_initiale);
void libererPartition(t_partition *p);
void afficherPartition(const t_partition *p);

// Algorithme de Tarjan.
// - G : graphe
// - sommet_vers_classe : tableau de taille G.taille, rempli avec l'indice
//   de la classe pour chaque sommet (0..nb_classes-1)
t_partition tarjan(liste_adjacence G, int *sommet_vers_classe);

#endif // __TARJAN_H__
