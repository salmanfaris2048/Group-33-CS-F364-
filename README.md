# Group-33 CS F364
Design and Analysis of Algorithms
CS F364

DESIGN AND ANALYSIS OF ALGORITHMS

ASSIGNMENT - 1

GROUP NO. 33




GROUP MEMBERS:


NAME
ID NUMBER
EMAIL ID
Adithya Kothimeer
2021B5A72527H
f20212527@hyderabad.bits-pilani.ac.in
Deepa N
2021B4A73132H
f20213132@hyderabad.bits-pilani.ac.in
Salman Faris
2021B5A70745H
f20210745@hyderabad.bits-pilani.ac.in
Siddharth Yohan Ceri
2021B4A73145H
f20213145@hyderabad.bits-pilani.ac.in
Sriya Gudipati
2021B4A73151H
f20213151@hyderabad.bits-pilani.ac.in


















INDIVIDUAL CONTRIBUTIONS



Adithya Kothimeer : 

Worked on ELS and Chiba algorithm and Github page

Deepa N : 

Worked on Tomita algorithm, Report and Github page

Salman Faris : 

Worked on ELS and Chiba algorithm and Github page

Siddharth Yohan Ceri : 

Worked on ELS and Chiba algorithm and Github page

Sriya Gudipati : 

Worked on Tomita algorithm, Report and Github page



















EXPLANATION OF ALGORITHMS


The worst-case time complexity for generating all maximal cliques and computational experiments
Etsuji Tomitaa, Akira Tanakaa, Haruhisa Takahashia
This algorithm is a depth-first search (DFS) based approach for efficiently finding all maximal cliques in a given graph G=(V,E). It maintains a global set Q that represents the current clique being expanded, starting with an empty set and recursively adding vertices to form larger cliques. The expansion process is carried out by the EXPAND procedure, which explores candidate vertices in a structured manner to ensure that each clique found is maximal. At each step, the algorithm considers the set SUBG, which consists of vertices that are adjacent to all elements of Q. If SUBG is empty, Q is declared a maximal clique and output. Otherwise, for each vertex q∈SUBGq , the algorithm expands Q to Q∪{q} and defines a new subgraph SUBG_q, which consists of vertices in SUBG that are also adjacent to q. This recursive process continues until all maximal cliques are discovered. To avoid duplication, the algorithm partitions SUBG into two subsets: FINI, containing already processed vertices, and CAND, containing the remaining vertices available for expansion. A pivot selection strategy is employed, choosing a vertex u that maximizes ∣CAND∩N(u)∣ ( N(u) represents neighborhood of the vertex u) , which effectively reduces redundant branches and minimizes the search space. Instead of printing full cliques, the algorithm outputs minimal information such as the words "clique" when a maximal clique is found, along with "back" for backtracking, allowing for an efficient tree representation of cliques. With these optimizations, the Tomita algorithm achieves a worst-case complexity of O(3^{n/3}) and improves upon traditional approaches like the Bron–Kerbosch algorithm by introducing structured candidate selection and pruning mechanisms, ensuring that maximal cliques are generated without duplication.

Listing All Maximal Cliques in Sparse Graphs in Near-optimal Time 
David Eppstein, Maarten Loffler, and Darren Strash
The Bron-Kerbosch algorithm with Tomita et al.'s pivoting strategy is an efficient method for finding all maximal cliques in a graph without duplication. This algorithm is particularly effective for sparse graphs, where there are relatively few edges compared to the number of nodes. The process begins by arranging the nodes in degeneracy order, meaning the node with the fewest remaining connections is chosen first, which helps improve efficiency. For each node vi  in this order, the algorithm attempts to find all maximal cliques that include vi . It defines two sets: P (candidate nodes for expansion), which includes neighbors of vi  that appear later in the order, and X (already processed nodes), which contains neighbors that have already been considered to prevent duplication. The function BronKerboschPivot(P, {vi }, X) is then used to recursively expand cliques, but instead of checking every node, it selects a pivot node to minimize unnecessary recursive calls, making the algorithm more efficient. This strategy ensures that each clique is counted only once by restricting expansion to nodes in PP while excluding those in X. By combining degeneracy ordering and pivoting, this method significantly reduces computational complexity, making it one of the fastest algorithms for finding maximal cliques in large graphs.
ARBORICITY AND SUBGRAPH LISTING ALGORITHMS
 NORISHIGE CHIBA AND TAKAO NISHIZEKI
The CLIQUE algorithm by Chiba and Nishizeki is designed for efficiently listing all cliques in a graph by leveraging the concept of arboricity, which relates to the density of graph substructures. The algorithm begins by iterating through each vertex and identifying potential cliques by considering its neighboring vertices. It uses an ordered adjacency list, ensuring that vertices are processed in a specific order to avoid redundant checks. The core idea is to prune the search space by focusing only on subsets of neighbors that maintain clique properties, effectively reducing computational complexity. The algorithm first updates the candidate set (C) by removing the currently considered vertex and prepares for recursive expansion. It then processes neighbors of the current vertex, forming subsets of potential cliques. A key step involves checking for lexicographical ordering, ensuring each clique is discovered only once. The algorithm iterates through these subsets, verifying clique conditions and updating the list accordingly. If a clique is found, it is stored and reported, and the process continues recursively until all possibilities are exhausted. By efficiently handling graph sparsity and using neighborhood-based pruning, the CLIQUE algorithm significantly optimizes clique listing, making it well-suited for applications involving large sparse graphs such as social networks or bioinformatics.













COMPARISON OF THE ALGORITHMS


TOMITA ALGORITHM:

Has been implemented in C




Largest size of the clique
Total number of maximal cliques
Execution time
Wiki-Vote.txt
17
41691741
127 seconds 
Email-Enron.txt
20
107034778
218 seconds
as-skitter.txt




Greater than 4 hours




CHIBA ALGORITHM:

Has been implemented in C++




Largest size of the clique
Total number of maximal cliques
Execution time
Wiki-Vote.txt
17
41691741
132 seconds
Email-Enron.txt
20
107034778
574 seconds
as-skitter.txt




Greater than 5 hours




ELS ALGORITHM:

Has been implemented in C++




Largest size of the clique
Total number of maximal cliques
Execution time
Wiki-Vote.txt
17
41691741
282 seconds
Email-Enron.txt
20
107034778
651 seconds
as-skitter.txt




Greater than 5 hours



Skitter Data Set : In this data set, we faced an issue or runtime. The dataset is so complex that the codes did not terminate even after many hours, and even caused our systems to crash. We also attempted to run the codes on google colab hoping the higher computation power and memory availability would give better results. However we faced the same issues as even google colab ran out of memory space provided. We have included these findings in our observations.























GRAPH FOR THE DATASETS


Wiki-Vote.txt:






Email-Enron.txt:
