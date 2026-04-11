#include <stdio.h> 
#include <stdlib.h>  // malloc 
#include <string.h>  // strcmp et strcpy

typedef struct {
    int source;
    int cible;
} Arete;

int compter_ligne(char *chemin){
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return 1;
        }
    char ligne[256];
    int nbr_ligne = 0;
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        nbr_ligne += 1;
    }

    fclose(f);  
    return nbr_ligne; 
}


int compter_noeuds(char *chemin){
// Lire le fichier et compter le nombre de nœuds uniques
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return NULL;
        }
    char ligne[256];
    int nbr_noeud = 0;
    int max = compter_ligne(chemin) * 2;
    char noeud[max][20];
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        char source[20];
        char cible[20]; 
        sscanf(ligne, "%s %s", source, cible);

        int trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(source, noeud[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            strcpy(noeud[nbr_noeud],source);;
            nbr_noeud++;
        }

        trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(cible, noeud[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            strcpy(noeud[nbr_noeud],cible);
            nbr_noeud++;
        }
    }
    fclose(f);  
    return nbr_noeud;
}

char** construire_table_noeuds(char *chemin){
// Lire le fichier et stocker tous les nœuds uniques dans un tableau
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return NULL;
        }
    char ligne[256];
    int nbr_noeud = compter_noeuds(chemin);
    char **table_noeuds = malloc(nbr_noeud * sizeof(char*));

    // malloc pour chaque string individuelle

    for (int i = 0; i < nbr_noeud; i++){
        table_noeuds[i] = malloc(20 * sizeof(char));
    }


    int nbr_ajouter = 0;
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        char source[20];
        char cible[20]; 
        sscanf(ligne, "%s %s", source, cible);

        int trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(source, table_noeuds[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            strcpy( table_noeuds[nbr_ajouter],source);
            nbr_ajouter++;
        }

        trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(cible, table_noeuds[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            strcpy(table_noeuds[nbr_ajouter],cible);
            nbr_ajouter++;
        }
    }

    fclose(f);  
    return table_noeuds;

}

// Pour chaque noeud attribut un indice
int trouver_indice(char **table, int nb_noeuds, char *id){
    for (int i = 0; i < nb_noeuds; i++) {
        if (strcmp(table[i], id) == 0) {
            return i;
        }
    }
    return - 1; // identifiant n'existe pas dans la table
}



Arete* lire_aretes(char *chemin, char **table, int nb_noeuds, int nb_aretes){
    Arete *table_aretes = malloc(nb_aretes * sizeof(Arete));
    if (table_aretes == NULL) {
    perror("Erreur malloc");
    return NULL;
}
    
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return 1;
        }
    int compteur = 0;
    char ligne[256];
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        char source[20];
        char cible[20]; 
        sscanf(ligne, "%s %s", source, cible);

        int indice_source = trouver_indice(table,nb_noeuds,source);
        int indice_cible = trouver_indice(table,nb_noeuds,cible);

        table_aretes[compteur].source = indice_source;
        table_aretes[compteur].cible = indice_cible;

        compteur++;
     
    }

    fclose(f);
    return table_aretes;

}


void free_table_noeuds(char **table, int nb_noeuds) {
// Libère mémoire allouée par construire_table_noeuds
    for (int i = 0; i < nb_noeuds; i++) {
        free(table[i]);
    }
    free(table);
}

// 2. Construire le graphe en mémoire













