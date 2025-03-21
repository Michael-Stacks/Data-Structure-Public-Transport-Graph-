#pragma once


#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "json.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using json = nlohmann::json;

const double EARTH_RADIUS = 6371.0; // Rayon de la Terre en km

// D�claration de la fonction Haversine
double haversine(double lat1, double lon1, double lat2, double lon2);

// Structure repr�sentant une ar�te du graphe (liaison entre deux arr�ts)
struct Edge {
    int dest;          // identifiant de l'arr�t de destination
    double distance;   // distance entre les arr�ts (en km)
    string routeId;    // num�ro de ligne (ex. "12", "175", etc.)
    Edge(int d, double dist, const string& r);
};

// Structure repr�sentant un arr�t de bus
struct Stop {
    int id;
    string name;
    double lat, lon;
    vector<Edge> neighbors; // Liste des arr�ts accessibles directement depuis cet arr�t
    Stop();
    Stop(int id_, const string& name_, double lat_, double lon_);
};

// Variables globales pour stocker les arr�ts et la correspondance des sous-trajets
extern unordered_map<int, Stop> stops;
extern unordered_map<string, vector<int>> subrouteStops; // Association: identifiant de sous-route -> liste d'arr�ts
extern unordered_map<string, string> routeNames;         // Association: identifiant de route -> nom de la route

// Fonctions de lecture des fichiers JSON et construction du graphe
bool readStops(const string& filename);
bool readRoutes(const string& filename);

// Fonctions d'algorithmes de recherche
vector<int> bfsRoute(int start, int dest,
    const unordered_set<string>& forbiddenRoutes,
    const unordered_set<int>& forbiddenStops,
    const unordered_set<string>& allowedRoutes,
    const unordered_set<int>& allowedStops);

vector<int> dijkstraRoute(int start, int dest,
    const unordered_set<string>& forbiddenRoutes,
    const unordered_set<int>& forbiddenStops,
    const unordered_set<string>& allowedRoutes,
    const unordered_set<int>& allowedStops);

// Fonction calculant la distance totale d'un chemin
double computePathDistance(const vector<int>& path);

// Fonction affichant le chemin d�taill�
void printDetailedRoute(const vector<int>& path);

// La fonction printRoute avec ses 6 param�tres
void printRoute(string A, string B,
    const vector<string>& forbiddenRoutesVec,
    const vector<string>& forbiddenStopsVec,
    const vector<string>& allowedRoutesVec,
    const vector<string>& allowedStopsVec);

#endif // GRAPH_H
