import networkx as nx
import matplotlib.pyplot as plt
import community.community_louvain as community_louvain

G = nx.read_edgelist("ChCh-Miner_durgbank-chem-chem.txt", comments="#")

partition = community_louvain.best_partition(G)

modularity = community_louvain.modularity(partition, G)
num_communities = len(set(partition.values()))

groupes = [partition[node] for node in G.nodes()]

plt.figure(figsize=(14, 10))

pos = nx.spring_layout(G, seed=42)

# Dessin des nœuds et arêtes
nx.draw_networkx_nodes(G, pos, node_size=20, node_color=groupes, cmap=plt.cm.nipy_spectral)
nx.draw_networkx_edges(G, pos, alpha=0.03, edge_color="gray")

stats_text = (f"Nœuds : {G.number_of_nodes()}\n"
              f"Arêtes : {G.number_of_edges()}\n"
              f"Communautés : {num_communities}\n"
              f"Modularité : {modularity:.6f}")

plt.figtext(0.02, 0.02, stats_text, fontsize=12, bbox=dict(facecolor='white', alpha=0.8, boxstyle='round'))

plt.title("Graphe Drug-Gene : Analyse de Communautés (Louvain)", fontsize=16)
plt.axis('off')
plt.tight_layout()

# Sauvegarde et affichage
plt.savefig("graphe_louvain_python.png", dpi=300)
plt.show()