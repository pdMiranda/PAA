#include "tree_edit.h"
#include <algorithm>
#include <functional>
#include <filesystem> 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <random>
#include <queue>
#include <set>

struct TreeInfo {
    std::vector<Node*> postorder; // pós-ordem
    std::unordered_map<Node*, int> node2idx; // nó -> índice pós-ordem
    std::vector<int> lld; // leftmost leaf descendant para cada índice
    std::vector<Node*> idx2node; // índice -> nó
};

static int cost(const std::string& a, const std::string& b) {
    if (a == b) return 0;
    if (a == "-") return 1;
    if (b == "-") return 1;
    return 1;
}

std::vector<std::string> getTreeMatrix(Node* root, const std::unordered_map<Node*, std::string>& ids) {
    std::vector<std::string> lines;
    if (!root) return lines;
    std::queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* node = q.front(); q.pop();
        for (auto* child : node->children) {
            lines.push_back(ids.at(node) + " -> " + ids.at(child));
            q.push(child);
        }
    }
    return lines;
}

void printSideBySide(const std::vector<std::string>& left, const std::vector<std::string>& right, const std::string& titleLeft, const std::string& titleRight) {
    std::cout << std::left << std::setw(16) << titleLeft << " " << titleRight << "\n";
    size_t maxLines = std::max(left.size(), right.size());
    size_t widthLeft = 0, widthRight = 0;
    for (const auto& s : left) widthLeft = std::max(widthLeft, s.size());
    for (const auto& s : right) widthRight = std::max(widthRight, s.size());
    for (size_t i = 0; i < maxLines; ++i) {
        std::cout << std::left << std::setw(widthLeft + 2)
                  << (i < left.size() ? left[i] : "")
                  << "| "
                  << (i < right.size() ? right[i] : "")
                  << "\n";
    }
}

static void computePostorder(Node* node, TreeInfo& info) {
    if (!node) return;
    for (auto* child : node->children)
        computePostorder(child, info);
    info.node2idx[node] = info.postorder.size();
    info.idx2node.push_back(node);
    info.postorder.push_back(node);
}

static void computeLLD(Node* node, TreeInfo& info, std::unordered_map<Node*, int>& lld_map) {
    if (!node) return;
    if (node->children.empty()) {
        lld_map[node] = info.node2idx[node];
    } else {
        computeLLD(node->children.front(), info, lld_map);
        lld_map[node] = lld_map[node->children.front()];
    }
    for (auto* child : node->children)
        computeLLD(child, info, lld_map);
}

static TreeInfo buildTreeInfo(Node* root) {
    TreeInfo info;
    computePostorder(root, info);
    info.lld.resize(info.postorder.size());
    std::unordered_map<Node*, int> lld_map;
    computeLLD(root, info, lld_map);
    for (size_t i = 0; i < info.postorder.size(); ++i)
        info.lld[i] = lld_map[info.postorder[i]];
    return info;
}

int treeEditDistance(Node* t1, Node* t2, bool showLogs) {

    // ARQUIVOS DE LOGS
    std::ofstream logFileAll, logFileMain;
    std::string logPathAll = "logs/forestdist.log";
    std::string logPathMain = "logs/treedist.log";
    if (!std::filesystem::exists("logs"))
        std::filesystem::create_directory("logs");
    logFileAll.open(logPathAll, std::ios::out | std::ios::trunc);
    logFileMain.open(logPathMain, std::ios::out | std::ios::trunc);

    auto t_start = std::chrono::high_resolution_clock::now();
    int opCount = 0;
    auto T1 = buildTreeInfo(t1);
    auto T2 = buildTreeInfo(t2);
    int n = T1.postorder.size();
    int m = T2.postorder.size();

    // DP table
    std::vector<std::vector<int>> treedist(n, std::vector<int>(m, 0));
    std::vector<std::vector<std::string>> op(n, std::vector<std::string>(m, ""));

    // Floresta para cada subárvore
    std::vector<std::vector<int>> forestdist(n+1, std::vector<int>(m+1, 0));
    std::vector<std::vector<std::string>> forestop(n+1, std::vector<std::string>(m+1, ""));

    std::unordered_map<Node*, std::string> ids1, ids2;
    int id_counter = 1;
    std::vector<std::string> t1_nodes, t2_nodes;
    for (auto* node : T1.postorder) {
        std::string curID = "n" + std::to_string(id_counter++) + node->label;
        ids1[node] = curID;
        t1_nodes.push_back(curID);
    }
    for (auto* node : T2.postorder) {
        std::string curID = "n" + std::to_string(id_counter++) + node->label;
        ids2[node] = curID;
        t2_nodes.push_back(curID);
    }
    std::cout << "\nT1 (" << t1_nodes.size() << ") |";
    for (const auto& s : t1_nodes) std::cout << " " << s;
    std::cout << "\nT2 (" << t2_nodes.size() << ") |";
    for (const auto& s : t2_nodes) std::cout << " " << s;
    std::cout << "\n\n";
    auto mat1 = getTreeMatrix(T1.postorder.back(), ids1);
    auto mat2 = getTreeMatrix(T2.postorder.back(), ids2);
    printSideBySide(mat1, mat2, "T1", "T2");
    std::cout << "\n";

    // --- LOGS ---

    std::vector<std::vector<std::string>> last_forestop;
    std::vector<std::vector<int>> last_forestdist;

    for (int i = 0; i < n; ++i) {
        int i_lld = T1.lld[i];
        for (int j = 0; j < m; ++j) {
            int j_lld = T2.lld[j];

            forestdist.assign(n+1, std::vector<int>(m+1, 0));
            forestop.assign(n+1, std::vector<std::string>(m+1, ""));

            forestdist[i_lld][j_lld] = 0;
            for (int di = i_lld+1; di <= i+1; ++di) {
                forestdist[di][j_lld] = forestdist[di-1][j_lld] + 1;
                forestop[di][j_lld] = "del";
                std::string logMsg = "log: forestdist[" + std::to_string(di) + "][" + std::to_string(j_lld) + "] = " +
                    std::to_string(forestdist[di][j_lld]) + " (delecao de T1[" + T1.postorder[di-1]->label + "])\n";
                if (showLogs) std::cout << logMsg;
                logFileAll << logMsg;
            }
            for (int dj = j_lld+1; dj <= j+1; ++dj) {
                forestdist[i_lld][dj] = forestdist[i_lld][dj-1] + 1;
                forestop[i_lld][dj] = "ins";
                std::string logMsg = "log: forestdist[" + std::to_string(i_lld) + "][" + std::to_string(dj) + "] = " +
                    std::to_string(forestdist[i_lld][dj]) + " (insercao de T2[" + T2.postorder[dj-1]->label + "])\n";
                if (showLogs) std::cout << logMsg;
                logFileAll << logMsg;
            }
            for (int di = i_lld+1; di <= i+1; ++di) {
                for (int dj = j_lld+1; dj <= j+1; ++dj) {
                    int ci = di-1, cj = dj-1;
                    if (T1.lld[ci] == i_lld && T2.lld[cj] == j_lld) {
                        int c_del = forestdist[di-1][dj] + 1;
                        int c_ins = forestdist[di][dj-1] + 1;
                        int c_sub = forestdist[di-1][dj-1] + cost(T1.postorder[ci]->label, T2.postorder[cj]->label);

                        int minc = std::min({c_del, c_ins, c_sub});
                        forestdist[di][dj] = minc;
                        std::string chosen;
                        if (minc == c_sub) {
                            if (T1.postorder[ci]->label == T2.postorder[cj]->label) {
                                forestop[di][dj] = "match";
                                chosen = "match";
                            } else {
                                forestop[di][dj] = "sub";
                                chosen = "substituicao";
                            }
                        } else if (minc == c_del) {
                            forestop[di][dj] = "del";
                            chosen = "delecao";
                        } else {
                            forestop[di][dj] = "ins";
                            chosen = "insercao";
                        }
                        treedist[ci][cj] = minc;
                        opCount++;
                        op[ci][cj] = forestop[di][dj];

                        // Log principal (apenas para treedist)
                        std::string mainLogMsg = "treedist[" + std::to_string(ci) + "][" + std::to_string(cj) + "] = " + std::to_string(minc)
                            + " | op: " + forestop[di][dj]
                            + " (T1[" + T1.postorder[ci]->label + "] x T2[" + T2.postorder[cj]->label + "])\n";
                        logFileMain << mainLogMsg;

                        // Log detalhado (já existente)
                        std::string logMsg = "log: forestdist[" + std::to_string(di) + "][" + std::to_string(dj) + "] = " + std::to_string(minc)
                            + " | del: " + std::to_string(c_del) + ", ins: " + std::to_string(c_ins) + ", sub: " + std::to_string(c_sub)
                            + " | op: " + chosen
                            + " (T1[" + T1.postorder[ci]->label + "] x T2[" + T2.postorder[cj]->label + "])\n";
                        if (showLogs) std::cout << logMsg;
                        logFileAll << logMsg;
                    } else {
                        int c_del = forestdist[di-1][dj] + 1;
                        int c_ins = forestdist[di][dj-1] + 1;
                        int c_sub = forestdist[T1.lld[ci]][T2.lld[cj]] + treedist[ci][cj];
                        int minc = std::min({c_del, c_ins, c_sub});
                        forestdist[di][dj] = minc;
                        std::string chosen;
                        if (minc == c_sub) {
                            forestop[di][dj] = "tree";
                            chosen = "tree";
                        } else if (minc == c_del) {
                            forestop[di][dj] = "del";
                            chosen = "delecao";
                        } else {
                            forestop[di][dj] = "ins";
                            chosen = "insercao";
                        }
                        std::string logMsg = "log: forestdist[" + std::to_string(di) + "][" + std::to_string(dj) + "] = " + std::to_string(minc)
                            + " | del: " + std::to_string(c_del) + ", ins: " + std::to_string(c_ins) + ", tree: " + std::to_string(c_sub)
                            + " | op: " + chosen + "\n";
                        if (showLogs) std::cout << logMsg;
                        logFileAll << logMsg;
                    }
                }
            }
            if (i == n-1 && j == m-1) {
                last_forestop = forestop;
                last_forestdist = forestdist;
            }
        }
    }

    logFileAll.close();
    logFileMain.close();

    std::cout << "\nMatriz de distancias (treedist):\n\n   |";
    for (int j = 0; j < m; ++j)
        std::cout << " " << std::setw(3) << T2.postorder[j]->label;
    std::cout << "\n---+";
    for (int j = 0; j < m; ++j)
        std::cout << "----";
    std::cout << "\n";
    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(2) << T1.postorder[i]->label << " |";
        for (int j = 0; j < m; ++j)
            std::cout << " " << std::setw(3) << treedist[i][j];
        std::cout << "\n";
    }
    
    int result = treedist[n-1][m-1];
    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t_end - t_start;

    size_t treedist_bytes = n * m * sizeof(int);
    size_t forestdist_bytes = (n+1) * (m+1) * sizeof(int);
    size_t total_bytes = treedist_bytes + forestdist_bytes;
    std::cout << "\nEspaco ocupado pelas principais matrizes: "
              << (total_bytes / 1024.0) << " KB (" << total_bytes << " bytes)\n";
    std::cout << "Numero de operacoes (celulas preenchidas): " << opCount << "\n";
    std::cout << "Tempo do algoritmo: " << elapsed.count() << " ms\n";

    return result;
}

Node::Node(std::string l) : label(std::move(l)) {}

Node* makeSampleTree1() {
    Node* A = new Node("A");
    Node* B = new Node("B");
    Node* C = new Node("C");
    Node* D = new Node("D");
    A->children = {B, C};
    B->children = {D};
    return A;
}

Node* makeSampleTree2() {
    Node* A = new Node("A");
    Node* X = new Node("X");
    Node* C = new Node("C");
    A->children = {X, C};
    return A;
}

Node* generateRandomTree(int minNodes) {
    return generateRandomTree(-1, -1, {}, 0, minNodes);
}

Node* generateRandomTree(
    int maxDepth,
    int maxChildren,
    const std::vector<std::string>& labels,
    int currentDepth,
    int minNodes
) {
    static std::mt19937 rng(std::random_device{}());

    int localMaxDepth = maxDepth > 0 ? maxDepth : std::uniform_int_distribution<int>(3, 6)(rng);
    int localMaxChildren = maxChildren > 0 ? maxChildren : std::uniform_int_distribution<int>(2, 4)(rng);

    std::vector<std::string> localLabels = labels;
    if (localLabels.empty()) {
        int nLabels = std::uniform_int_distribution<int>(4, 10)(rng);
        std::set<char> used;
        while ((int)localLabels.size() < nLabels) {
            char c = 'A' + std::uniform_int_distribution<int>(0, 25)(rng);
            if (used.insert(c).second)
                localLabels.push_back(std::string(1, c));
        }
    }

    std::uniform_int_distribution<int> labelDist(0, localLabels.size() - 1);

    int maxNodes = 2 * minNodes;

    std::function<Node*(int, int&)> gen = [&](int depth, int& nodesLeft) -> Node* {
        if (nodesLeft <= 0 || (localMaxDepth == 0 || depth >= localMaxDepth))
            return nullptr;
        Node* node = new Node(localLabels[labelDist(rng)]);
        nodesLeft--;
        if (nodesLeft <= 0) return node;
        std::uniform_int_distribution<int> childDist(1, localMaxChildren);
        int numChildren = (depth + 1 < localMaxDepth) ? std::min(childDist(rng), nodesLeft) : 0;
        for (int i = 0; i < numChildren && nodesLeft > 0; ++i) {
            Node* child = gen(depth + 1, nodesLeft);
            if (child) node->children.push_back(child);
        }
        return node;
    };

    Node* root = nullptr;
    int tries = 0;
    do {
        int nodesLeft = maxNodes;
        if (root) delete root;
        root = gen(0, nodesLeft);
        ++tries;
    } while (root && tries < 20 && [&]{
        std::function<int(Node*)> countNodes = [&](Node* n) {
            if (!n) return 0;
            int cnt = 1;
            for (auto* c : n->children) cnt += countNodes(c);
            return cnt;
        };
        return countNodes(root) < minNodes;
    }());

    return root;
}