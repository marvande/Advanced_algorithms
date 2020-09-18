#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;


void solve(){
    int n, m;
    cin >> n >> m;

    auto comparator = [](const auto left, const auto right){
        if(left.second.size() == right.second.size())
            return left.first < right.first;
        return left.second.size() < right.second.size();
    };
    vector<pair<int, set<int>>> collection;

    for(int i = 0; i < m; ++i){
        int sz;
        cin >> sz;
        set<int> subset;
        for(int j = 0; j < sz; ++j){
            int a;
            cin >> a;
            subset.insert(a);
        }
        collection.push_back({i + 1, subset});
    }
    sort(collection.begin(), collection.end(), comparator);


    vector<int> solution;
    vector<int> inv_price(n + 1, 0);

    int cover_size = 0;
    while(cover_size < n){
        auto argmax = *collection.rbegin();
        collection.pop_back();

        // Add the index to the solution vector
        solution.push_back(argmax.first);
        cover_size += argmax.second.size();

        for(auto el: argmax.second){
            inv_price[el] = argmax.second.size();
            for(auto &subset: collection){
                if(subset.second.find(el) != subset.second.end()){
                    subset.second.erase(el);
                }
            }
        }
        sort(collection.begin(), collection.end(), comparator);
    }

    double Hn = 0;
    for(int i = 1; i <= n; ++i){
        Hn += 1.0L / i;
    }

    cout << solution.size() << "\n";
    for(auto el: solution){
        cout << el << " ";
    }
    cout << "\n";
    for(int i = 1; i <= n; ++i){
        cout << (1.0L / inv_price[i])/Hn << " ";
    }


}

int main(){
    std::cin.tie(0);
    std::ios_base::sync_with_stdio(0);
    std::cout << std::setprecision(16) << fixed;

    solve();

    return 0;
}
