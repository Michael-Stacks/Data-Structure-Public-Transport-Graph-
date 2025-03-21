#include <iostream>
#include <vector>
#include "graph.h"

using namespace std;

int main() {
    if (!readStops("stops.json") || !readRoutes("routes.json")) {
        cerr << "Erreur lors du chargement des donnees JSON." << endl;
        return 1;
    }

    // Test 1 : Itinéraire sans restrictions
    cout << "=== Test 1 : Itineraire sans restrictions ===" << endl;
    string depart = "1680";
    string arrivee = "686";
    vector<string> forbiddenRoutes, forbiddenStops, allowedRoutes, allowedStops;
    printRoute(depart, arrivee, forbiddenRoutes, forbiddenStops, allowedRoutes, allowedStops);

    // Test 2 : Itinéraire avec interdiction de la route "20"
    cout << "\n=== Test 2 : Itineraire avec interdiction de la route \"20\" ===" << endl;
    vector<string> fr = { "20" };
    printRoute(depart, arrivee, fr, forbiddenStops, allowedRoutes, allowedStops);

    // Test 3 : Itinéraire avec interdiction de l'arrêt "1289"
    cout << "\n=== Test 3 : Itineraire avec interdiction de l'arret \"1289\" ===" << endl;
    vector<string> fs = { "1289" };
    printRoute(depart, arrivee, forbiddenRoutes, fs, allowedRoutes, allowedStops);

 

    return 0;
}
