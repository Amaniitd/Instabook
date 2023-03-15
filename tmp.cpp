#include "bits/stdc++.h"
#include "mpi.h"

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



void readGraph(string filename, int &n, int &m, vector<vector<int>> &adjList, vector<int> &degree) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    file.read((char *) &n, sizeof(int));
    file.read((char *) &m, sizeof(int));
    adjList.resize(n);
    degree.resize(n);
    for (int i = 0; i < n; i++) {
        int node, deg;
        file.read((char *) &node, sizeof(int));
        file.read((char *) &deg, sizeof(int));
        adjList[node].resize(deg);
        degree[node] = deg;
        for (int j = 0; j < deg; j++) {
            file.read((char *) &adjList[node][j], sizeof(int));
        }
    }
    file.close();
}



int main(){
    int n, m;
    vector<vector<int>> adjList;
    vector<int> degrees;
    readGraph("test0/test-input-0.gra", n, m, adjList, degrees);
    cout << "n = " << n << endl;
    cout << "m = " << m << endl;

    // int* deleted = new int[n];
    // memset(deleted, 0, n * sizeof(int));
    // int* deg_reduced = new int[n];
    // memset(deg_reduced, 0, n * sizeof(int));
    // int k1 = 23;
    // int num_deletable = 0;
    // do {
    //     int local_num_deletable = 0;
    //     for (int i = 0; i < n; i++) {
    //         if (deleted[i]) {
    //             continue;
    //         }
    //         if (degrees[i] - deg_reduced[i] < k1 - 1) {
    //             deleted[i] = 1;
    //             local_num_deletable++;
    //             for (int j = 0; j < degrees[i]; j++) {
    //                 int neighbor = adjList[i][j];
    //                 if (deleted[neighbor]) { 
    //                     continue;
    //                 }
    //                 deg_reduced[neighbor]++;
    //             }
    //         }
    //     }
    //     num_deletable = local_num_deletable;
    //     cout << "num_deletable = " << num_deletable << endl;
    // } while (num_deletable > 0);

    // for (int i = 0; i < n; i++) {
    //     if (deleted[i]) {
    //         cout << i << " ";
    //     }
    // }

    // return 0;
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int start_vertex = world_rank * n / world_size;
    int end_vertex = (world_rank + 1) * n / world_size;
    if (world_rank == world_size - 1) {
        end_vertex = n;
    }
    // prefilter 
    int* deleted = new int[n];
    int* local_deleted = new int[n];
    int* local_deg_reduced = new int[n];
    int* deg_reduced = new int[n];
    memset(deg_reduced, 0, n * sizeof(int));
    memset(deleted, 0, n * sizeof(int));
    memset(local_deleted, 0, n * sizeof(int));

    int k1 = 24;
    int num_deletable = 0;
    do {
        for (int i = 0; i < n; i++) {
            local_deg_reduced[i] = 0;
        }
        int local_num_deletable = 0;
        for (int i = start_vertex; i < end_vertex; i++){
            local_deleted[i] = deleted[i];
        }
        for (int i = start_vertex; i < end_vertex; i++) {
            if (local_deleted[i]) {
                continue;
            }
            if (degrees[i] < k1 - 1) {
                local_deleted[i] = 1;
                local_num_deletable++;
                for (int j = 0; j < degrees[i]; j++) {
                    int neighbor = adjList[i][j];
                    if (deleted[neighbor]) { 
                        continue;
                    }
                    local_deg_reduced[neighbor]++;
                }
            }
        }
        MPI_Allreduce(&local_num_deletable, &num_deletable, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(local_deg_reduced, deg_reduced, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(local_deleted, deleted, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        
        if (world_rank == 0){
            for (int i = 0; i < n; i++){
                cout << deg_reduced[i] << " ";
            }
            cout << endl;
        }
        for (int i = 0; i < n; i++){
            degrees[i] -= deg_reduced[i];
        }
    } while (num_deletable > 0);
    if (world_rank == 0) {
        cout << "prefilter done" << endl;
        for (int i = 0; i < n; i++) {
            if (deleted[i]) {
                cout << i << " ";
            }
        }
        // for (int i : degrees) cout << i << " ";
    }
    // end of prefilter
    MPI_Finalize();



}

// compile: mpicc -g -Wall -o program_3_01 program_3_01.cpp
// run: mpiexec -n <number of processes> ./program_3_01