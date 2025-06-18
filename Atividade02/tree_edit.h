#ifndef TREE_EDIT_H
#define TREE_EDIT_H

#include <string>
#include <vector>
#include <unordered_map>

struct Node {
    std::string label;
    std::vector<Node*> children;

    explicit Node(std::string l);
};

int treeEditDistance(Node* t1, Node* t2, bool showLogs = false);
Node* makeSampleTree1();
Node* makeSampleTree2();

Node* generateRandomTree(
    int minNodes = 6
);

Node* generateRandomTree(
    int maxDepth,
    int maxChildren,
    const std::vector<std::string>& labels,
    int currentDepth,
    int minNodes
);

#endif