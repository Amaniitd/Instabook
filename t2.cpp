#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) {
    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define file names
    string graph_file = "test0/test-input-0.gra";
    string header_file = "test0/test-header-0.dat";

    // Open header file
    ifstream header(header_file, ios::binary);
    if (!header) {
        cerr << "Error opening header file " << header_file << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Read the start offset for each node
    int n;
    vector<int> start_offsets;
    header.read(reinterpret_cast<char*>(&n), sizeof(int));
    start_offsets.resize(n);
    for (int i = 0; i < n; i++) {
        header.read(reinterpret_cast<char*>(&start_offsets[i]), sizeof(int));
    }

    // Close header file
    header.close();

    // Open graph file
    ifstream graph(graph_file, ios::binary);
    if (!graph) {
        cerr << "Error opening graph file " << graph_file << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Determine the number of nodes to process in this rank
    int nodes_per_rank = n / size;
    int remainder = n % size;
    int nodes_this_rank = nodes_per_rank + (rank < remainder ? 1 : 0);

    // Calculate the start and end nodes to process in this rank
    int start_node = rank * nodes_per_rank + min(rank, remainder);
    int end_node = start_node + nodes_this_rank - 1;

    // Seek to the start of the first node to process in this rank
    graph.seekg(start_offsets[start_node]);

    // Read the graph data for this rank's nodes
    vector<vector<int>> neighbors(nodes_this_rank);
    for (int i = start_node; i <= end_node; i++) {
        int node_id, node_deg;
        graph.read(reinterpret_cast<char*>(&node_id), sizeof(int));
        graph.read(reinterpret_cast<char*>(&node_deg), sizeof(int));
        neighbors[i - start_node].resize(node_deg);
        graph.read(reinterpret_cast<char*>(&neighbors[i - start_node][0]), node_deg * sizeof(int));
    }

    // Close graph file
    graph.close();

    // Do something with the graph data
    // ...

    // Finalize MPI
    MPI_Finalize();

    return 0;
}

