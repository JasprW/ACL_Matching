#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

#define FILE_NAME "rules.txt"
using namespace std;

const int MAX_N = 1e5 + 10;
const int MAX_NODE = 6e5 + 10;
const int SIGMA_SIZE = 38;

class Trie {
   private:
    int ch[MAX_NODE][SIGMA_SIZE];
    char alpha[MAX_NODE];
    vector<int> val[MAX_NODE];

   public:
    bool vis[MAX_N];
    vector<int> ans;
    int sz;
    Trie() { clear(); };
    void clear();
    int idx(char c);
    void find(const char* s, int u);
    void insert(const char* s, int v);
    void dfs(const char* s, int u);
};

bool match(string s);

#endif