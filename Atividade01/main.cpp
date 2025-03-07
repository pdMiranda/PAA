#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
using namespace std::chrono;

class Graph
{
public:
    int V;
    vector<vector<int>> adj;

    Graph(int V)
    {
        this->V = V;
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

    void addEdgeBack(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void DFSUtil(int u, vector<bool> &visited)
    {
        visited[u] = true;
        for (int v : adj[u])
            if (!visited[v])
                DFSUtil(v, visited);
    }

    bool isConnectedAfterRemoval(int u, int v)
    {
        vector<bool> visited(V, false);
        DFSUtil(u, visited);
        return visited[v];
    }
};

bool isBridgeNaive(Graph &g, int u, int v)
{
    g.removeEdge(u, v);
    bool connected = g.isConnectedAfterRemoval(u, v);
    g.addEdgeBack(u, v);
    return !connected;
}

void bridgeUtilTarjan(int u, vector<bool> &visited, vector<int> &disc, vector<int> &low, vector<int> &parent,
                      vector<pair<int, int>> &bridges, Graph &g, int &timeCounter)
{
    visited[u] = true;
    disc[u] = low[u] = ++timeCounter;
    for (int v : g.adj[u])
    {
        if (!visited[v])
        {
            parent[v] = u;
            bridgeUtilTarjan(v, visited, disc, low, parent, bridges, g, timeCounter);
            low[u] = min(low[u], low[v]);
            if (low[v] > disc[u])
                bridges.push_back({u, v});
        }
        else if (v != parent[u])
        {
            low[u] = min(low[u], disc[v]);
        }
    }
}

vector<pair<int, int>> findBridgesTarjan(Graph &g)
{
    vector<bool> visited(g.V, false);
    vector<int> disc(g.V, 0);
    vector<int> low(g.V, 0);
    vector<int> parent(g.V, -1);
    vector<pair<int, int>> bridges;
    int timeCounter = 0;
    for (int i = 0; i < g.V; i++)
    {
        if (!visited[i])
            bridgeUtilTarjan(i, visited, disc, low, parent, bridges, g, timeCounter);
    }
    return bridges;
}

void fleuryEulerianPathNaive(Graph g, int start, bool show)
{
    vector<int> path;
    int u = start;
    path.push_back(u);

    while (!g.adj[u].empty())
    {
        int next = -1;
        if (g.adj[u].size() == 1)
        {
            next = g.adj[u][0];
        }
        else
        {
            for (int v : g.adj[u])
            {
                if (!isBridgeNaive(g, u, v))
                {
                    next = v;
                    break;
                }
            }
            if (next == -1)
                next = g.adj[u][0];
        }
        g.removeEdge(u, next);
        u = next;
        path.push_back(u);
    }

    if (show)
    {
        cout << "Caminho Euleriano (Fleury - metodo ingenuo): ";
        for (int v : path)
            cout << v << " ";
        cout << endl;
    }
}

void fleuryEulerianPathTarjan(Graph g, int start, bool show)
{
    vector<int> path;
    int u = start;
    path.push_back(u);

    while (!g.adj[u].empty())
    {
        int next = -1;
        if (g.adj[u].size() == 1)
        {
            next = g.adj[u][0];
        }
        else
        {
            for (int v : g.adj[u])
            {
                vector<pair<int, int>> bridges = findBridgesTarjan(g);
                bool isBridge = false;
                for (auto &b : bridges)
                {
                    if ((b.first == u && b.second == v) || (b.first == v && b.second == u))
                    {
                        isBridge = true;
                        break;
                    }
                }
                if (!isBridge)
                {
                    next = v;
                    break;
                }
            }
            if (next == -1)
                next = g.adj[u][0];
        }
        g.removeEdge(u, next);
        u = next;
        path.push_back(u);
    }

    if (show)
    {
        cout << "Caminho Euleriano (Fleury - metodo Tarjan): ";
        for (int v : path)
            cout << v << " ";
        cout << endl;
    }
}

bool isEulerian(Graph &g)
{
    for (int i = 0; i < g.V; i++)
    {
        if (g.adj[i].size() % 2 != 0)
            return false;
    }
    return true;
}

bool makeEulerian(Graph &g)
{
    vector<int> odds;
    for (int i = 0; i < g.V; i++)
    {
        if (g.adj[i].size() % 2 != 0)
            odds.push_back(i);
    }

    if (odds.size() % 2 != 0)
        return false;

    bool converted = true;
    for (size_t i = 0; i + 1 < odds.size(); i += 2)
    {
        int u = odds[i], v = odds[i + 1];
        if (find(g.adj[u].begin(), g.adj[u].end(), v) == g.adj[u].end())
        {
            g.addEdge(u, v);
        }
        else
        {
            bool paired = false;
            for (size_t j = i + 2; j < odds.size(); j++)
            {
                int candidate = odds[j];
                if (find(g.adj[u].begin(), g.adj[u].end(), candidate) == g.adj[u].end())
                {
                    g.addEdge(u, candidate);
                    swap(odds[i + 1], odds[j]);
                    paired = true;
                    break;
                }
            }
            if (!paired)
            {
                converted = false;
                break;
            }
        }
    }
    return isEulerian(g) && converted;
}

Graph createCompleteEulerianGraph(int V)
{
    Graph newGraph(V);
    for (int i = 0; i < V; i++)
    {
        for (int j = i + 1; j < V; j++)
        {
            newGraph.addEdge(i, j);
        }
    }
    if (V % 2 == 0)
    {
        for (int i = 0; i < V / 2; i++)
        {
            newGraph.removeEdge(i, i + V / 2);
        }
    }
    return newGraph;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Uso: " << argv[0] << " <numero_de_vertices> [-s]" << endl;
        return 1;
    }

    bool showDetails = false;
    if (argc >= 3)
    {
        string arg = argv[2];
        if (arg == "-s")
            showDetails = true;
    }

    srand(time(NULL));
    int V = atoi(argv[1]);
    if (V < 2)
    {
        cout << "O grafo deve ter pelo menos 2 vertices." << endl;
        return 1;
    }

    auto startGraph = high_resolution_clock::now();

    int extra = rand() % V;
    int M = (V - 1) + extra;

    Graph g(V);

    vector<int> connected;
    vector<int> notConnected;
    for (int i = 0; i < V; i++)
        notConnected.push_back(i);

    connected.push_back(notConnected[0]);
    notConnected.erase(notConnected.begin());

    while (!notConnected.empty())
    {
        int idx1 = rand() % connected.size();
        int idx2 = rand() % notConnected.size();
        int u = connected[idx1];
        int v = notConnected[idx2];
        g.addEdge(u, v);
        connected.push_back(v);
        notConnected.erase(notConnected.begin() + idx2);
    }

    int currentEdges = V - 1;
    while (currentEdges < M)
    {
        int u = rand() % V;
        int v = rand() % V;
        if (u == v)
            continue;
        if (find(g.adj[u].begin(), g.adj[u].end(), v) != g.adj[u].end())
            continue;
        g.addEdge(u, v);
        currentEdges++;
    }

    auto endGraph = high_resolution_clock::now();
    double durationGraph = duration_cast<duration<double, milli>>(endGraph - startGraph).count();

    cout << "====================================" << endl;
    cout << "Grafo aleatorio gerado:" << endl;
    cout << "Numero de vertices: " << V << endl;
    cout << "Numero de arestas: " << currentEdges << endl;
    cout << "Tempo de geracao do grafo: " << fixed << setprecision(6) << durationGraph << " ms" << endl;

    vector<pair<int, int>> edges;
    vector<vector<bool>> marked(V, vector<bool>(V, false));
    for (int u = 0; u < V; u++)
    {
        for (int v : g.adj[u])
        {
            if (u < v && !marked[u][v])
            {
                edges.push_back({u, v});
                marked[u][v] = marked[v][u] = true;
            }
        }
    }

    auto start_naive = high_resolution_clock::now();
    int bridgeCountNaive = 0;
    for (auto edge : edges)
    {
        if (isBridgeNaive(g, edge.first, edge.second))
            bridgeCountNaive++;
    }
    auto end_naive = high_resolution_clock::now();
    double duration_naive = duration_cast<duration<double, milli>>(end_naive - start_naive).count();

    auto start_tarjan = high_resolution_clock::now();
    vector<pair<int, int>> bridgesTarjan = findBridgesTarjan(g);
    auto end_tarjan = high_resolution_clock::now();
    double duration_tarjan = duration_cast<duration<double, milli>>(end_tarjan - start_tarjan).count();

    cout << "------------------------------------" << endl;
    cout << "Metodo ingenuo (pontes):" << endl;
    cout << "Numero de pontes encontradas: " << bridgeCountNaive << endl;
    cout << "Tempo de execucao: " << fixed << setprecision(6) << duration_naive << " ms" << endl;
    cout << "------------------------------------" << endl;
    cout << "Metodo Tarjan (pontes):" << endl;
    cout << "Numero de pontes encontradas: " << bridgesTarjan.size() << endl;
    cout << "Tempo de execucao: " << fixed << setprecision(6) << duration_tarjan << " ms" << endl;
    cout << "====================================" << endl;

    if (!isEulerian(g))
    {
        cout << "O grafo nao e Euleriano. Tentando converter para grafo Euleriano..." << endl;
        bool converted = makeEulerian(g);
        if (!converted)
        {
            cout << "Nao foi possivel converter o grafo para Euleriano com a estrategia inicial." << endl;
            cout << "Utilizando grafo completo Euleriano como substituto." << endl;
            g = createCompleteEulerianGraph(V);
            cout << "Grafo Euleriano completo gerado." << endl;
        }
        else
        {
            cout << "Grafo convertido para Euleriano com sucesso." << endl;
        }
    }
    else
    {
        cout << "O grafo original ja e Euleriano." << endl;
    }

    cout << "====================================" << endl;
    cout << "Executando Algoritmo de Fleury para busca do caminho Euleriano:" << endl;

    Graph g_naive = g;
    Graph g_tarjan = g;

    double durationFleuryNaivePrint = 0.0, durationFleuryNaiveNoPrint = 0.0;
    if (showDetails)
    {
        auto startFleuryNaivePrint = high_resolution_clock::now();
        fleuryEulerianPathNaive(g_naive, 0, true);
        auto endFleuryNaivePrint = high_resolution_clock::now();
        durationFleuryNaivePrint = duration_cast<duration<double, milli>>(endFleuryNaivePrint - startFleuryNaivePrint).count();

        Graph g_naive_noPrint = g;
        auto startFleuryNaiveNoPrint = high_resolution_clock::now();
        fleuryEulerianPathNaive(g_naive_noPrint, 0, false);
        auto endFleuryNaiveNoPrint = high_resolution_clock::now();
        durationFleuryNaiveNoPrint = duration_cast<duration<double, milli>>(endFleuryNaiveNoPrint - startFleuryNaiveNoPrint).count();

        cout << "Metodo Fleury ingenuo:" << endl;
        cout << "Tempo de execucao (com print): " << fixed << setprecision(6) << durationFleuryNaivePrint << " ms" << endl;
        cout << "Tempo de execucao (sem print): " << fixed << setprecision(6) << durationFleuryNaiveNoPrint << " ms" << endl;
    }
    else
    {
        auto startFleuryNaive = high_resolution_clock::now();
        fleuryEulerianPathNaive(g_naive, 0, false);
        auto endFleuryNaive = high_resolution_clock::now();
        double durationFleuryNaive = duration_cast<duration<double, milli>>(endFleuryNaive - startFleuryNaive).count();
        cout << "Metodo Fleury ingenuo:" << endl;
        cout << "Tempo de execucao: " << fixed << setprecision(6) << durationFleuryNaive << " ms" << endl;
    }

    double durationFleuryTarjanPrint = 0.0, durationFleuryTarjanNoPrint = 0.0;
    if (showDetails)
    {
        auto startFleuryTarjanPrint = high_resolution_clock::now();
        fleuryEulerianPathTarjan(g_tarjan, 0, true);
        auto endFleuryTarjanPrint = high_resolution_clock::now();
        durationFleuryTarjanPrint = duration_cast<duration<double, milli>>(endFleuryTarjanPrint - startFleuryTarjanPrint).count();

        // Para medir sem prints, cria outra copia
        Graph g_tarjan_noPrint = g;
        auto startFleuryTarjanNoPrint = high_resolution_clock::now();
        fleuryEulerianPathTarjan(g_tarjan_noPrint, 0, false);
        auto endFleuryTarjanNoPrint = high_resolution_clock::now();
        durationFleuryTarjanNoPrint = duration_cast<duration<double, milli>>(endFleuryTarjanNoPrint - startFleuryTarjanNoPrint).count();

        cout << "Metodo Fleury Tarjan:" << endl;
        cout << "Tempo de execucao (com print): " << fixed << setprecision(6) << durationFleuryTarjanPrint << " ms" << endl;
        cout << "Tempo de execucao (sem print): " << fixed << setprecision(6) << durationFleuryTarjanNoPrint << " ms" << endl;
    }
    else
    {
        auto startFleuryTarjan = high_resolution_clock::now();
        fleuryEulerianPathTarjan(g_tarjan, 0, false);
        auto endFleuryTarjan = high_resolution_clock::now();
        double durationFleuryTarjan = duration_cast<duration<double, milli>>(endFleuryTarjan - startFleuryTarjan).count();
        cout << "Metodo Fleury Tarjan:" << endl;
        cout << "Tempo de execucao: " << fixed << setprecision(6) << durationFleuryTarjan << " ms" << endl;
    }

    return 0;
}
