#include "graph.h"

using namespace std;

// --- Implémentation des constructeurs ---

Edge::Edge(int d, double dist, const string& r)
    : dest(d), distance(dist), routeId(r) {
}

Stop::Stop() : id(-1), lat(0), lon(0) {}

Stop::Stop(int id_, const string& name_, double lat_, double lon_)
    : id(id_), name(name_), lat(lat_), lon(lon_) {
}

// --- Variables globales ---
unordered_map<int, Stop> stops;
unordered_map<string, vector<int>> subrouteStops;
unordered_map<string, string> routeNames;

// --- Fonction haversine ---
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    double a = sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

// --- Lecture du fichier stops.json ---
bool readStops(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erreur: impossible d'ouvrir " << filename << endl;
        return false;
    }
    json j;
    file >> j;
    if (!j.contains("features")) {
        cerr << "Format invalide de " << filename << endl;
        return false;
    }
    for (auto& feature : j["features"]) {
        int id = feature["id"];
        string name = feature["properties"]["name"];
        double lon = feature["geometry"]["coordinates"][0];
        double lat = feature["geometry"]["coordinates"][1];
        stops[id] = Stop(id, name, lat, lon);
        if (feature["properties"].contains("subroutes")) {
            for (auto& subr : feature["properties"]["subroutes"]) {
                for (auto& subId : subr["subroute_ids"]) {
                    string subroute = subId;
                    subrouteStops[subroute].push_back(id);
                }
            }
        }
    }
    return true;
}

// --- Lecture du fichier routes.json ---
bool readRoutes(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erreur: impossible d'ouvrir " << filename << endl;
        return false;
    }
    json jr;
    file >> jr;
    if (!jr.contains("data")) {
        cerr << "Format invalide de " << filename << endl;
        return false;
    }
    for (auto& r : jr["data"]) {
        string routeId = r["id"];
        string routeName = r["name"];
        routeNames[routeId] = routeName;
    }
    // Construction des arêtes pour chaque sous-trajet
    for (auto& entry : subrouteStops) {
        string subrouteId = entry.first;
        vector<int>& stopsList = entry.second;
        if (stopsList.size() < 2) continue;
        double maxDist = -1.0;
        int end1 = -1, end2 = -1;
        for (size_t i = 0; i < stopsList.size(); ++i) {
            for (size_t j = i + 1; j < stopsList.size(); ++j) {
                int s1 = stopsList[i], s2 = stopsList[j];
                double d = haversine(stops[s1].lat, stops[s1].lon,
                    stops[s2].lat, stops[s2].lon);
                if (d > maxDist) {
                    maxDist = d;
                    end1 = s1;
                    end2 = s2;
                }
            }
        }
        if (end1 == -1 || end2 == -1) continue;
        vector<int> ordered;
        ordered.reserve(stopsList.size());
        unordered_map<int, bool> visited;
        for (int sid : stopsList) visited[sid] = false;
        int current = end1;
        visited[current] = true;
        ordered.push_back(current);
        while (true) {
            double minDist = numeric_limits<double>::infinity();
            int nearestId = -1;
            for (int sid : stopsList) {
                if (!visited[sid]) {
                    double d = haversine(stops[current].lat, stops[current].lon,
                        stops[sid].lat, stops[sid].lon);
                    if (d < minDist) {
                        minDist = d;
                        nearestId = sid;
                    }
                }
            }
            if (nearestId == -1) break;
            visited[nearestId] = true;
            ordered.push_back(nearestId);
            current = nearestId;
        }
        string routeId = subrouteId.substr(0, subrouteId.find('_'));
        for (size_t k = 0; k + 1 < ordered.size(); ++k) {
            int a = ordered[k], b = ordered[k + 1];
            double dist = haversine(stops[a].lat, stops[a].lon,
                stops[b].lat, stops[b].lon);
            stops[a].neighbors.emplace_back(b, dist, routeId);
            stops[b].neighbors.emplace_back(a, dist, routeId);
        }
    }
    return true;
}

// --- Implémentation de bfsRoute ---
vector<int> bfsRoute(int start, int dest,
    const unordered_set<string>& forbiddenRoutes,
    const unordered_set<int>& forbiddenStops,
    const unordered_set<string>& allowedRoutes,
    const unordered_set<int>& allowedStops)
{
    unordered_map<int, int> parent;
    unordered_map<int, string> parentRoute;
    queue<int> q;
    q.push(start);
    parent[start] = -1;
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        if (cur == dest)
            break;
        for (auto& edge : stops[cur].neighbors) {
            int nxt = edge.dest;
            if (!forbiddenRoutes.empty() && forbiddenRoutes.find(edge.routeId) != forbiddenRoutes.end())
                continue;
            if (!allowedRoutes.empty() && allowedRoutes.find(edge.routeId) == allowedRoutes.end())
                continue;
            if (forbiddenStops.find(nxt) != forbiddenStops.end())
                continue;
            if (!allowedStops.empty() && allowedStops.find(nxt) == allowedStops.end())
                continue;
            if (parent.find(nxt) == parent.end()) {
                parent[nxt] = cur;
                parentRoute[nxt] = edge.routeId;
                q.push(nxt);
            }
        }
    }
    vector<int> path;
    if (parent.find(dest) == parent.end())
        return path;
    for (int cur = dest; cur != -1; cur = parent[cur])
        path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

// --- Implémentation de dijkstraRoute ---
vector<int> dijkstraRoute(int start, int dest,
    const unordered_set<string>& forbiddenRoutes,
    const unordered_set<int>& forbiddenStops,
    const unordered_set<string>& allowedRoutes,
    const unordered_set<int>& allowedStops)
{
    vector<double> dist(10000, numeric_limits<double>::max());
    unordered_map<int, int> parent;
    unordered_map<int, string> parentRoute;
    typedef pair<double, int> Node;
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    dist[start] = 0.0;
    pq.push({ 0.0, start });
    while (!pq.empty()) {
        auto top = pq.top();
        double d = top.first;
        int cur = top.second;
        pq.pop();
        if (d > dist[cur])
            continue;
        if (cur == dest)
            break;
        for (auto& edge : stops[cur].neighbors) {
            int nxt = edge.dest;
            if (!forbiddenRoutes.empty() && forbiddenRoutes.find(edge.routeId) != forbiddenRoutes.end())
                continue;
            if (!allowedRoutes.empty() && allowedRoutes.find(edge.routeId) == allowedRoutes.end())
                continue;
            if (forbiddenStops.find(nxt) != forbiddenStops.end())
                continue;
            if (!allowedStops.empty() && allowedStops.find(nxt) == allowedStops.end())
                continue;
            double nd = d + edge.distance;
            if (nd < dist[nxt]) {
                dist[nxt] = nd;
                parent[nxt] = cur;
                parentRoute[nxt] = edge.routeId;
                pq.push({ nd, nxt });
            }
        }
    }
    vector<int> path;
    if (dist[dest] == numeric_limits<double>::max())
        return path;
    for (int cur = dest; cur != start; cur = parent[cur])
        path.push_back(cur);
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// --- Implémentation de computePathDistance ---
double computePathDistance(const vector<int>& path) {
    double total = 0.0;
    for (size_t i = 0; i + 1 < path.size(); i++) {
        int a = path[i], b = path[i + 1];
        total += haversine(stops[a].lat, stops[a].lon, stops[b].lat, stops[b].lon);
    }
    return total;
}

// --- Implémentation de printDetailedRoute ---
void printDetailedRoute(const vector<int>& path) {
    if (path.empty()) {
        cout << "Aucun chemin trouve." << endl;
        return;
    }
    int startId = path[0];
    Stop s = stops[startId];
    cout << s.name << " (" << s.id << ") [" << fixed << setprecision(6)
        << s.lat << ", " << s.lon << "]" << endl;
    for (size_t i = 1; i < path.size(); i++) {
        int cur = path[i - 1], nxt = path[i];
        string routeUsed = "";
        for (auto& edge : stops[cur].neighbors) {
            if (edge.dest == nxt) {
                routeUsed = edge.routeId;
                break;
            }
        }
        Stop sn = stops[nxt];
        cout << routeUsed << " - " << sn.name << " (" << sn.id << ") ["
            << fixed << setprecision(6) << sn.lat << ", " << sn.lon << "]" << endl;
    }
}

// --- Implémentation de printRoute ---
void printRoute(string A, string B,
    const vector<string>& forbiddenRoutesVec,
    const vector<string>& forbiddenStopsVec,
    const vector<string>& allowedRoutesVec,
    const vector<string>& allowedStopsVec)
{
    if (!forbiddenRoutesVec.empty() && !allowedRoutesVec.empty()) {
        cout << "Erreur: on ne peut avoir simultanement des routes interdites et des routes autorisees." << endl;
        return;
    }
    if (!forbiddenStopsVec.empty() && !allowedStopsVec.empty()) {
        cout << "Erreur: on ne peut avoir simultanement des arrets interdits et des arrets autorises." << endl;
        return;
    }
    int start = stoi(A);
    int dest = stoi(B);
    unordered_set<string> forbiddenRoutes(forbiddenRoutesVec.begin(), forbiddenRoutesVec.end());
    unordered_set<string> allowedRoutes(allowedRoutesVec.begin(), allowedRoutesVec.end());
    unordered_set<int> forbiddenStops;
    for (auto& s : forbiddenStopsVec)
        forbiddenStops.insert(stoi(s));
    unordered_set<int> allowedStops;
    for (auto& s : allowedStopsVec)
        allowedStops.insert(stoi(s));

    auto t1 = chrono::high_resolution_clock::now();
    vector<int> pathBFS = bfsRoute(start, dest, forbiddenRoutes, forbiddenStops, allowedRoutes, allowedStops);
    auto t2 = chrono::high_resolution_clock::now();
    double timeBFS = chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1000.0;
    double distanceBFS = computePathDistance(pathBFS);

    t1 = chrono::high_resolution_clock::now();
    vector<int> pathDijkstra = dijkstraRoute(start, dest, forbiddenRoutes, forbiddenStops, allowedRoutes, allowedStops);
    t2 = chrono::high_resolution_clock::now();
    double timeDijkstra = chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1000.0;
    double distanceDijkstra = computePathDistance(pathDijkstra);

    cout << "----------------------------" << endl;
    cout << "Itineraire BFS (moins optimal):" << endl;
    printDetailedRoute(pathBFS);
    cout << "Distance totale: " << fixed << setprecision(2) << distanceBFS << " km" << endl;
    cout << "Temps d'execution: " << timeBFS << " ms" << endl;
    cout << "----------------------------" << endl << endl;

    cout << "Itineraire Dijkstra (optimal):" << endl;
    printDetailedRoute(pathDijkstra);
    cout << "Distance totale: " << fixed << setprecision(2) << distanceDijkstra << " km" << endl;
    cout << "Temps d'execution: " << timeDijkstra << " ms" << endl;
}
