#include "fonction.h"

int main() {
    liste_adjacence G = readGraph("data/exemple_valid_step3.txt");
    afficherListeAdjacence(G);
    verifierGrapheMarkov(G);
    ecrireMermaid(G, "graphe_mermaid.txt");
    return 0;
}