#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip> // para setprecision
using namespace std;
using namespace std::chrono;

// Classe que representa o grafo utilizando lista de adjacência
class Graph
{
public:
    int V;                   // número de vértices
    vector<vector<int>> adj; // lista de adjacência

    Graph(int V)
    {
        this->V = V;
        adj.resize(V);
    }

    // Adiciona aresta bidirecional (u, v)
    void addEdge(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Remove a aresta (u, v) de ambas as listas
    void removeEdge(int u, int v)
    {
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end())
            adj[u].erase(it);
        it = find(adj[v].begin(), adj[v].end(), u);
        if (it != adj[v].end())
            adj[v].erase(it);
    }

    // Reinsere a aresta (u, v)
    void addEdgeBack(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // DFS auxiliar para marcar vértices visitados
    void DFSUtil(int u, vector<bool> &visited)
    {
        visited[u] = true;
        for (int v : adj[u])
            if (!visited[v])
                DFSUtil(v, visited);
    }

    // Verifica se, após a remoção da aresta (u, v), u e v continuam conectados
    bool isConnectedAfterRemoval(int u, int v)
    {
        vector<bool> visited(V, false);
        DFSUtil(u, visited);
        return visited[v];
    }
};

// MÉTODO INGÊNUO: verifica se a aresta (u, v) é ponte
bool isBridgeNaive(Graph &g, int u, int v)
{
    g.removeEdge(u, v);
    bool connected = g.isConnectedAfterRemoval(u, v);
    g.addEdgeBack(u, v);
    return !connected;
}

// ALGORITMO DE TARJAN PARA DETECÇÃO DE PONTES
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

// ALGORITMO DE FLEURY utilizando verificação de ponte (método ingênuo)
void fleuryEulerianPathNaive(Graph g, int start)
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

    cout << "Caminho Euleriano (Fleury - metodo ingenuo): ";
    for (int v : path)
        cout << v << " ";
    cout << endl;
}

// ALGORITMO DE FLEURY utilizando verificação de ponte com Tarjan
void fleuryEulerianPathTarjan(Graph g, int start)
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

    cout << "Caminho Euleriano (Fleury - metodo Tarjan): ";
    for (int v : path)
        cout << v << " ";
    cout << endl;
}

// Verifica se o grafo é Euleriano (todos os vértices com grau par)
bool isEulerian(Graph &g)
{
    for (int i = 0; i < g.V; i++)
    {
        if (g.adj[i].size() % 2 != 0)
            return false;
    }
    return true;
}

// Tenta converter o grafo para Euleriano emparelhando vértices de grau ímpar
// Retorna true se obteve sucesso.
bool makeEulerian(Graph &g)
{
    vector<int> odds;
    for (int i = 0; i < g.V; i++)
    {
        if (g.adj[i].size() % 2 != 0)
            odds.push_back(i);
    }
    // O número de vértices com grau ímpar deve ser par
    if (odds.size() % 2 != 0)
        return false; // situação inesperada

    bool converted = true;
    // Tenta parear os vértices ímpares
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

// Cria um grafo completo Euleriano com V vértices.
// Se V for ímpar, o grafo completo já é Euleriano. Se for par, remove-se uma combinação perfeita.
Graph createCompleteEulerianGraph(int V)
{
    Graph newGraph(V);
    // Cria o grafo completo
    for (int i = 0; i < V; i++)
    {
        for (int j = i + 1; j < V; j++)
        {
            newGraph.addEdge(i, j);
        }
    }
    // Se V for par, o grau de cada vértice é ímpar; remova uma combinação perfeita.
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
        cout << "Uso: " << argv[0] << " <numero_de_vertices>" << endl;
        return 1;
    }

    srand(time(NULL));
    int V = atoi(argv[1]);
    if (V < 2)
    {
        cout << "O grafo deve ter pelo menos 2 vertices." << endl;
        return 1;
    }

    // Define o número de arestas: pelo menos V-1 (para conectividade) mais um extra aleatório
    int extra = rand() % V; // valor entre 0 e V-1
    int M = (V - 1) + extra;

    Graph g(V);

    // Gera uma árvore geradora (spanning tree) para garantir conectividade
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

    cout << "====================================" << endl;
    cout << "Grafo aleatorio gerado:" << endl;
    cout << "Numero de vertices: " << V << endl;
    cout << "Numero de arestas: " << currentEdges << endl;

    // Contagem de pontes usando ambos os métodos
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
    cout << "Metodo ingenuo:" << endl;
    cout << "Numero de pontes encontradas: " << bridgeCountNaive << endl;
    cout << "Tempo de execucao: " << fixed << setprecision(6) << duration_naive << " ms" << endl;
    cout << "------------------------------------" << endl;
    cout << "Metodo Tarjan:" << endl;
    cout << "Numero de pontes encontradas: " << bridgesTarjan.size() << endl;
    cout << "Tempo de execucao: " << fixed << setprecision(6) << duration_tarjan << " ms" << endl;
    cout << "====================================" << endl;

    // Verifica se o grafo é Euleriano; caso não seja, tenta convertê-lo
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

    // Exibe a busca do caminho Euleriano se o grafo for Euleriano
    if (isEulerian(g))
    {
        cout << "Executando Algoritmo de Fleury para busca do caminho Euleriano:" << endl;
        Graph g_naive = g;
        Graph g_tarjan = g;
        cout << "Busca do caminho Euleriano (usando metodo ingenuo para pontes):" << endl;
        fleuryEulerianPathNaive(g_naive, 0);
        cout << "Busca do caminho Euleriano (usando metodo Tarjan para pontes):" << endl;
        fleuryEulerianPathTarjan(g_tarjan, 0);
    }
    else
    {
        cout << "Nao foi possivel obter um grafo Euleriano para busca do caminho Euleriano." << endl;
    }

    return 0;
}
