#include "fonction.h"

int main() {
    // Lecture du graphe depuis un fichier
    liste_adjacence G = readGraph("data/exemple_valid_step3.txt");

    // Affichage du graphe
    afficherListeAdjacence(G);

    verifierGrapheMarkov(G);

    // Générer le fichier Mermaid
    ecrireFichierMermaid(G, "graphe_mermaid.txt");
    return 0;
}
