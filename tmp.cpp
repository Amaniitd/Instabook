#include "bits/stdc++.h"
using namespace std;


//  Input Format
// The structure for each binary test-input file is provided below. The corresponding representation is little-endian-format in the binary file, e.g., test-input.gra:
// <4-bytes-representing n> <4-bytes-representing m>
// «node-info-node-0»
// «node-info-node-1»
// —
// —
// «node-info-node-(n-1)»
// where
// «node-info-node-i»=<4-byte-representing node-i> <4-byte-representing-deg(i)> «neighbors»
// and
// «neighbors» = <4-bytes-representing neighbour-1> ... <4-bytes-representing neighbour-deg(i)>
// Along with each test input.gra, you will also have a meta-data file, e.g., header.dat. This file
// will contain the start offset for each node in test-input.bat file. The same little-endian format is
// used for this also, and each offset is 4-byte. Thus, the offset for vertex i starts at byte 4 × i in
// header.dat
// Note that vertices in the graph are 0-based.



void readGraph(string filename, int &n, int &m, vector<vector<int>> &adjList) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    file.read((char *) &n, sizeof(int));
    file.read((char *) &m, sizeof(int));
    adjList.resize(n);
    for (int i = 0; i < n; i++) {
        int node, deg;
        file.read((char *) &node, sizeof(int));
        file.read((char *) &deg, sizeof(int));
        adjList[node].resize(deg);
        for (int j = 0; j < deg; j++) {
            file.read((char *) &adjList[node][j], sizeof(int));
        }
    }
    file.close();
}

int main(){
      int n, m;
      vector<vector<int>> adjList;
      readGraph("test0/test-input-0.gra", n, m, adjList);
      cout << n << " " << m << endl;
      for (int i = 0; i < n; i++) {
         cout << i << " " << adjList[i].size() << " ";
         for (int j = 0; j < adjList[i].size(); j++) {
               cout << adjList[i][j] << " ";
         }
         cout << endl;
      }
      return 0;
}
