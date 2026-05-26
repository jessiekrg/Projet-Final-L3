all: compile chch chg yeast celc dmlc

compile:
	gcc -o louvain loader.c

chch: compile
	./louvain ChCh-Miner_durgbank-chem-chem.txt
	python3 interface.py ChCh-Miner_durgbank-chem-chem.txt

chg: compile
	./louvain ChG-Miner_miner-chem-gene.txt
	python3 interface.py ChG-Miner_miner-chem-gene.txt

yeast: compile
	./louvain bio-yeast-protein-inter.txt
	python3 interface.py bio-yeast-protein-inter.txt

celc: compile
	./louvain bio-CE-LC.txt
	python3 interface.py bio-CE-LC.txt

dmlc: compile
	./louvain bio-DM-LC.txt
	python3 interface.py bio-DM-LC.txt

clean:
	rm -f louvain communaute.txt graphe.png

zip:
	zip -r Louvain_BERROUG_Lina-KAREGA_Jessica.zip . -x "__pycache__/*" "*.git*" ".DS_Store" "*.o"

.PHONY: all compile chch chg yeast celc dmlc clean zip