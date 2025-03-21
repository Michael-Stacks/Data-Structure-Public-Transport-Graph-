Public Transport Graph (STS (SOCIÉTÉ DE TRANSPORT DU SAGUENAY) )🚍📊
Overview
This project simulates a public transportation network for the Saguenay bus system using real JSON data. It builds a graph where each bus stop is a node and the connections between stops (via bus routes) are the edges. The system calculates the shortest route between two stops using two different algorithms:

BFS (Breadth-First Search) for unweighted paths 🔍
Dijkstra for the optimal (shortest distance) path 🚀
It also supports filtering options to include or exclude specific routes or stops. 🚦❌✅

Features
JSON Parsing: Reads stops.json and routes.json to build the graph 🗂️
Graph Construction: Creates nodes (stops) and edges (routes) from the data 🕸️
Route Calculation: Implements BFS and Dijkstra algorithms to find routes ⏱️
Performance Metrics: Compares execution time and total distance for both methods ⏳📏
Flexible Filtering: Allows setting restrictions on allowed/forbidden routes and stops 🚏
Modular Code: Organized into multiple header and source files for clarity and maintenance 📂

STS_PublicTransport/
├── graph.h           // Declarations for graph and algorithms
├── graph.cpp         // Graph and algorithm implementations
├── main.cpp          // Main function and test cases
├── stops.json        // JSON file with stops data
├── routes.json       // JSON file with routes data
├── json.hpp          // nlohmann JSON library header
└── README.md         // This README file


Acknowledgements
Special thanks to the course instructors and the STS (SOCIÉTÉ DE TRANSPORT DU SAGUENAY) for providing the data used in this project. 🎓🙏


![Console de débogage Microsoft Visual Studio 2025-03-21 00_02_18](https://github.com/user-attachments/assets/1cb007fb-cc87-440a-b5be-ca0fe64cbbd4)

![Console de débogage Microsoft Visual Studio 2025-03-21 00_02_39](https://github.com/user-attachments/assets/0bae8ab9-77c9-417d-ad58-aabcdddd88c6)
