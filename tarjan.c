#include <stdio.h>
#include <stdlib.h>
#include "tarjan.h"

// ---------- utilitaires partition / classes ----------

static t_classe creerClasseVide(void) {
    t_classe c;
    c.size = 0;
    c.capacity = 4;
    c.vertices = (int *)malloc(c.capacity * sizeof(int));
    if (!c.vertices) {
        perror("Erreur allocation classe");
        exit(EXIT_FAILURE);
    }
    return c;
}

static void ajouterSommetClasse(t_classe *c, int vertex_id) {
    if (c->size == c->capacity) {
        c->capacity *= 2;
        c->vertices = (int *)realloc(c->vertices, c->capacity * sizeof(int));
        if (!c->vertices) {
            perror("Erreur realloc classe");
            exit(EXIT_FAILURE);
        }
    }
    c->vertices[c->size++] = vertex_id;
}

t_partition creerPartitionVide(int capacite_initiale) {
    t_partition p;
    p.size = 0;
    p.capacity = (capacite_initiale > 0) ? capacite_initiale : 1;
    p.classes = (t_classe *)malloc(p.capacity * sizeof(t_classe));
    if (!p.classes) {
        perror("Erreur allocation partition");
        exit(EXIT_FAILURE);
    }
    return p;
}

static int ajouterClasse(t_partition *p, t_classe classe) {
    if (p->size == p->capacity) {
        p->capacity *= 2;
        p->classes = (t_classe *)realloc(p->classes, p->capacity * sizeof(t_classe));
        if (!p->classes) {
            perror("Erreur realloc partition");
            exit(EXIT_FAILURE);
        }
    }
    p->classes[p->size] = classe;
    return p->size++;
}

void libererPartition(t_partition *p) {
    if (!p) return;
    for (int i = 0; i < p->size; ++i) {
        free(p->classes[i].vertices);
    }
    free(p->classes);
    p->classes = NULL;
    p->size = p->capacity = 0;
}

// ---------- affichage ----------

void afficherPartition(const t_partition *p) {
    for (int i = 0; i < p->size; ++i) {
        printf("Composante C%d: {", i + 1);
        for (int j = 0; j < p->classes[i].size; ++j) {
            printf("%d", p->classes[i].vertices[j]);
            if (j < p->classes[i].size - 1) printf(", ");
        }
        printf("}\n");
    }
}

// ---------- Tarjan ----------

t_tarjan_vertex *init_tarjan_vertices(liste_adjacence G) {
    int n = G.taille;
    t_tarjan_vertex *V = (t_tarjan_vertex *)malloc(n * sizeof(t_tarjan_vertex));
    if (!V) {
        perror("Erreur allocation t_tarjan_vertex");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) {
        V[i].id = i + 1;
        V[i].num = -1;
        V[i].lowlink = -1;
        V[i].on_stack = 0;
    }
    return V;
}

// Sous-fonction 'parcours' (DFS de Tarjan)
static void parcours(int v,
                     liste_adjacence G,
                     t_tarjan_vertex *V,
                     int *index,
                     int *stack,
                     int *stack_size,
                     t_partition *p,
                     int *sommet_vers_classe) {
    V[v].num = *index;
    V[v].lowlink = *index;
    (*index)++;

    stack[(*stack_size)++] = v;
    V[v].on_stack = 1;

    // Pour tous les successeurs w de v
    cellule *courant = G.tab[v].head;
    while (courant != NULL) {
        int w = courant->sommet_arrivee - 1; // -> index 0..n-1
        if (V[w].num == -1) {
            // w non encore visité
            parcours(w, G, V, index, stack, stack_size, p, sommet_vers_classe);
            if (V[w].lowlink < V[v].lowlink)
                V[v].lowlink = V[w].lowlink;
        } else if (V[w].on_stack) {
            // w est dans la pile
            if (V[w].num < V[v].lowlink)
                V[v].lowlink = V[w].num;
        }
        courant = courant->suiv;
    }

    // Si v est racine d'une CFC
    if (V[v].lowlink == V[v].num) {
        t_classe c = creerClasseVide();

        while (1) {
            int w = stack[--(*stack_size)];
            V[w].on_stack = 0;
            ajouterSommetClasse(&c, V[w].id); // id = numéro de sommet (1..n)
            if (w == v) break;
        }

        int idx_classe = ajouterClasse(p, c);

        // mise à jour du tableau sommet -> classe
        for (int i = 0; i < p->classes[idx_classe].size; ++i) {
            int sommet = p->classes[idx_classe].vertices[i]; // 1..n
            sommet_vers_classe[sommet - 1] = idx_classe;
        }
    }
}

t_partition tarjan(liste_adjacence G, int *sommet_vers_classe) {
    int n = G.taille;
    t_partition part = creerPartitionVide(n);
    t_tarjan_vertex *V = init_tarjan_vertices(G);

    // pile
    int *stack = (int *)malloc(n * sizeof(int));
    if (!stack) {
        perror("Erreur allocation pile Tarjan");
        exit(EXIT_FAILURE);
    }
    int stack_size = 0;
    int index = 0;

    // initialisation tableau sommet -> classe
    for (int i = 0; i < n; ++i) sommet_vers_classe[i] = -1;

    // Lancement des parcours
    for (int v = 0; v < n; ++v) {
        if (V[v].num == -1) {
            parcours(v, G, V, &index, stack, &stack_size, &part, sommet_vers_classe);
        }
    }

    free(V);
    free(stack);
    return part;
}
