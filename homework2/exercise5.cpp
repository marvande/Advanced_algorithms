#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <iomanip>
#include <random>
#include <complex>
#include <chrono>
#include <assert.h>

using namespace std;

struct Edge{
    int x;
    int y;
    int z;
    int rank;
};

unsigned get_seed(){
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    return seed;
}

// Use the Mersene Twister random number generator.
mt19937 rng;

struct HyperGraph{
    vector<Edge> E;
    vector<int> parent;
    vector<int> size;
    int N;
    int min_cut = 0;
    int compressed_hash = 0;

    // Union-find data structure
    void unite(int x, int y){
        // Small-to-big trick
        if(size[x] > size[y]){
            parent[y] = x;
            size[x] += size[y];
        }else{
            parent[x] = y;
            size[y] += size[x];
        }
    }

    int u_find(int x){
        int R, y;

        for(R = x; parent[R] != R; R = parent[R]);

        // Path compression
        for(; parent[x] != x;){

            y = parent[x];
            parent[x] = R;
            x = y;
        }

        return R;
    }

    // If set is set then we don't remove at random, but the edge at position set.
    void contract(int set = -1){
        Edge elim;
        // Remove self-edges
        E.erase(remove_if(E.begin(), E.end(), [this](Edge e){
            return (u_find(e.x) == u_find(e.y) && u_find(e.y) == u_find(e.z));
        }), E.end());

        if(set == -1)
            elim = E[(((rand()) % E.size()) + E.size()) % E.size()];
        else
            elim = E[set];

        E.erase(remove_if(E.begin(), E.end(), [this, elim](Edge e){
            return (e.rank == elim.rank);
        }), E.end());

        // Contracting an {x, y, z} edge is similar to uniting x - y and y - z.
        if(u_find(elim.x) != u_find(elim.y)){
            unite(u_find(elim.x), u_find(elim.y));
            N--;
        }
        if(u_find(elim.y) != u_find(elim.z)){
            unite(u_find(elim.y), u_find(elim.z));
            N--;
        }
    }

    void reduce(){
        // Add the spanning edges to the min-cut:
        for(auto e: E){
            if(((u_find(e.x) != u_find(e.y)) + (u_find(e.y) != u_find(e.z)) + (u_find(e.x) != u_find(e.z))) == N){
                compressed_hash = compressed_hash ^ e.rank;
                min_cut++;
            }
        }

        // Delete self-hyperedges and spanning edges.
        E.erase(remove_if(E.begin(), E.end(), [this](Edge e){
            return ((u_find(e.x) == u_find(e.y) && u_find(e.y) == u_find(e.z)) ||
            (((u_find(e.x) != u_find(e.y)) + (u_find(e.y) != u_find(e.z)) + (u_find(e.x) != u_find(e.z))) == N));
        }), E.end());
    }
};

int min_cut_global;
set<int> hashes;

void consider_mincut(pair<int, int> el){
    if(el.first < min_cut_global){
        // If found a better mincut,
        // wipe the solutions found hitherto.
        min_cut_global = el.first;
        hashes.clear();
    }

    if(el.first == min_cut_global){
        hashes.insert(el.second);
    }
}

void KargerStein(HyperGraph G){

    // Small case optimizations

    if(G.N <= 3)
        G.reduce();

    if(G.N == 2 || !G.E.size()){
        consider_mincut({G.min_cut, G.compressed_hash});
        return;
    }


    // Small graph optimization
    if(G.N == 3){
        for(int i = 0; i < G.E.size(); ++i){
            HyperGraph G1 = G;
            G1.contract(i);
            KargerStein(G1);
        }
        return;
    }
    assert(G.N != 1);

    int n = G.N;
    // Branching probability
    double p = 1.0L * 3 / n;

    uniform_real_distribution<> uniform_zero_to_one(0, 1.0);

    // Copy initialization:
    HyperGraph G1 = G;
    G1.contract();

    if(uniform_zero_to_one(rng) < p){
        // Branch with probability p
        KargerStein(G1);
        KargerStein(G);
    }else{
        KargerStein(G1);
    }

}

void solve(){
    int n, m;
    cin >> n >> m;
    HyperGraph G;
    G.N = n;
    G.min_cut = 0;
    G.size = G.parent = vector<int>(n + 1, 0);
    for(int i = 1; i <= n; ++i){
        G.parent[i] = i;
        G.size[i] = 1;
    }

    clock_t begin = clock();
    clock_t end = clock();

    for(int i = 1; i <= m; ++i){
        int a, b, c;
        cin >> a >> b >> c;

        // Random value for hashing the cut.
        G.E.push_back({a, b, c, (int) rng()});
    }

    min_cut_global = m;

    while((double(end - begin) / CLOCKS_PER_SEC) < 2){
        // If there is time to be spared,
        // run the algorithm again.
        KargerStein(G);

        end = clock();
    }

    cout << min_cut_global << " " << hashes.size() << "\n";
}

int main() {
    std::ios_base::sync_with_stdio(0);
    cin.tie(0);
    int T = 1;

    rng = mt19937{get_seed()};

    solve();


    return 0;
}
