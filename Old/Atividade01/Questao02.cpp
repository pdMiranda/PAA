#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstdlib> 
#include <ctime>   

using namespace std;

class Graph {
private:
    int numVertices;
    vector<vector<int>> adjMatrix;

public:
    Graph(int vertices) : numVertices(vertices) {
        adjMatrix.resize(vertices, vector<int>(vertices, 0));
    }

    void addEdge(int u, int v) {
        adjMatrix[u][v] = 1;
        adjMatrix[v][u] = 1;
    }

    void generateRandomEdges(int maxEdges) {
        srand(time(0));

        for (int i = 0; i < maxEdges; ++i) {
            int u = rand() % numVertices;
            int v = rand() % numVertices;
            
            if (u != v && adjMatrix[u][v] == 0) {
                addEdge(u, v);
            }
        }
    }

    vector<int> bfs(int startVertex) {
        vector<int> distances(numVertices, INT_MAX);
        queue<int> q;

        distances[startVertex] = 0;
        q.push(startVertex);

        while (!q.empty()) {
            int currentVertex = q.front();
            q.pop();

            for (int i = 0; i < numVertices; ++i) {
                if (adjMatrix[currentVertex][i] == 1 && distances[i] == INT_MAX) {
                    distances[i] = distances[currentVertex] + 1;
                    q.push(i);
                }
            }
        }

        return distances;
    }

    int findExcentricity(int vertex) {
        vector<int> distances = bfs(vertex);
        int excentricity = 0;

        for (int dist : distances) {
            if (dist != INT_MAX) {
                excentricity = max(excentricity, dist);
            }
        }

        return excentricity;
    }

    void printAdjMatrix() {
        for (int i = 0; i < numVertices; ++i) {
            for (int j = 0; j < numVertices; ++j) {
                cout << adjMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    vector<int> findAllExcentricities() {
        vector<int> excentricities(numVertices, 0);

        for (int vertex = 0; vertex < numVertices; ++vertex) {
            vector<int> distances = bfs(vertex);
            int maxDistance = 0;

            for (int dist : distances) {
                if (dist != INT_MAX) {
                    maxDistance = max(maxDistance, dist);
                }
            }
            excentricities[vertex] = maxDistance;
        }

        return excentricities;
    }

};

int main() {
    int numVertices = 6;
    int maxEdges = 10;

    Graph graph(numVertices);
    graph.generateRandomEdges(maxEdges);

    cout << "Matriz de Adjacencia do Grafo Gerado Aleatoriamente:\n" << endl;
    graph.printAdjMatrix();

    int vertex = 0;
    int excentricity = graph.findExcentricity(vertex);
    cout << "\nA excentricidade do vertice " << vertex << " eh " << excentricity << endl;

    vector<int> excentricities = graph.findAllExcentricities();
    cout << "\nExcentricidade de cada vertice:\n" << endl;
    for (int i = 0; i < excentricities.size(); ++i) {
        cout << "Vertice " << i << ": " << excentricities[i] << endl;
    }

    return 0;
}
