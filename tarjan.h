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

// Une classe = une composante fortment connexe
typedef struct {
    int *vertices;
    int size;
    int capacity;
} t_classe;

// Partition = ensemble de classe
typedef struct {
    t_classe *classes;
    int size;
    int capacity;
} t_partition;

t_tarjan_vertex *init_tarjan_vertices(liste_adjacence G);

// Gestion de partition
t_partition creerPartitionVide(int capacite_initiale);
void libererPartition(t_partition *p);
void afficherPartition(const t_partition *p);

t_partition tarjan(liste_adjacence G, int *sommet_vers_classe);

#endif
