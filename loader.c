#include <stdio.h>

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
            return 1;
        }
    char ligne[256];
    int nbr_noeud = 0;
    int max = compter_lignes(chemin) * 2;
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
            noeud[nbr_noeud][20] = source;
            nbr_noeud++;
        }

        int trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(cible, noeud[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            noeud[nbr_noeud][20] = cible;
            nbr_noeud++;
        }
    }

    fclose(f);  
    return nbr_noeud;
}

void construire_table_noeuds(char *chemin){
// Lire le fichier et stocker tous les nœuds uniques dans un tableau
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return 1;
        }
    char ligne[256];
    int nbr_noeud = compter_noeuds(chemin);
    char **table_noeuds = malloc(nbr_noeud * sizeof(char*));
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
            table_noeuds[nbr_ajouter] = source;
            nbr_ajouter++;
        }

        int trouve = 0;
        for (int i = 0; i< nbr_noeud; i++){
            if (strcmp(cible, table_noeuds[i]) == 0){
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            table_noeuds[nbr_ajouter] = cible;
            nbr_ajouter++;
        }
    }

    fclose(f);  
    return nbr_noeud;


}










