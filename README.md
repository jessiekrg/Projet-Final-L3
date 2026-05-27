# Projet-Final-L3

Binôme : 
Lina Berroug
Jessica Karega

Présentation du projet : 
Ce projet de fin de L3 porte sur l'analyse de réseaux complexes, et plus particulièrement sur la **détection de communautés au sein de réseaux biologiques**.

**Code Source :**
* `loader.c` : Programme principal en C. 
* `interface.py` : Script Python permettant de générer une visualisation graphique des réseaux analysés.

**Réseaux Biologiques & Chimiques (Graphes d'entrée) :**
* `bio-yeast-protein-inter.txt` : Réseau d'interactions protéines-protéines chez la levure.
* `ChCh-Miner_durgbank-chem-chem.txt` : Réseau d'interactions entre composés chimiques/médicaments.
* `ChG-Miner_miner-chem-gene.txt` : Réseau biparti d'interactions entre composés chimiques et gènes.
* `bio-CE-LC.txt` / `bio-DM-LC.txt` : Autres réseaux biologiques étudiés.

**Résultats & Visualisation :**
* `communaute.txt` : Fichier de sortie généré par `loader.c`, contenant la modularité finale et l'association entre chaque nœud et son ID de communauté.


**Article :**
* `Community discovery in Biological networks.pdf` : L'article scientifique principal ayant servi pour la comparaison de résulat.


**Pour l'exécution de Python :**
* NetworkX (`pip3 install networkx`)
* Matplotlib (`pip3 install matplotlib`)

**Commandes :**
make compile : Compilation du programme principal en C pour générer l'exécutable
make chch : Exécution et visualisation sur le réseau de médicaments (DrugBank)
make chg : Exécution et visualisation sur le réseau biparti médicaments-gènes
make yeast : Exécution et visualisation sur le réseau de la levure (protéines-protéines)
make celc : Exécution et visualisation sur le réseau biologique CE-LC
make dmlc : Exécution et visualisation sur le réseau biologique DM-LC
make all : Exécution et visualisation séquentielles sur l'ensemble des réseaux
make clean : Suppression des fichiers générés 
make zip : Création de l'archive compressée pour le rendu final