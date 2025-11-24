#ifndef __HASSE_H__
#define __HASSE_H__

#include "tarjan.h"
#include "fonction.h"

// Structure pour un lien entre deux classes
typedef struct {
    int from_class;
    int to_class;
} t_link;

// Structure pour stocker tous les liens
typedef struct {
    t_link *links;
    int size;
    int capacity;
} t_link_array;
t_link_array createLinkArray(int initial_capacity);
void addLink(t_link_array *arr, int from, int to);
int linkExists(const t_link_array *arr, int from, int to);
t_link_array buildHasseLinks(const t_partition *part,
                              const liste_adjacence *G,
                              const int *sommet_vers_classe);

void afficherLinkArray(const t_link_array *arr);

void removeTransitiveLinks(t_link_array *arr);

// Générer le diagramme de Hasse en format Mermaid
void ecrireMermaidHasse(const t_partition *part,
                        const t_link_array *liens,
                        const char *filename);

// Caractériser le graphe (classes transitoires/persistantes, etc.)
void caracteriserGraphe(const t_partition *part, const t_link_array *liens);

#endif