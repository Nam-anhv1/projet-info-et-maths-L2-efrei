#include <stdio.h>
#include <stdlib.h>

#include "fonction.h"
#include "tarjan.h"
#include "hasse.h"

int main(void) {
    // 1) Lecture du graphe
    liste_adjacence G = readGraph("data/exemple_valid_step3.txt");
    afficherListeAdjacence(G);
    verifierGrapheMarkov(G);

    // 2) Tarjan : partition en classes
    int *sommet_vers_classe = (int *)malloc(G.taille * sizeof(int));
    if (!sommet_vers_classe) {
        perror("Erreur allocation sommet_vers_classe");
        exit(EXIT_FAILURE);
    }

    t_partition part = tarjan(G, sommet_vers_classe);

    printf("\nPartition (composantes fortement connexes) :\n");
    afficherPartition(&part);

    // 3) Liens entre classes (diagramme de Hasse)
    t_link_array liens = buildHasseLinks(&part, &G, sommet_vers_classe);

    printf("\nLiens entre classes (avec redondances) :\n");
    afficherLinkArray(&liens);

    // Suppression des redondances (option)
    removeTransitiveLinks(&liens);

    printf("\nLiens du diagramme de Hasse (sans redondances) :\n");
    afficherLinkArray(&liens);

    // 4) Écriture Mermaid
    ecrireMermaid(G, "graphe_mermaid.txt");
    ecrireMermaidHasse(&part, &liens, "hasse_mermaid.txt");

    // 5) Caractéristiques du graphe
    caracteriserGraphe(&part, &liens);

    // Libérations
    free(sommet_vers_classe);
    free(liens.links);
    libererPartition(&part);

    return 0;
}
