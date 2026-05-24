#include <stdio.h> 
#include <stdlib.h>  // malloc 
#include <string.h>  // strcmp et strcpy
#include "uthash.h" // bibliothèque pour pouvoir manipuler des fonctions de hachage


// Structure qui sera hachée
typedef struct{
    char noeud[50]; // chaque noeud lu = clef
    int valeur;
    UT_hash_handle hh;
} Noeud;

typedef struct {
    int source;
    int cible;
} Arete;

Noeud *tbl_noeud = NULL;

int compter_ligne(char *chemin){
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return -1;
        }
    char ligne[256];
    int nbr_ligne = 0;
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        nbr_ligne += 1;
    }

    fclose(f);  
    return nbr_ligne; 
}


int inserer(Noeud **tbl_noeud, char *noeud, int *indice_actuel){
    Noeud *n; // permet de stocker ou pas la valeur trouver dans le tableau

    // Vérifier si (valeur,clef) est déjà présent dans le tableau
    HASH_FIND_STR(*tbl_noeud,noeud,n);
    if (n == NULL){ // donc si la clef n'est pas trouvé dans le tableau
        n = malloc(sizeof(Noeud));
        strcpy(n->noeud, noeud);
        n -> valeur =  *indice_actuel;
        HASH_ADD_STR(*tbl_noeud,noeud,n);
        (*indice_actuel)++;
        return n->valeur; 
    }

    return n->valeur;;
}

int construire_table_noeuds(char *chemin){
    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return -1;
            
        }
    char ligne[256];
    int indice = 0;
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        char source[50];
        char cible[50]; 
        sscanf(ligne, "%s %s", source, cible);

        inserer(&tbl_noeud,source,&indice);

        inserer(&tbl_noeud,cible,&indice);
    }

    fclose(f);

    return indice; // retourne le nombre total de noeud trouvé
}

Arete *construire_table_arete(char *chemin,int nb_aretes){
    Arete *table_aretes = malloc(nb_aretes * sizeof(Arete));

    FILE *f = fopen(chemin ,"r");
        if (f == NULL) {
            perror("Erreur d'ouverture");
            return NULL;
            
        }
    char ligne[256];
    int compteur = 0;
    int indice = 0;
    while (fgets(ligne, sizeof(ligne),f)  != NULL){
        char source[50];
        char cible[50]; 
        sscanf(ligne, "%s %s", source, cible);

        table_aretes[compteur].source = inserer(&tbl_noeud,source,&indice);
        table_aretes[compteur].cible = inserer(&tbl_noeud,cible,&indice);

        compteur++;
    }
    fclose(f);
    return table_aretes; // On retourne le pointeur vers le tableau ( c'est un ** )
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
    double poids_arete_vers_autre = 0;
    double poids_boucle_soi = 0;

    Voisin *Voisin_noeud = G -> Tableau_Voisins[i];
    while (Voisin_noeud != NULL){ // Parcours tous les voisin du noeud i 

        if (Voisin_noeud -> id == i ){ 
            poids_boucle_soi += Voisin_noeud ->poids;
            }

        else if (communautes[Voisin_noeud -> id] == communaute ){ 
            poids_arete_vers_autre += Voisin_noeud -> poids;
            }
        
        Voisin_noeud = Voisin_noeud-> suivant;
        }
    
    sigma_in[communaute] -= (2* poids_arete_vers_autre) + poids_boucle_soi; // on enlève à sigma in (= la sommes des poids des arêtes des noeuds de la communauté dont i appartient) la sommes des poids des arêtes incident au noeud i qui lie des noeuds qui appartiennent à la commu
    sigma_tot[communaute] -= k[i]; // on enleve à sigma_tot (la somme des poids des arêtes incidentes à tous les noeuds de la commu dont i appartient) la somme des tous les poids des noeuds incidents au noeud i )

    communautes[i] = -1; // retirer le noeud i de sa communauté
}


void rajouter_noeud(int i, int communaute_cible, int *communautes, Graphe *G, double *sigma_in, double *sigma_tot, double *k) {
    communautes[i] = communaute_cible;

    double poids_arete_vers_autre = 0;
    double poids_boucle_soi = 0;

    Voisin *Voisin_noeud = G -> Tableau_Voisins[i];
    while (Voisin_noeud != NULL){ // Parcours tous les voisin du noeud i 
        if (Voisin_noeud -> id == i ){ 
            poids_boucle_soi += Voisin_noeud ->poids;
            }
            
        else if (communautes[Voisin_noeud -> id] == communaute_cible ){ 
            poids_arete_vers_autre += Voisin_noeud -> poids;
            }
        
        Voisin_noeud = Voisin_noeud-> suivant;
        }

    sigma_in[communaute_cible] += (2* poids_arete_vers_autre) + poids_boucle_soi ; // on enlève à sigma in (= la sommes des poids des arêtes des noeuds de la communauté dont i appartient) la sommes des poids des arêtes incident au noeud i qui lie des noeuds qui appartiennent à la commu
    sigma_tot[communaute_cible] += k[i]; // on enleve à sigma_tot (la somme des poids des arêtes incidentes à tous les noeuds de la commu dont i appartient) la somme des tous les poids des noeuds incidents au noeud i )
    }




double delta_Q(double sigma_in, double sigma_tot, double k, double ki_in_c , double m){
    double a = ( (sigma_in + 2*ki_in_c)/(2*m) ) - ( (sigma_tot + k)/(2*m)*(sigma_tot + k)/(2*m) );
    double b = (sigma_in / (2*m) ) - ( (sigma_tot)/(2*m)*(sigma_tot)/(2*m) ) - (k/(2*m))*(k/(2*m));

    return a-b;
}

double calculer_modularite_finale(int nb_noeuds, double *sigma_in, double *sigma_tot, double m) {
    double Q = 0;
    for (int i = 0; i < nb_noeuds; i++) {
        if (sigma_tot[i] > 0) {
            double term1 = sigma_in[i] / (2.0 * m);
            double term2 = (sigma_tot[i] / (2.0 * m)) * (sigma_tot[i] / (2.0 * m));
            Q += (term1 - term2);
        }
    }
    return Q;
}

int main(int argc, char *argv[]) {

    if (argc < 2){
        return -1;
    }

    char *chemin = argv[1];
    int nb_noeuds = construire_table_noeuds(chemin);

    int nb_noeuds_og = nb_noeuds; // permet de le nb og de noeuds pour la représentation graphique

    int nb_aretes = compter_ligne(chemin);

    Arete *table_arete = construire_table_arete(chemin, nb_aretes);

    Graphe *G = construire_graphe(table_arete,nb_noeuds,nb_aretes);



    // LOUVAIN 

    // initialisation communautés ou chaque noeud constitue sa propre communauté initialement
    int *communautes = malloc(sizeof(int)*nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++){
        communautes[i] = i; // chaque noeud = sa propre communauté
    }



    // Calcul de k[i] pour chaque noeuds i (k[i] =  la somme des poids/degrés de toutes les arêtes connectées au  noeud i)
    double *k = malloc(sizeof(double)*nb_noeuds); // k
    for (int i = 0; i < nb_noeuds; i++) { 
        k[i] = 0; // initialisation à 0 

        Voisin *courant = G -> Tableau_Voisins[i]; // Initialisation du tableau des voisins dee chaque noeuds 
        while (courant != NULL){ // On parcourt tous les voisins du noeud i tant qu'il y a des voisins 
            k[i] += courant -> poids; // On incrémente la valeur ki avec le poids de l'arete (qui lie le voisin au noeud i) 
            courant = courant -> suivant; // on passe au voisin suivant (tant que qu'il y en a un)
        }
    }

    // Calcul de sigma_in = sommes des tous les poids des arêtes internes (cad juste les arêtes entre noeuds de la communauté)
    double *sigma_in = malloc(sizeof(double) * nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++ ) {
        sigma_in[i] = 0;
        Voisin *courant = G->Tableau_Voisins[i];
        while (courant != NULL) {
            if (courant->id == i) { 
                sigma_in[i] += courant->poids;
            }
            courant = courant->suivant;
        }
    }

    // Calcul de sigma_tot = sommes des tous les poids des arêtes internes + incidente aux noeuds de la communauté courante
    double *sigma_tot = malloc(sizeof(double) * nb_noeuds);
    for (int i = 0; i < nb_noeuds; i++ ){
        // Pour chaque commu (noeuds au départ) initialement sigma_tot[i] = k[i] (sommes des poids des arêtes incidentes au noeud de la communauté courante (donc le noeud au début) ce  qui est k[i])
        sigma_tot[i] = k[i]; 
    }


    // Initialisation de m qui est la somme de toutes les poids des arêtes du graphe
    double m = G -> poids_total;

    // Libération de la table de hachage après construction du graphe (on en a plus besoin pq le graphe G est construit)
    Noeud *actuel, *tmp;
    HASH_ITER(hh, tbl_noeud, actuel, tmp) {
        HASH_DEL(tbl_noeud, actuel);
        free(actuel);
    }

    int *communaute_final = malloc(sizeof(int)*nb_noeuds);
    for (int i = 0; i<nb_noeuds_og; i++){
        communaute_final[i] = i; // Chaque noeud = sa propre communauté
    }


    // BOUCLE PHASE 1 ET 2 

    while (1){ // boucle qui répète l'itération de la phase 1 et 2 

        // PHASE 1 : Parcourir tous les noeuds et calculer le gain de modularatié maximal (meilleur communauté) 

        int noeuds_bougent = 1; // valeur "boolenne" qui sert à dire "tant qu'il y a des noeuds qui se déplacent on continue" 

        while (noeuds_bougent) {

            noeuds_bougent = 0;

            // pour chaque noeud on va les retirer leur communauté courante et voir la meilleure commu voisine
            for (int i = 0; i < nb_noeuds ; i++){
                int communaute_noeud_i = communautes[i]; // j'enregistre la commu courante du noeud i

                retirer_noeud(i, communautes, G, sigma_in, sigma_tot, k); // retirer le noeud i de sa communauté courante
                // initialisation des variable qui définiront la meilleure communauté pour le noeud i et son deltaQ maximal
                int meilleure_commu = communaute_noeud_i;
                double meilleur_delta_Q = 0;

                // On regarde chacun des noeuds voisin (en particulier les communautés voisines)
                Voisin *voisin = G -> Tableau_Voisins[i]; 
                while (voisin != NULL){ // tant qu'il y a des voisins on regarde le communauté (c) respectif
                    int c = communautes[voisin -> id]; // c <- communauté dont le voisin courant de i appartient
                    if (c < 0){ // ca c'est si le voisin a été retiré de sa communauté dans ce cas -> next
                        voisin = voisin -> suivant;
                        continue;
                    }
                    
                    // calculer ki_in vers c (cad : pour chaque communauté (c) voisinant notre noeud i , on regarde (ENCORE) les voisins du noeud i pour calculer ki_In)
                    Voisin *voisin_c = G -> Tableau_Voisins[i];
                    double ki_in_c = 0; 
                    while (voisin_c != NULL){ // tant qu'il y a des voisins
                        if (communautes[voisin_c -> id] == c){ // si le voisin appartient à la communauté c bah on incrémente le poids apporté par ce voisin à ki_in
                            ki_in_c += voisin_c -> poids;
                        }
                        voisin_c = voisin_c -> suivant; // thank you next 
                    }

                    double delta_Q_c = delta_Q(sigma_in[c], sigma_tot[c], k[i], ki_in_c, m ); // calculer du delta_q
                    if (delta_Q_c > meilleur_delta_Q) {
                        meilleure_commu = c;
                        meilleur_delta_Q = delta_Q_c;
                        }

                    voisin = voisin -> suivant;
                    

                }
                
                rajouter_noeud(i, meilleure_commu, communautes, G, sigma_in, sigma_tot, k);
                if (meilleure_commu != communaute_noeud_i ) {
                    noeuds_bougent = 1;
                }

                
            }
            

        }
        

        
        // PHASE 2 : Agregation

        // On cherche le nombre de communautés distinctes qui ont été formées

        int *vu = calloc(nb_noeuds, sizeof(int));  // réserve mémoire + chaque octet à zéro

        // 2. Marquer les communautés présentes
        for (int i = 0; i < nb_noeuds; i++) {
            vu[communautes[i]] = 1;
        }

        // 3. Compter les communautés marquées
        int nbr_communaute = 0;
        for (int i = 0; i < nb_noeuds; i++) {
            if (vu[i] == 1) {
                nbr_communaute++;
            }
        }

        if (nb_noeuds == nbr_communaute) {
            break;
        }
        

        int *mapping = malloc(nb_noeuds* sizeof(int));
        for (int i = 0; i<nb_noeuds; i++){
            mapping[i] = -1;
        }

        int indice = 0;
        for (int i = 0; i < nb_noeuds; i++) {
            if (mapping[communautes[i]] == -1 ){
                mapping[communautes[i]] = indice;
                indice++;
            }
            
        }

        typedef struct {
            long long clef; // Combine comm_u et comm_v
            double poids;
            UT_hash_handle hh;
        }H_aretes;

        H_aretes *h_tableau = NULL;

        for (int i = 0; i < G ->nb_noeuds; i++){

            int comm_u = mapping[communautes[i]];
            Voisin *v = G->Tableau_Voisins[i];

            while (v!= NULL){
                H_aretes *h;

                int comm_v = mapping[communautes[v->id]];
                // On fait en sorte que c1 est <= à c2 pour éviter doublon
                int c1 = (comm_u < comm_v) ? comm_u : comm_v;
                int c2 = (comm_u < comm_v) ? comm_v : comm_u;
                
                long long clef = ((long long)c1 << 32) | (unsigned int)c2;
                HASH_FIND(hh,h_tableau,&clef,sizeof(long long),h);
                if (h == NULL){
                    h = malloc(sizeof(H_aretes));
                    h ->clef = clef;
                    h -> poids = 0;
                    HASH_ADD(hh, h_tableau, clef, sizeof(long long), h);
                }
                h-> poids += v -> poids;

                v = v->suivant; // on met à jour v pour qu'il pointe vers le voisin suivant
            }
        }

        for (int i = 0; i<nb_noeuds_og; i++){
            communaute_final[i] = mapping[communautes[communaute_final[i]]];

        }


        // On créer les arêtes du nouveau graphe
        
        Graphe *nouveau_graphe = initialiser_graphe(nbr_communaute,0);

        // Parcours de la table de hachage
        H_aretes *cur2,*tmp2;
        HASH_ITER(hh, h_tableau, cur2, tmp2){
            long long clef = cur2 -> clef;
            double poids = cur2 -> poids/2.0; // On rencontre le couple (u,v) deux fois donc on divise le poids trouvé par 2

            int a = (int)(clef >> 32);
            int b = (int)(clef & 0xFFFFFFFF);

            Ajouter_Aretes(nouveau_graphe, a, b, poids);
        }     

        // Libération de la mémoire pour passer à l'itération suivante

        H_aretes *actuel3 = NULL;
        H_aretes *tmp3 = NULL;
        HASH_ITER(hh, h_tableau, actuel3, tmp3) {
            HASH_DEL(h_tableau, actuel3);
            free(actuel3);
        }


        free_graphe(G);
        free(communautes);
        free(k);
        free(sigma_in);
        free(sigma_tot);
        free(mapping);
        free(vu);
        
        G = nouveau_graphe;
        nb_noeuds = nbr_communaute;



        //on crée a nouveau un tableau communauté où chaque super noeud = sa propre communauté
        communautes = malloc(sizeof(int)*nb_noeuds);
        for (int i = 0; i < nb_noeuds; i++){
            communautes[i] = i; // chaque noeud = sa propre communauté
        }

        k = malloc(sizeof(double)*nb_noeuds); // k
        for (int i = 0; i < nb_noeuds; i++) { 
            k[i] = 0; // initialisation à 0 

            Voisin *courant = G -> Tableau_Voisins[i]; // Initialisation du tableau des voisins dee chaque noeuds 
            while (courant != NULL){ // On parcourt tous les voisins du noeud i tant qu'il y a des voisins 
                k[i] += courant -> poids; // On incrémente la valeur ki avec le poids de l'arete (qui lie le voisin au noeud i) 
                courant = courant -> suivant; // on passe au voisin suivant (tant que qu'il y en a un)
            }
        }

        sigma_in = malloc(sizeof(double) * nb_noeuds);
        for (int i = 0; i < nb_noeuds; i++ ) {
            sigma_in[i] = 0; 
            
            // On parcourt les voisins pour voir s'il y a une boucle sur soi-même
            Voisin *courant = G->Tableau_Voisins[i];
            while (courant != NULL) {
                if (courant->id == i) { // Si le voisin est le nœud lui-même (self-loop)
                    sigma_in[i] += courant->poids;
                }
                courant = courant->suivant;
            }
        }

        
        // Calcul de sigma_tot = sommes des tous les poids des arêtes internes + incidente aux noeuds de la communauté courante
        sigma_tot = malloc(sizeof(double) * nb_noeuds);
        for (int i = 0; i < nb_noeuds; i++ ){
            // Pour chaque commu (noeuds au départ) initialement sigma_tot[i] = k[i] (sommes des poids des arêtes incidentes au noeud de la communauté courante (donc le noeud au début) ce  qui est k[i])
            sigma_tot[i] = k[i]; 
            }
        
        m = G -> poids_total;

    }

    double Q_final = calculer_modularite_finale(nb_noeuds, sigma_in, sigma_tot, m);

    printf("Modularite finale Q = %.6f\n", Q_final);

    FILE *f = fopen("communaute.txt", "w");
        if (f == NULL) {
            perror("Impossible d'ouvrir le fichier pour écriture");
            return -1;
        }
        for(int i = 0; i<nb_noeuds_og; i++){
            fprintf(f,"%d,%d\n", i, communaute_final[i]);
        }

        fclose(f);
}