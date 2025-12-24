#include <bits/stdc++.h>
using namespace std;

struct Subzone {
    string zone;
    string name;
    double historical1;
    double historical2;
    double historical3;
    double currentPollution;
    double soilToxicity;
    double moisture;
    double heavyMetal;
    double landQuality;
    int index;
};

// Priority queue comparator
struct ComparePriority {
    bool operator()(Subzone const &a, Subzone const &b) {
        if(a.landQuality == b.landQuality)
            return a.currentPollution < b.currentPollution;
        return a.landQuality > b.landQuality;
    }
};

// Compute land quality
double computeLandQuality(double pollution, double soilTox, double heavyMetal){
    return 100 - (0.5*pollution + 0.3*soilTox + 0.2*heavyMetal);
}

// Sliding window average (3 historical + current)
double slidingWindowAverage(double h1, double h2, double h3, double current){
    return (h1 + h2 + h3 + current) / 4.0;
}

// Read CSV
vector<Subzone> readCSV(const string &filename){
    vector<Subzone> subzones;
    ifstream file(filename);
    string line;
    getline(file,line); // header
    int index=0;
    while(getline(file,line)){
        stringstream ss(line);
        Subzone sz;
        string temp;
        getline(ss, sz.zone, ',');
        getline(ss, sz.name, ',');
        ss >> sz.historical1; ss.ignore();
        ss >> sz.historical2; ss.ignore();
        ss >> sz.historical3; ss.ignore();
        ss >> sz.currentPollution; ss.ignore();
        ss >> sz.soilToxicity; ss.ignore();
        ss >> sz.moisture; ss.ignore();
        ss >> sz.heavyMetal; ss.ignore();
        sz.landQuality = computeLandQuality(sz.currentPollution, sz.soilToxicity, sz.heavyMetal);
        sz.index = index++;
        subzones.push_back(sz);
    }
    return subzones;
}

// Build priority queue
priority_queue<Subzone, vector<Subzone>, ComparePriority> buildPriorityQueue(vector<Subzone>& subzones){
    priority_queue<Subzone, vector<Subzone>, ComparePriority> pq;
    for(auto &sz: subzones) pq.push(sz);
    return pq;
}

// Dijkstra placeholder
vector<int> dijkstra(vector<vector<double>> &graph, int start){
    int n = graph.size();
    vector<double> dist(n,1e9);
    vector<int> parent(n,-1);
    set<pair<double,int>> s;
    dist[start]=0;
    s.insert({0,start});

    while(!s.empty()){
        int u = s.begin()->second;
        s.erase(s.begin());
        for(int v=0;v<n;v++){
            if(graph[u][v]>0 && dist[u]+graph[u][v]<dist[v]){
                s.erase({dist[v],v});
                dist[v]=dist[u]+graph[u][v];
                parent[v]=u;
                s.insert({dist[v],v});
            }
        }
    }
    return parent;
}

int main(){
    vector<Subzone> subzones = readCSV("land_pol_data.csv");

    // Sliding Window Trend
    cout << "--- Sliding Window Trend Analysis ---\n";
    for(auto &sz: subzones){
        double avg = slidingWindowAverage(sz.historical1, sz.historical2, sz.historical3, sz.currentPollution);
        cout << sz.zone << " - " << sz.name 
             << " | Avg Pollution: " << avg;
        if(avg>50) cout << " ⚠️ Unsafe Zone";
        cout << endl;
    }

    // Priority Queue for Waste Collection
    auto pq = buildPriorityQueue(subzones);
    cout << "\n--- Waste Collection Priority ---\n";
    while(!pq.empty()){
        auto sz = pq.top(); pq.pop();
        cout << sz.zone << " - " << sz.name
             << " | Land Quality: " << sz.landQuality
             << " | Current Pollution: " << sz.currentPollution << endl;
    }

    // Dijkstra placeholder
    int n = subzones.size();
    vector<vector<double>> graph(n, vector<double>(n,1)); 
    for(int i=0;i<n;i++) graph[i][i]=0;
    auto parent = dijkstra(graph,0);
    cout << "\n--- Shortest Paths from Depot ---\n";
    for(int i=0;i<n;i++){
        cout << subzones[i].name << " <- ";
        int cur=i;
        while(parent[cur]!=-1){
            cout << subzones[parent[cur]].name << " <- ";
            cur = parent[cur];
        }
        cout << "Depot\n";
    }

    return 0;
}
