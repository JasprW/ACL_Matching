#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

#define FILE_NAME "rules.txt"
using namespace std;

const int MAXN = 1e5 + 10;
const int maxnode = 6e5 + 10;
const int sigma_size = 38;

class Trie {
   private:
    int ch[maxnode][sigma_size];
    char alpha[maxnode];
    vector<int> val[maxnode];
    int sz;

   public:
    Trie() { clear(); };
    void clear();
    int idx(char c);
    void insert(const char* s, int v);
    void dfs(const char* s, int u);
};

bool match(string s);

#endif