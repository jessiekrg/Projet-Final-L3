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
            return -1; // J'ai modifié NULL par -1 car la fonction tu as mis int pour type de retout de la fonction 
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
            strcpy(noeud[nbr_noeud],source); // suppression de ";;""
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
            return NULL; // pareille
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

// 2. Construire le graphe en mémoire (liste d'ajacence)
typedef struct Voisin {
    int id;
    struct Voisin *suivant;
    double poids;
    } Voisin ;


typedef struct {
    int nb_noeuds;
    int nb_aretes;
    Voisin **Tableau_Voisins;
    double poids_total;
    } Graphe ;

Graphe* initialiser_graphe(int nombre_noeuds, int nombre_aretes){
    Graphe *Mon_Graphe = malloc(sizeof(Graphe));

    Mon_Graphe -> nb_noeuds = nombre_noeuds;
    Mon_Graphe -> nb_aretes = nombre_aretes;
    Mon_Graphe -> Tableau_Voisins = malloc(sizeof(Voisin*) * nombre_noeuds);
    Mon_Graphe -> poids_total = 0;

    for (int i = 0; i < nombre_noeuds; i++) {
        
        Mon_Graphe->Tableau_Voisins[i] = NULL;
        }
    
    return Mon_Graphe;
    }

void Ajouter_Aretes(Graphe *G, int a, int b, double Nouveau_poids ){

    Voisin *Nouveau_Voisin_de_A = malloc(sizeof(Voisin));
    if (Nouveau_Voisin_de_A == NULL){
        return;
        }

    Nouveau_Voisin_de_A -> id = b;
    Nouveau_Voisin_de_A -> suivant = G->Tableau_Voisins[a];
    Nouveau_Voisin_de_A -> poids = Nouveau_poids;
    
    G -> Tableau_Voisins[a] = Nouveau_Voisin_de_A; // Tableau[a] → [ami 1] → [ami 2] → rien  => Tableau[a] → [nouveau] → [ami 1] → [ami 2] → rien


    Voisin *Nouveau_Voisin_de_B = malloc(sizeof(Voisin));
    if (Nouveau_Voisin_de_B == NULL){
        return;
        }

    Nouveau_Voisin_de_B -> id = a;
    Nouveau_Voisin_de_B -> suivant = G->Tableau_Voisins[b];
    Nouveau_Voisin_de_B -> poids =  Nouveau_poids;
    
    G -> Tableau_Voisins[b] = Nouveau_Voisin_de_B; // Tableau[a] → [ami 1] → [ami 2] → rien  => Tableau[a] → [nouveau] → [ami 1] → [ami 2] → rien

    G->poids_total += Nouveau_poids;
    }

Graphe* construire_graphe(Arete *table_aretes, int nombre_noeuds, int nombre_aretes) {

    Graphe *Mon_Reseau = initialiser_graphe(nombre_noeuds, nombre_aretes);

    for (int i = 0; i < nombre_aretes; i++){ 
        Ajouter_Aretes(Mon_Reseau, table_aretes[i].source, table_aretes[i].cible, 1.0);
        }
    
    return Mon_Reseau;
    }

void free_graphe(Graphe *G) {
    if (G == NULL) return; 
    for (int i = 0; i < G->nb_noeuds; i++) {
        Voisin *courant = G->Tableau_Voisins[i];
        
        while (courant != NULL) {
            Voisin *temp = courant;
            courant = courant->suivant;
            free(temp);
        }
    }
    free(G->Tableau_Voisins);
    free(G);
}



// Les fonctions pour gérer les changements de communautés en particulier 

void retirer_noeud(int i, int *communautes, Graphe *G, double *sigma_in, double *sigma_tot, double *k) {
    // communauté du noeud i
    int communaute = communautes[i];
    // calculer ki_in ( ki_in = poids des arêtes entre noeuds i et ses voisins de la meme communauté)
    double ki_in = 0;

    Voisin *Voisin_noeud = G -> Tableau_Voisins[i];
    while (Voisin_noeud != NULL){ // Parcours tous les voisin du noeud i 
        if (communautes[Voisin_noeud -> id] == communaute ){ 
            ki_in += Voisin_noeud -> poids;
            }
        Voisin_noeud = Voisin_noeud-> suivant;
        }
    
    sigma_in[communaute] -= 2* ki_in; // on enlève à sigma in (= la sommes des poids des arêtes des noeuds de la communauté dont i appartient) la sommes des poids des arêtes incident au noeud i qui lie des noeuds qui appartiennent à la commu
    sigma_tot[communaute] -= k[i]; // on enleve à sigma_tot (la somme des poids des arêtes incidentes à tous les noeuds de la commu dont i appartient) la somme des tous les poids des noeuds incidents au noeud i )

    communautes[i] = -1; // retirer le noeud i de sa communauté
}


void rajouter_noeud(int i, int communaute_cible, int *communautes, Graphe *G, double *sigma_in, double *sigma_tot, double *k) {
    communautes[i] = communaute_cible;

    double ki_in = 0;

    Voisin *Voisin_noeud = G -> Tableau_Voisins[i];
    while (Voisin_noeud != NULL){ // Parcours tous les voisin du noeud i 
        if (communautes[Voisin_noeud -> id] == communaute_cible ){ 
            ki_in += Voisin_noeud -> poids;
            }
        Voisin_noeud = Voisin_noeud-> suivant;
        }

    sigma_in[communaute_cible] += 2* ki_in; // on enlève à sigma in (= la sommes des poids des arêtes des noeuds de la communauté dont i appartient) la sommes des poids des arêtes incident au noeud i qui lie des noeuds qui appartiennent à la commu
    sigma_tot[communaute_cible] += k[i]; // on enleve à sigma_tot (la somme des poids des arêtes incidentes à tous les noeuds de la commu dont i appartient) la somme des tous les poids des noeuds incidents au noeud i )
    }

int main(int argc, char *argv[]) {
    // 
    if (argc < 2){
        return -1;
        }

    char *chemin = argv[1];
    int nb_noeuds = compter_noeuds(chemin);
    int nb_aretes = compter_ligne(chemin);

    char **table_noeud = construire_table_noeuds(chemin);

    Arete *table_arete = lire_aretes(chemin,table_noeud, nb_noeuds, nb_aretes);

    Graphe *G = construire_graphe(table_arete,nb_noeuds,nb_aretes);



    // LOUVAIN 

    // initialisation communautés ou chaque noeud constitue sa propre communauté initialement
    int *communautes = malloc(sizeof(int)*nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++){
        communautes[i] = i; // chaque noeud = sa propre communauté
    }

    // Calcul de k[i] pour chaque noeuds i (k[i] =  la somme des poids de toutes les arêtes connectées au  noeud i)
    double *k = malloc(sizeof(double)*nb_noeuds); // k
    for (int i = 0; i < nb_noeuds; i++) { 
        k[i] = 0; // initialisation à 0 

        Voisin *courant = G -> Tableau_Voisins[i]; // Initialisation du tableau des voisins du noeud i 
        while (courant != NULL){ // On parcourt tous les voisins du noeud i tant qu'il y a des voisins 
            k[i] += courant -> poids; // On incrémente la valeur ki avec le poids de l'arete (qui lie le voisin au noeud i) 
            courant = courant -> suivant; // on passe au voisin suivant (tant que qu'il y en a un)
        }
    }

    // Calcul de sigma_in = sommes des tous les poids des arêtes internes (cad juste les arêtes entre noeuds de la communauté)
    double *sigma_in = malloc(sizeof(double) * nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++ ){
        // Pour chaque communauté (noeuds au départ) initialement sigma_in[i] = 0 (au début du moins 1 commu = 1 noeud )
        sigma_in[i] = 0;
        }

    // Calcul de sigma_tot = sommes des tous les poids des arêtes internes + incidente aux noeuds de la communauté courante
    double *sigma_tot = malloc(sizeof(double) * nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++ ){
        // Pour chaque commu (noeuds au départ) initialement sigma_tot[i] = k[i] (sommes des poids des arêtes incidentes au noeud de la communauté courante (donc le noeud au début) ce  qui est k[i])
        sigma_tot[i] = k[i]; 
        }

    // Initialisation de m qui est la somme de toutes les poids des arêtes du graphe
    double m = G -> poids_total;


    // BOUCLE PHASE 1 ET 2 

    while (1){
        // Boucle phase 1 : Parcourir tous les noeuds 
        int noeuds_bougent = 1;

        while (noeuds_bougent){
            noeuds_bougent = 0;

            

        }
    
    
    }


    
}