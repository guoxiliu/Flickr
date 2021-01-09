# Community-based Friend Recommendation on Flick Dataset

This is the final project for CPSC 8480 Network Science, which uses `Flickr` dataset to do clustering and community-based recommendation. 

## Problem 

The connections in the social network are not uniformly distributed, which means that the network can be divided into several communities and there are dense connections in the same community but sparse connections between different communities. The project is to perform community detection with Flickr dataset. 

Apart from the clustering, new friend and group suggestion are also important in social networks. We are going to recommend some new groups for a specific user according to his/her friendship and some possible friends to a particular blogger based on the groups he/she has joined. 

## Methods

### Clustering

The modularity $Q$ of a network is defined as 

$$
Q = \frac{1}{2m} * \sum_{ij} [A_{i,j} - \frac{k_i * k_j}{2m} ] * \delta (C_i, C_j),
$$

where $m$ is the total weights of the edges in the network, $k_i$ and $k_j$ are the weights of edges incident to $i$ and $j$ respectively, $\delta (C_i, C_j)$ equals to 1 if $i$ and $j$ are in the same community, and 0 otherwise.

The modularity $Q$ measures the strength of division of a network into modules, and thus an intuitive way for community detection is to maximize this value. 

Clauset et al. proposed a fast greedy algorithm, that is, we merge two communities that can reach the maximum modularity in each iteration. Blondel et al. introduced a fast algorithm to maximize the modularity by use the concept of modularity gain $\Delta Q$; 

$$
\Delta Q = \frac{1}{2m}(k_{i,in} - \frac{k_i \sum_{tot}}{m}),
$$

where $k_{i,in}$ means the sum of the weights of the links from $i$ to nodes in $C$, and $\sum_{tot}$ denotes the sum of weights of the links incident to nodes in $C$. The algorithm is divided into two phases that are repeated iteratively. In the first phase, each node will be placed into the community of one of its neighbors if the gain is maximum.The second phase is to build a new network whose nodes are the communities found during the first phase.

Label Propagation was another way to detect community structure which was proposed in \cite{raghavan2007near}. In each iteration, we change the community of each node to its neighbor's community which has the maximum sum of edge weights, and stop until there is no change happen. 

### Recommendation

The practice of friend recommendation is based on the similarity. If two nodes are quite "similar", and there will be a high probability for them to become friends with each other. The similarity used in the project is defined as: 

$$
Similarity_{(i, j)} = \frac{n_{ij}}{\sqrt{d(i)d(j)}}, 
$$

where $n_{ij}$ denotes the number of common neighbors of node $i$ and $j$, $d(i)$ and $d(j)$ are degrees of node $i$ and $j$ respectively. The similarity is also known as cosine similarity.

Since there are dense connections in the same community, and thus the probability of becoming friends between two users who are in the same community tends to be higher.

### Measurements

The new graph is created by randomly removing about 5\% edges (about 295,000) from the original graph. This can be done by using `shuf -n NUM edges.txt > test-edges.txt` in the terminal, which means randomly select NUM lines from `edges.txt` and save them to `test-edges.txt`. 

The clustering algorithms are evaluated by their run time, reached modularity and number of clusters, whereas the accuracy of recommendation results is calculated as below: 

$$
Accuracy_{(R)} = \frac{\sum_{i\in N} \delta(R(i), M(i))}{|N|}, 
$$

where $N$ is the user list for friend recommendation, $R(i)$ is the recommendation list for user $i$, $M(i)$ is the missing actual friends of user $i$, and $\delta (A,B) = 1$ if set $A$ and $B$ have at least one common element, and $\delta (A,B) = 0$ otherwise.

## Poster
![](/figures/poster-flickr.jpg)

## Resources

- [Proposal](/docs/Proposal-Flickr.pdf) 
- [Final report](/docs/FinalReport-Flickr.pdf) 
