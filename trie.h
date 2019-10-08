#ifndef TRIE_H
#define TRIE_H

#define FILE_NAME "rules.txt"
#include <string>
using namespace std;

const int N = 100002;
const int sonNum = 28;

struct nodeIdx {
    int index;
    nodeIdx *link;
    nodeIdx(int i = -1, nodeIdx *l = nullptr) : index(i), link(l) {}
};

struct trieNode {
    nodeIdx *pIdx;
    int idNums;
    trieNode *son[sonNum];
};

void insertP(char *p, int idx);
void query(trieNode *curNode, int curIndex);
bool match(string request);

#endif