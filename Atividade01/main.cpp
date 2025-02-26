#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <sstream>

using namespace std;
using ll = long long;

ll naiveOpsCount = 0;
ll tarjanOpsCount = 0;

struct Graph
{
    int V;
    vector<vector<int>> adj;

    Graph(int V) : V(V)
    {
        adj.resize(V);
    }

    void addEdge(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void removeEdge(int u, int v)
    {
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end())
            adj[u].erase(it);
        it = find(adj[v].begin(), adj[v].end(), u);
        if (it != adj[v].end())
            adj[v].erase(it);
    }

    void DFSUtil(int v, vector<bool> &visited)
    {
        visited[v] = true;
        naiveOpsCount++;
        for (int u : adj[v])
            if (!visited[u])
                DFSUtil(u, visited);
    }

    void bridgeDFS(int u, vector<bool> &visited, vector<int> &disc, vector<int> &low, vector<int> &parent,
                   unordered_set<ll> &bridges, int &timeCounter)
    {
        visited[u] = true;
        tarjanOpsCount++;
        disc[u] = low[u] = ++timeCounter;
        for (int v : adj[u])
        {
            if (!visited[v])
            {
                parent[v] = u;
                bridgeDFS(v, visited, disc, low, parent, bridges, timeCounter);
                low[u] = min(low[u], low[v]);
                if (low[v] > disc[u])
                {
                    int a = min(u, v);
                    int b = max(u, v);
                    ll key = ((ll)a << 32) | (unsigned int)b;
                    bridges.insert(key);
                }
            }
            else if (v != parent[u])
            {
                low[u] = min(low[u], disc[v]);
            }
        }
    }

    unordered_set<ll> getBridges()
    {
        unordered_set<ll> bridges;
        vector<bool> visited(V, false);
        vector<int> disc(V, 0);
        vector<int> low(V, 0);
        vector<int> parent(V, -1);
        int timeCounter = 0;
        for (int i = 0; i < V; i++)
        {
            if (!visited[i])
                bridgeDFS(i, visited, disc, low, parent, bridges, timeCounter);
        }
        return bridges;
    }

    bool isBridgeTarjan(int u, int v, unordered_set<ll> &bridges)
    {
        int a = min(u, v);
        int b = max(u, v);
        ll key = ((ll)a << 32) | (unsigned int)b;
        return (bridges.find(key) != bridges.end());
    }
};

unordered_set<ll> computeBridgesNaive(Graph g)
{
    unordered_set<ll> bridges;
    for (int u = 0; u < g.V; u++)
    {
        for (int v : g.adj[u])
        {
            if (v > u)
            {
                g.removeEdge(u, v);
                vector<bool> visited(g.V, false);
                g.DFSUtil(u, visited);
                if (!visited[v])
                {
                    int a = u, b = v;
                    if (b < a)
                        swap(a, b);
                    ll key = ((ll)a << 32) | (unsigned int)b;
                    bridges.insert(key);
                }
                g.addEdge(u, v);
            }
        }
    }
    return bridges;
}

vector<int> fleuryTarjan(Graph &g, int start)
{
    vector<int> path;
    int u = start;
    path.push_back(u);
    while (!g.adj[u].empty())
    {
        unordered_set<ll> bridges = g.getBridges();
        int v;
        if (g.adj[u].size() == 1)
            v = g.adj[u][0];
        else
        {
            bool foundNonBridge = false;
            for (int candidate : g.adj[u])
            {
                if (!g.isBridgeTarjan(u, candidate, bridges))
                {
                    v = candidate;
                    foundNonBridge = true;
                    break;
                }
            }
            if (!foundNonBridge)
                v = g.adj[u][0];
        }
        g.removeEdge(u, v);
        path.push_back(v);
        u = v;
    }
    return path;
}

Graph generateEulerianGraph(int n, int e)
{
    Graph g(n);
    vector<int> degree(n, 0);

    for (int i = 0; i < e; i++)
    {
        int u, v;
        do
        {
            u = rand() % n;
            v = rand() % n;
        } while (u == v || find(g.adj[u].begin(), g.adj[u].end(), v) != g.adj[u].end());

        g.addEdge(u, v);
        degree[u]++;
        degree[v]++;
    }

    for (int i = 0; i < n; i++)
    {
        if (degree[i] % 2 != 0)
        {
            int j = (i + 1) % n;
            g.addEdge(i, j);
            degree[i]++;
            degree[j]++;
        }
    }

    return g;
}

int countEdges(const Graph &g)
{
    int count = 0;
    for (int u = 0; u < g.V; u++)
    {
        for (int v : g.adj[u])
        {
            if (v > u)
                count++;
        }
    }
    return count;
}

int main(int argc, char *argv[])
{
    srand(time(0));

    int n = 100;
    if (argc >= 2)
    {
        istringstream ss(argv[1]);
        ss >> n;
    }
    int e = n + (rand() % n);

    cout << "Graph Euleriano conexo com " << n << " vertices e " << e << " arestas" << endl;
    Graph gOriginal = generateEulerianGraph(n, e);
    int totalEdges = countEdges(gOriginal);
    cout << "Total de arestas: " << totalEdges << endl;
    cout << "Tamanho esperado do caminho Euleriano: " << (totalEdges + 1) << endl;

    naiveOpsCount = 0;
    auto start_naive = chrono::high_resolution_clock::now();
    unordered_set<ll> naiveBridges = computeBridgesNaive(gOriginal);
    auto end_naive = chrono::high_resolution_clock::now();
    auto duration_naive = chrono::duration_cast<chrono::milliseconds>(end_naive - start_naive).count();

    tarjanOpsCount = 0;
    auto start_tarjan = chrono::high_resolution_clock::now();
    unordered_set<ll> tarjanBridges = gOriginal.getBridges();
    auto end_tarjan = chrono::high_resolution_clock::now();
    auto duration_tarjan = chrono::duration_cast<chrono::milliseconds>(end_tarjan - start_tarjan).count();

    cout << "Metodo ingeno: " << naiveBridges.size() << " pontes encontradas em "
         << duration_naive << " ms, operacoes = " << naiveOpsCount << endl;
    cout << "Metodo Tarjan: " << tarjanBridges.size() << " pontes encontradas em "
         << duration_tarjan << " ms, operacoes = " << tarjanOpsCount << endl;

    Graph gFleury = gOriginal;
    tarjanOpsCount = 0;
    auto start_fleury = chrono::high_resolution_clock::now();
    vector<int> eulerPath = fleuryTarjan(gFleury, 0);
    auto end_fleury = chrono::high_resolution_clock::now();
    auto duration_fleury = chrono::duration_cast<chrono::milliseconds>(end_fleury - start_fleury).count();

    cout << "Caminho Euleriano encontrado em " << duration_fleury << " ms, operacoes = " << tarjanOpsCount << endl;
    cout << "Tamanho do caminho: " << eulerPath.size() << endl;

    return 0;
}
