#include "trie.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

bool vis[MAX_N];
vector<int> ans;

void Trie::clear() {
    sz = 1;
    memset(ch[0], 0, sizeof(ch[0]));
}

int Trie::idx(char c) {
    if (c == '.')
        return 36;
    else if (c == '*')
        return 37;
    else if (c >= 'A' && c <= 'Z')
        return c - 'A';
    else if (c >= 'a' && c <= 'z')
        return c - 'a';
    else if (c >= '0' && c <= '9')
        return 26 + c - '0';
    else
        return -1;
}

void Trie::insert(const char* s, int v) {
    int u = 0, n = strlen(s);
    for (int i = 0; i < n; i++) {
        int c = idx(s[i]);
        if (!ch[u][c]) {
            memset(ch[sz], 0, sizeof(ch[sz]));
            alpha[sz] = s[i];
            val[sz].clear();
            ch[u][c] = sz++;
        }
        u = ch[u][c];
    }
    val[u].push_back(v);
}

void Trie::dfs(const char* s, int u) {
    if (*s == '\0') {
        for (int i = 0; i < val[u].size(); i++) {
            int tmp = val[u][i];
            if (!vis[tmp]) {
                vis[tmp] = true;
                ans.push_back(tmp);
            }
        }
        if (ch[u][37]) dfs(s, ch[u][37]);
        return;
    }
    int c = idx(*s);
    if (ch[u][c])
        dfs(s + 1, ch[u][c]);
    if (ch[u][37]) {
        dfs(s + 1, ch[u][37]);
        dfs(s, ch[u][37]);
    }
    if (alpha[u] == '*')
        dfs(s + 1, u);
}

static Trie trie;

bool match(string str) {
    trie.clear();
    ifstream fin(FILE_NAME);
    if (!fin) {
        cout << "文件不存在" << endl;
        return false;
    } else {
        string line;
        int i = 0;
        while (getline(fin, line)) {
            trie.insert(line.c_str(), i);
            i++;
        }
    }
    memset(vis, false, sizeof(vis));
    ans.clear();
    trie.dfs(str.c_str(), 0);
    int size = ans.size();
    if (size > 0) {
        // puts("Matched!");
        return true;
    } else {
        // puts("Not match");
        return false;
    }
}

// int main() {
//     match("www.jaspr.me");
//     match("www.sa.com");
//     match("....");
//     match("www91porncom");
//     match(".google.com");
//     match("192.136.11.255");
//     match("192.168.192.191");
//     return 0;
// }