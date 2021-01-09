# Community-based Friend Recommendation on Flick Dataset

This is the final project for CPSC 8480 Network Science, which uses `Flickr` dataset to do clustering and community-based recommendation. 

## Problem 

The connections in the social network are not uniformly distributed, which means that the network can be divided into several communities and there are dense connections in the same community but sparse connections between different communities. The project is to perform community detection with Flickr dataset. 

Apart from the clustering, new friend and group suggestion are also important in social networks. We are going to recommend some new groups for a specific user according to his/her friendship and some possible friends to a particular blogger based on the groups he/she has joined. 

## Methods

### Clustering

The modularity <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/1afcdb0f704394b16fe85fb40c45ca7a.svg?invert_in_darkmode" align=middle width=12.99542474999999pt height=22.465723500000017pt/> of a network is defined as 

<p align="center"><img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/3997d9e4b21e5393960ef95f24e5b2ce.svg?invert_in_darkmode" align=middle width=290.80627125pt height=44.1686784pt/></p>

where <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode" align=middle width=14.433101099999991pt height=14.15524440000002pt/> is the total weights of the edges in the network, <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/ec71f47b6aee7b3cd545386b93601915.svg?invert_in_darkmode" align=middle width=13.20877634999999pt height=22.831056599999986pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/75c9e9a3fab35d4c380586a9713adfe4.svg?invert_in_darkmode" align=middle width=14.66238509999999pt height=22.831056599999986pt/> are the weights of edges incident to <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/> respectively, <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/f0b31121e57337fb4dd28e9b3860631c.svg?invert_in_darkmode" align=middle width=63.91628759999998pt height=24.65753399999998pt/> equals to 1 if <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/> are in the same community, and 0 otherwise.

The modularity <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/1afcdb0f704394b16fe85fb40c45ca7a.svg?invert_in_darkmode" align=middle width=12.99542474999999pt height=22.465723500000017pt/> measures the strength of division of a network into modules, and thus an intuitive way for community detection is to maximize this value. 

Clauset et al. proposed a fast greedy algorithm, that is, we merge two communities that can reach the maximum modularity in each iteration. Blondel et al. introduced a fast algorithm to maximize the modularity by use the concept of modularity gain <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/f5d905617c33ccf73739f0535559628a.svg?invert_in_darkmode" align=middle width=26.69409599999999pt height=22.465723500000017pt/>; 

<p align="center"><img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/17929e5581b9ec8f59e3d01f9f551e60.svg?invert_in_darkmode" align=middle width=198.67297065pt height=34.94718315pt/></p>

where <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/8ef8b691d9394b80af081d58a34e2b0a.svg?invert_in_darkmode" align=middle width=29.889830849999992pt height=22.831056599999986pt/> means the sum of the weights of the links from <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> to nodes in <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/9b325b9e31e85137d1de765f43c0f8bc.svg?invert_in_darkmode" align=middle width=12.92464304999999pt height=22.465723500000017pt/>, and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/73d75c668ca635fbad68eb94738a8c49.svg?invert_in_darkmode" align=middle width=33.77183864999999pt height=24.657735299999988pt/> denotes the sum of weights of the links incident to nodes in <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/9b325b9e31e85137d1de765f43c0f8bc.svg?invert_in_darkmode" align=middle width=12.92464304999999pt height=22.465723500000017pt/>. The algorithm is divided into two phases that are repeated iteratively. In the first phase, each node will be placed into the community of one of its neighbors if the gain is maximum.The second phase is to build a new network whose nodes are the communities found during the first phase.

Label Propagation was another way to detect community structure which was proposed in \cite{raghavan2007near}. In each iteration, we change the community of each node to its neighbor's community which has the maximum sum of edge weights, and stop until there is no change happen. 

### Recommendation

The practice of friend recommendation is based on the similarity. If two nodes are quite "similar", and there will be a high probability for them to become friends with each other. The similarity used in the project is defined as: 

<p align="center"><img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/cb1bcc0b1153d884be55f7e291457047.svg?invert_in_darkmode" align=middle width=206.9153493pt height=36.773649pt/></p>

where <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/894246f28d69aaede1ebed600795650c.svg?invert_in_darkmode" align=middle width=20.62228409999999pt height=14.15524440000002pt/> denotes the number of common neighbors of node <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/>, <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/b419f33ea0fd1898ba18c255f0cb10b5.svg?invert_in_darkmode" align=middle width=27.00462269999999pt height=24.65753399999998pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/ded29e26d51d8a55e22d7e206f70ca52.svg?invert_in_darkmode" align=middle width=29.051813999999986pt height=24.65753399999998pt/> are degrees of node <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode" align=middle width=7.710416999999989pt height=21.68300969999999pt/> respectively. The similarity is also known as cosine similarity.

Since there are dense connections in the same community, and thus the probability of becoming friends between two users who are in the same community tends to be higher.

### Measurements

The new graph is created by randomly removing about 5\% edges (about 295,000) from the original graph. This can be done by using `shuf -n NUM edges.txt > test-edges.txt` in the terminal, which means randomly select NUM lines from `edges.txt` and save them to `test-edges.txt`. 

The clustering algorithms are evaluated by their run time, reached modularity and number of clusters, whereas the accuracy of recommendation results is calculated as below: 

<p align="center"><img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/ef1e02a143eabe1eb2cc05fa4adcf335.svg?invert_in_darkmode" align=middle width=260.52237255pt height=39.9275514pt/></p>

where <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> is the user list for friend recommendation, <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/899fa4252595caea42785ee116815313.svg?invert_in_darkmode" align=middle width=31.05712994999999pt height=24.65753399999998pt/> is the recommendation list for user <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/>, <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/632842e4ee03326a5969dfaa1eb34007.svg?invert_in_darkmode" align=middle width=36.18837584999999pt height=24.65753399999998pt/> is the missing actual friends of user <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode" align=middle width=5.663225699999989pt height=21.68300969999999pt/>, and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/37c71931cd0c39fd09781a03575800e7.svg?invert_in_darkmode" align=middle width=83.77842989999998pt height=24.65753399999998pt/> if set <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/53d147e7f3fe6e47ee05b88b166bd3f6.svg?invert_in_darkmode" align=middle width=12.32879834999999pt height=22.465723500000017pt/> and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/61e84f854bc6258d4108d08d4c4a0852.svg?invert_in_darkmode" align=middle width=13.29340979999999pt height=22.465723500000017pt/> have at least one common element, and <img src="https://rawgit.com/guoxiliu/Flickr/master/svgs/6fca3a6fc43a183c0630343d6a235827.svg?invert_in_darkmode" align=middle width=83.77842989999998pt height=24.65753399999998pt/> otherwise.

## Poster
![](/figures/poster-flickr.jpg)

## Resources

- [Proposal](/docs/Proposal-Flickr.pdf) 
- [Final report](/docs/FinalReport-Flickr.pdf) 
