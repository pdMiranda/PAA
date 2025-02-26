#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;


class Graph
{
private:
    int vertices, edges, timeCounter;
    vector<list<int>> adjacencyList;
    vector<int> discoveryTime, finishTime;
    vector<bool> visited;
    map<pair<int, int>, string> edgeTypes;

    void DFSVisit(int vertex)
    {
        visited[vertex] = true;
        discoveryTime[vertex] = ++timeCounter;

        for (int neighbor : adjacencyList[vertex])
        {
            if (!visited[neighbor])
            {
                edgeTypes[{vertex, neighbor}] = "tree";
                DFSVisit(neighbor);
            }
            else
            {
                if (discoveryTime[neighbor] < discoveryTime[vertex] && finishTime[neighbor] == -1)
                {
                    edgeTypes[{vertex, neighbor}] = "back";
                }
                else if (discoveryTime[vertex] < discoveryTime[neighbor] && finishTime[neighbor] != -1)
                {
                    edgeTypes[{vertex, neighbor}] = "forward";
                }
                else if (discoveryTime[neighbor] < discoveryTime[vertex] && finishTime[neighbor] != -1)
                {
                    edgeTypes[{vertex, neighbor}] = "cross";
                }
            }
        }

        finishTime[vertex] = ++timeCounter;
    }

public:
    Graph(int vertices)
        : vertices(vertices), edges(0), timeCounter(0)
    {
        adjacencyList.resize(vertices);
        discoveryTime.assign(vertices, -1);
        finishTime.assign(vertices, -1);
        visited.assign(vertices, false);
    }

    void addEdge(int origin, int destination)
    {
        if (origin != destination && find(adjacencyList[origin].begin(), adjacencyList[origin].end(), destination) == adjacencyList[origin].end())
        {
            adjacencyList[origin].push_back(destination);
            edges++;
        }
    }

    void createSpecificGraph()
    {
        int e;
        cout << "Enter the number of edges: ";
        cin >> e;

        cout << "Enter the edges in the format 'origin destination':\n";
        for (int i = 0; i < e; ++i)
        {
            int origin, destination;
            cin >> origin >> destination;
            addEdge(origin, destination);
        }
    }

    void generateRandomGraph()
    {
        srand(time(nullptr));
        edges = rand() % 37 + 9;

        for (int i = 0; i < edges; ++i)
        {
            int origin, destination;
            do
            {
                origin = rand() % vertices;
                destination = rand() % vertices;
            } while (origin == destination || find(adjacencyList[origin].begin(), adjacencyList[origin].end(), destination) != adjacencyList[origin].end());

            adjacencyList[origin].push_back(destination);
        }
    }

    void performDFS()
    {
        for (int i = 0; i < vertices; ++i)
        {
            if (!visited[i])
            {
                DFSVisit(i);
            }
        }

        cout << "\nEdge Classification:\n";
        for (const auto &[edge, type] : edgeTypes)
        {
            cout << "Edge (" << edge.first << ", " << edge.second << "): " << type << endl;
        }
    }
};

int main()
{
    int vertexCount = 10;
    Graph graph(vertexCount);

    int option;
    cout << "Choose an option:\n1. Create a specific graph\n2. Generate a random graph\n";
    cin >> option;

    if (option == 1)
    {
        graph.createSpecificGraph();
    }
    else if (option == 2)
    {
        graph.generateRandomGraph();
    }

    graph.performDFS();
    return 0;
}
