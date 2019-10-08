#include "trie.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
// #include <string>

nodeIdx sIndex[N];
trieNode node[N * 7], root;
char pattrn[50], words[50];
int cnt = 0, idxCnt = 0, num = 0, wordLen;
bool used[N];

void insertP(char *p, int idx) {
    int i;
    trieNode *tmp = &root;
    while ('\0' != *p) {
        if (nullptr == tmp->son[*p - 'a']) {
            tmp->son[*p - 'a'] = &node[cnt];
            for (i = 0; i < sonNum; i++)
                node[cnt].son[i] = NULL;
            node[cnt].idNums = 0;
            node[cnt].pIdx = NULL;
            cnt++;
        }
        tmp = tmp->son[*p - 'a'];
        p++;
    }
    tmp->idNums++;
    if (1 < tmp->idNums) {
        sIndex[idxCnt].index = idx;
        sIndex[idxCnt].link = tmp->pIdx;
        tmp->pIdx = &sIndex[idxCnt];
        idxCnt++;
    } else {
        tmp->pIdx = &sIndex[idxCnt];
        sIndex[idxCnt].index = idx;
        sIndex[idxCnt].link = NULL;
        idxCnt++;
    }
}

void query(trieNode *curNode, int curIndex) {
    if (curIndex == wordLen) {
        if (curNode->idNums > 0) {
            nodeIdx *tmp = curNode->pIdx;
            while (NULL != tmp) {
                if (!used[tmp->index]) {
                    // ans[num++]=tmp->index;
                    num++;
                    used[tmp->index] = true;
                }
                tmp = tmp->link;
            }
        }
        if ((NULL != curNode->son[26]) && (0 < curNode->son[26]->idNums)) {
            nodeIdx *tmp = curNode->son[26]->pIdx;
            while (NULL != tmp) {
                if (!used[tmp->index]) {
                    // ans[num++]=tmp->index;
                    num++;
                    used[tmp->index] = true;
                }
                tmp = tmp->link;
            }
        }
        return;
    }
    if (NULL != curNode->son[words[curIndex] - 'a']) {
        query(curNode->son[words[curIndex] - 'a'], curIndex + 1);
    }
    if (NULL != curNode->son[26]) {
        if (0 < curNode->son[26]->idNums) {
            nodeIdx *tmp = curNode->son[26]->pIdx;
            while (NULL != tmp) {
                if (!used[tmp->index]) {
                    //    ans[num++]=tmp->index;
                    num++;
                    used[tmp->index] = true;
                }
                tmp = tmp->link;
            }
        }
        trieNode *tmp = curNode->son[26];
        int pos = wordLen - 1;
        while (pos >= curIndex) {
            if (NULL != tmp->son[words[pos] - 'a']) {
                query(tmp->son[words[pos] - 'a'], pos + 1);
            }
            //一开始忘了判断*后面是否是？或者*了，贡献了好几个WA啊，考虑不周全
            if (NULL != tmp->son[27]) {
                query(tmp->son[27], pos + 1);
            }
            if (NULL != tmp->son[26]) {
                query(tmp->son[26], pos + 1);
            }
            pos--;
        }
    }
    if (NULL != curNode->son[27]) {
        query(curNode->son[27], curIndex + 1);
    }
}

bool match(string request) {
    ifstream fin(FILE_NAME);
    if (!fin) {
        cout << "文件不存在" << endl;
        return false;
    } else {
        int n = 0, i, j, k;
        root = node[cnt];
        for (i = 0; i < sonNum; i++)
            node[cnt].son[i] = NULL;
        node[cnt].idNums = 0;
        node[cnt].pIdx = NULL;
        cnt = 1;
        string line;
        while (getline(fin, line)) {
            n++;
            memset(pattrn, '\0', sizeof(pattrn));
            strcpy(pattrn, line.c_str());
            j = strlen(pattrn);
            if ('*' == pattrn[j - 1]) {
                //将末尾全是*的缩为一个，多了也没用
                while ('*' == pattrn[j - 2]) {
                    pattrn[j - 1] = '\0';
                    --j;
                }
            }
            for (i = 0; i < j; i++) {
                if ('*' == pattrn[i])
                    pattrn[i] = 'z' + 1;
                else if ('?' == pattrn[i])
                    pattrn[i] = 'z' + 2;
            }
            insertP(pattrn, k);
        }

        memset(words, '\0', sizeof(words));
        strcpy(words, request.c_str());
        num = 0;
        memset(used, 0, sizeof(used));
        wordLen = strlen(words);
        query(&root, 0);
        if (num) {
            // for (k = 0; k < n; k++) {
            //     if (used[k])
            //         printf("%d ", k);
            // }
            // printf("\n");
            printf("Matched!\n");
            return true;
        } else {
            printf("Not match\n");
            return false;
        }
    }
}

// int main() {
//     match("www.google.com");
//     return 0;
// }

// int main() {
//     int n, m, i, j, k;
//     scanf("%d%d", &n, &m);
//     root = node[cnt];
//     for (i = 0; i < sonNum; i++)
//         node[cnt].son[i] = NULL;
//     node[cnt].idNums = 0;
//     node[cnt].pIdx = NULL;
//     cnt = 1;
//     for (k = 0; k < n; k++) {
//         memset(pattrn, '\0', sizeof(pattrn));
//         scanf("%s", pattrn);
//         j = strlen(pattrn);
//         if ('*' == pattrn[j - 1]) {
//             //将末尾全是*的缩为一个，多了也没用
//             while ('*' == pattrn[j - 2]) {
//                 pattrn[j - 1] = '\0';
//                 --j;
//             }
//         }
//         for (i = 0; i < j; i++) {
//             if ('*' == pattrn[i])
//                 pattrn[i] = 'z' + 1;
//             else if ('?' == pattrn[i])
//                 pattrn[i] = 'z' + 2;
//         }
//         insertP(pattrn, k);
//     }
//     for (i = 0; i < m; i++) {
//         memset(words, '\0', sizeof(words));
//         //    scanf("%s",words);
//         cin >> words;
//         num = 0;
//         memset(used, 0, sizeof(used));
//         wordLen = strlen(words);
//         query(&root, 0);
//         if (num) {
//             //    qsort(ans,num,sizeof(int),cmp);
//             for (k = 0; k < n; k++) {
//                 if (used[k])
//                     printf("%d ", k);
//             }
//             printf("\n");
//             //             printf("%d",ans[0]);
//             //             for(k=1;k<num;k++)
//             //                 printf(" %d",ans[k]);
//             //            printf("\n");
//         } else
//             printf("Not match\n");
//     }
//     return 0;
// }