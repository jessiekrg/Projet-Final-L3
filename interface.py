import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import sys

fichier_entree = sys.argv[1]

# Charger le graphe + garder l'ordre d'insertion des nœuds
G = nx.Graph()
with open(fichier_entree, "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        u, v = line.split()
        G.add_edge(str(u), str(v))

# Créer un mapping indice -> nom de nœud
noeuds = list(G.nodes())
index_to_node = {str(i): noeuds[i] for i in range(len(noeuds))}

# Charger les communautés avec le bon mapping
communautes = {}
with open("communaute.txt", "r") as f:
    modularite = f.readline()
    for line in f:
        line = line.strip()
        if not line:
            continue
        idx, comm = line.split(',')
        node_name = index_to_node.get(idx.strip())
        if node_name:
            communautes[node_name] = int(comm.strip())


print(list(G.nodes())[:5])
print(list(communautes.keys())[:5])

for node in G.nodes():
    G.nodes[node]['group'] = communautes.get(node, 0)

# Couleurs par communauté
groupes = [G.nodes[n]['group'] for n in G.nodes()]
couleurs = [cm.tab20(g % 20) for g in groupes]

# Affichage
plt.figure(figsize=(12, 8))
pos = nx.spring_layout(G, seed=42)
nx.draw(G, pos, node_color=couleurs, node_size=30, edge_color="#aaaaaa", linewidths=0.3, with_labels=False)
plt.title("Graphe - Communautés Louvain")
plt.figtext(0.02, 0.02, 
    f"Nœuds : {G.number_of_nodes()}\nArêtes : {G.number_of_edges()}\nCommunautés : {len(set(communautes.values()))}\nModularité : {modularite}",
    fontsize=9, verticalalignment="bottom",
    bbox=dict(facecolor="white", alpha=0.7, boxstyle="round"))
plt.tight_layout()
plt.savefig("graphe.png", dpi=150)
plt.show()
