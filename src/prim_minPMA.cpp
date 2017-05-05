
/*
    This program runs Parallel version of Prim's algorithm
    using minPMA technique.
*/

/*

    Algorithm : minPMA algorithm
    Input: G(V, E) : An undirected weighted graph;
    Q : the list of vertices in the current sub-tree;
    Output: The lightest edge e(u, v) such that u ∈ Q and v Not ∈ Q
    1 minW = ∞ ;
    2 for Each u in Q do
    3   for Each edge e(u, v) of u do
    4       if successor[u] != successor[v] and
    e.          weight < minW then
    5               minE = e;
    6               minW = e.weight;
    7 Return minE;

*/

///include libraries
#include<iostream>
#include<set>
#include<vector>
#include<queue>
#include<cstdio>
#include<sys/stat.h>
#include<unistd.h>
#include<string>
#include<omp.h>
#include<unordered_map>

using namespace std;

///define infinite
#define INF 1e9

int THREADS = 2;
long long MAX_ALLOWED_THREADS = 2 ;


///Edge structure
typedef struct _ {

    int u;
    int v;
    int w;
} Edge;

///Graph Parameters
vector<Edge> edges; ///list of edges of graph
vector<vector<int> > adj; ///adjacency list of graph


///Sub Tree Parameters
vector<vector<int> > sub_tree_info; ///store sub tree information during
vector<int> sub_tree_root;  ///root of each subtree
set<int>child;              ///information about becoming child from root of its own subtree


///DSU Parameters
vector<int> root;
vector<int> size;


///DFS Paramenter
vector<bool> visited;   ///mark visited or not
unordered_map<int,int> mp;
unordered_map<int,int> rmp;

///prepare_sub_tree : initialize sub tree parameters
void prepare_sub_tree(int V){

    sub_tree_root = vector<int> (V);

    for(int i = 0; i < V; i++){

        sub_tree_root[i] = i;
    }

    sub_tree_info = vector<vector<int> >(V);

}

///prepare_dsu : initialize dsu parameters
void prepare_dsu(int V){

    root = vector<int>(V);
    size = vector<int>(V);

    for(int i=0;i<V;i++){
        root[i] = i;
        size[i] = i;
    }
}

///__find : get root of u's subtree
int __find(int u){

    ///distance doubling technique
    while( u != root[root[u]] ){

        u = root[root[u]];
    }
    return u;
}

///__union : unifies subtree using edge e
int __union(int e){

    int root_u = __find(mp[edges[e].u]);
    int root_v = __find(mp[edges[e].v]);


    if( root_u != root_v ){ ///edge connecting two subtrees

        if(size[root_u]>size[root_v]){

            ///update dsu info
            root[root_v] = root_u;
            size[root_u] += size[root_v];

            ///update subtree info
            child.insert(root_v);
            sub_tree_info[root_u].push_back(root_v);

        }else{

            ///update dsu info
            root[root_u] = root_v;
            size[root_v] += size[root_u];

            ///update subtree info
            child.insert(root_u);
            sub_tree_info[root_v].push_back(root_u);
        }

        return edges[e].w;

    }else{      ///edge inside subtree only

        return 0;
    }

}

int minPMA(int root_Q){

    int minW = INF;       ///minW :  minimum weight edge
    int minE;               ///minE : id of minW weight edge

    queue<int> Q;           ///Q : keep track of vertices of current subtree which are not processed
    Q.push(root_Q);         ///push root vertex


    while(!Q.empty()){

        int u = Q.front();     ///pick one vertex of subtree
        Q.pop();


        for(int i=0;i<adj[rmp[u]].size();i++){   ///pick all adjacent edges of u

            int e = adj[rmp[u]][i];

            if( __find(mp[edges[e].u]) != __find(mp[edges[e].v])){   ///edge connecting two subtrees

                if(edges[e].w < minW){  ///found lesser weight edge

                    minW = edges[e].w;
                    minE = e;
                }
            }
        }

        ///push child of u into Q which are part of this sub tree only
        for(int i=0;i<sub_tree_info[u].size();i++){

            Q.push(sub_tree_info[u][i]);
        }

    }

    return minE;
}


long long prim_minPMA(int V){

    vector<int> minE(V);    ///minE : list of min weight edge for each subtree
    long long ans = 0;      ///weight of MST

    while(sub_tree_root.size()>1){  /// more than one tree

        THREADS =  min((long long)sub_tree_root.size(),(long long)MAX_ALLOWED_THREADS);
        omp_set_num_threads(THREADS);

        #pragma omp parallel
        {
            #pragma omp for
            ///find min weight edge of each subtree
            for(int i=0;i<sub_tree_root.size();i++){
                minE[i] = minPMA(sub_tree_root[i]);
            }
        }
        child.clear();  ///clear child set

        /// Add edges to graph inorder to get MST
        for(int i=0;i<sub_tree_root.size();i++){

            ans += __union(minE[i]);
        }

        /// Some of subtree are merged, so remove them from sub_tree_root
        vector<int> new_sub_tree_root;

        for(int i=0;i<sub_tree_root.size();i++){

            if(child.find(sub_tree_root[i])==child.end()){

                new_sub_tree_root.push_back(sub_tree_root[i]);
            }
        }

        ///update sub tree root to further processing
        sub_tree_root = new_sub_tree_root;
    }
    return ans;
}


///fexists : check whether file exist or not

inline bool fexists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

///comparision_info : add info for comparision over several algorithm execution time
void comparision_info(double exe_time){

        freopen("../output/time.txt","a+",stdout);
        printf("%.6f ",exe_time);
}


void dfs(int u,vector<int> &vertices){

    vertices.push_back(u);
    visited[u] = true;

    for(int i=0;i<adj[u].size();i++){

        int e = adj[u][i];
        int next;
        if(edges[e].u==u){

            next = edges[e].v;
        }else{

            next = edges[e].u;
        }
        if(visited[next]==false){
            ///recure for this vertex
            dfs(next,vertices);
        }
    }
}

int main(int argc,char** argv){


    if(argc>1){
        THREADS = atoi(argv[1]);
    }

    clock_t runtime = clock();  ///run timer

    freopen("../input/simple_graph.txt","r",stdin); ///read input

    int V,E;    /// G = (V,E)
    scanf("%d %d",&V,&E);


    edges = vector<Edge>(E);
    adj = vector<vector<int> >(V);

    int u,v,w;  ///edge endpoint and weight

    ///reading edges of graph
    for(int i=0;i<E;i++){

        int u,v,w;
        cin >> u >> v >> w;
        ///store edge info
        edges[i] = {u,v,w};

        ///build adjacency list
        adj[u].push_back(i);
        adj[v].push_back(i);

    }


    long long ans = 0;

    /// identify  components
    /// solve for component as connected sub graph
    visited = vector<bool> (V,false);
    for(int j=0;j<V;j++){
        if(visited[j]==false){      ///identified new component

            vector<int> vertices;
            dfs(j,vertices);

            prepare_sub_tree(vertices.size());       /// initialization of subtree parameter
            prepare_dsu(vertices.size());            /// initialization of dsu parameter

            ///rename vertex number
            ///to make independent subgraph
            mp.clear();
            rmp.clear();

            for(int i=0;i<vertices.size();i++){

                mp[vertices[i]]=i;
                rmp[i] = vertices[i];
            }
            ans += prim_minPMA(vertices.size());

        }
    }

    printf("MST Weight : %lld\n",ans);

    runtime = clock()-runtime;  ///keep track of runtime of minPMA
    double exe_time = double(runtime)/CLOCKS_PER_SEC;
    printf("Runtime : %.6f\n",exe_time);

    comparision_info(exe_time); ///write information for comparision

    freopen("../output/prim_minPMA.txt","w",stdout);    ///write output to file
    printf("MST Weight : %lld\n",ans);

    return 0;
}
