#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
using namespace std;

/* ===================== CONSTANTS ===================== */
#define FLOOD_THRESHOLD 80
#define SAFE_POLLUTION 50

/* ===================== DATA STRUCTURES ===================== */

struct SubZone {
    string zone;
    string name;
    float waterLevel;
    float surfacePollution;
    float groundPollution;
    float rainPollution;
    float totalPollution;
};

/* For Bellman–Ford */
struct Edge {
    int u, v, weight;
};

/* For Priority Queue */
struct ZonePriority {
    string zone;
    string subzone;
    float score;

    bool operator<(const ZonePriority& other) const {
        return score < other.score; // Max Heap
    }
};

/* ===================== CSV READING ===================== */

vector<SubZone> readCSV(const string& filename) {
    vector<SubZone> data;
    ifstream file(filename);
    string line;

    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        SubZone z;

        getline(ss, z.zone, ',');
        getline(ss, z.name, ',');
        ss >> z.waterLevel; ss.ignore();
        ss >> z.surfacePollution; ss.ignore();
        ss >> z.groundPollution; ss.ignore();
        ss >> z.rainPollution;

        z.totalPollution =
            z.surfacePollution +
            z.groundPollution +
            z.rainPollution;

        data.push_back(z);
    }
    return data;
}

vector<SubZone> getZone(const vector<SubZone>& all, const string& zoneName) {
    vector<SubZone> result;
    for (auto& z : all)
        if (z.zone == zoneName)
            result.push_back(z);
    return result;
}

/* ===================== MERGE SORT (UNSAFE ZONES) ===================== */

void merge(vector<SubZone>& a, int l, int m, int r) {
    vector<SubZone> temp;
    int i = l, j = m + 1;

    while (i <= m && j <= r) {
        if (a[i].totalPollution > a[j].totalPollution)
            temp.push_back(a[i++]);
        else
            temp.push_back(a[j++]);
    }

    while (i <= m) temp.push_back(a[i++]);
    while (j <= r) temp.push_back(a[j++]);

    for (int k = l; k <= r; k++)
        a[k] = temp[k - l];
}

void mergeSort(vector<SubZone>& a, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r);
}

void markUnsafeZones(vector<SubZone>& data) {
    mergeSort(data, 0, data.size() - 1);

    cout << "\n--- UNSAFE ZONES (Sorted by Pollution) ---\n";
    for (auto& z : data)
        if (z.totalPollution > SAFE_POLLUTION)
            cout << z.zone << " - " << z.name
                 << " | Pollution: " << z.totalPollution << endl;
}

/* ===================== BELLMAN–FORD (SPIKE DETECTION) ===================== */

bool bellmanFordSpike(int V, vector<Edge>& edges) {
    vector<int> dist(V, 0);

    for (int i = 1; i <= V - 1; i++)
        for (auto& e : edges)
            if (dist[e.u] + e.weight < dist[e.v])
                dist[e.v] = dist[e.u] + e.weight;

    for (auto& e : edges)
        if (dist[e.u] + e.weight < dist[e.v])
            return true;

    return false;
}

/* ===================== FLOOD CHECK ===================== */

void floodCheck(const vector<SubZone>& upstream) {
    int count = 0;
    for (auto& z : upstream)
        if (z.waterLevel > FLOOD_THRESHOLD)
            count++;

    if (count >= 3)
        cout << "\n⚠ FLOOD ALERT: Upstream water rising rapidly\n";
}

/* ===================== DAM CONTROL ===================== */

void damControl(const vector<SubZone>& dam) {
    float avg = 0;
    for (auto& z : dam)
        avg += z.totalPollution;
    avg /= dam.size();

    if (avg > SAFE_POLLUTION)
        cout << "DAM ACTION: Releasing water to refresh reservoir\n";
}

/* ===================== INDUSTRIAL ALERT ===================== */

void industrialAlert(const vector<SubZone>& d2) {
    for (auto& z : d2)
        if (z.totalPollution > 90) {
            cout << "INDUSTRIAL ALERT at " << z.name << endl;
            return;
        }
}

/* ===================== FISHING CHECK ===================== */

void fishingCheck(const vector<SubZone>& d1) {
    int safe = 0;
    for (auto& z : d1)
        if (z.totalPollution < SAFE_POLLUTION)
            safe++;

    cout << (safe >= 3 ? "Fishing Allowed\n" : "Fishing Banned\n");
}

/* ===================== PRIORITY QUEUE ===================== */

priority_queue<ZonePriority> buildPriorityQueue(const vector<SubZone>& data) {
    priority_queue<ZonePriority> pq;

    for (auto& z : data) {
        float score = z.totalPollution * 2;
        if (z.waterLevel > FLOOD_THRESHOLD)
            score += 50;

        pq.push({z.zone, z.name, score});
    }
    return pq;
}

void processPriorityQueue(priority_queue<ZonePriority> pq) {
    cout << "\n--- EMERGENCY PRIORITY ORDER ---\n";
    while (!pq.empty()) {
        auto t = pq.top(); pq.pop();
        cout << t.zone << " - " << t.subzone
             << " | Score: " << t.score << endl;
    }
}

/* ===================== MAIN ===================== */

int main() {
    cout << "=== SMART WATER POLLUTION & FLOOD MONITORING SYSTEM ===\n";

    vector<SubZone> allData = readCSV("water_zones_data.csv");

    auto upstream    = getZone(allData, "Upstream");
    auto dam         = getZone(allData, "Dam");
    auto downstream1 = getZone(allData, "Downstream1");
    auto downstream2 = getZone(allData, "Downstream2");

    vector<Edge> upstreamEdges = {
        {0,1,-5}, {1,2,-6}, {2,3,-8}, {3,4,-7}, {4,1,-10}
    };

    if (bellmanFordSpike(5, upstreamEdges))
        cout << "\n⚠ Sudden Water Spike Detected in Upstream\n";

    floodCheck(upstream);
    damControl(dam);
    industrialAlert(downstream2);
    fishingCheck(downstream1);

    markUnsafeZones(allData);

    auto pq = buildPriorityQueue(allData);
    processPriorityQueue(pq);

    return 0;
}
