#pragma once
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <bitset>
#include <string>
#include <vector>

using namespace std;

map<char, int> freqMap;
map<char, string> codes;

struct Node {
    char ch;
    int freq;

    Node* left;
    Node* right;
};

struct Compare {
    bool operator()(Node* a, Node* b) const {
        return a->freq > b->freq;
    }
};

Node* buildTree() {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (int i = 0; i < 256; i++) {
        char ch = (char)i;
        int freq = freqMap[ch];

        if (freq > 0) {
            Node* n = new Node;
            n->ch = ch;
            n->freq = freq;
            n->left = NULL;
            n->right = NULL;
            pq.push(n);
        }
    }

    if (pq.empty()) {
        return NULL;
    }

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();

        Node* right = pq.top();
        pq.pop();

        Node* parent = new Node;
        parent->freq = left->freq + right->freq;
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}

void generateCodes(Node* node, string code) {
    if (node == NULL) {
        return; 
    }

    if (node->left == NULL && node->right == NULL) {
        if (code == "") {
            code = "0"; 
        }
        codes[node->ch] = code;
        return;
    }

    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}