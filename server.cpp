#include "server.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <memory>
using namespace std;

#define PORT 6666

bool Server::init_trie() {
    ifstream fin(FILE_NAME, ios::in);
    if (!fin) {
        ofstream fou(FILE_NAME, ios::out);
        if (!fou) {
            return false;
        }
    }
    trie = new Trie();
    rule_num = 0;
    trie->clear();
    int i = 0;
    string line;
    while (getline(fin, line)) {
        trie->insert(line.c_str(), i);
        ++i;
        ++rule_num;
    }
    return true;
}

bool Server::init_socket() {
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Create socket error: %s (errno: %d)\n", strerror(errno), errno);
        return false;
    } else {
        return true;
    }
}

int Server::bind_listen() {
    int option = 0;
    int optlen = sizeof(option);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (::bind(sd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        printf("bind socket error: %s (errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    if (listen(sd, 10) == -1) {
        printf("listen socket error: %s (errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    return sd;
}

Message Server::process_connection(int type, string request) {
    Message res;
    switch (type) {
        case MESSAGE_TYPE_MATCH:
            // 匹配
            if (match(request)) {
                res.type = 1;
                // res.msg = "Matched!";
                strcpy(res.msg, "Matched!");
            } else {
                res.type = 0;
                // res.msg = "Pattern Not Found";
                strcpy(res.msg, "Pattern Not Found");
            }
            break;
        case MESSAGE_TYPE_ADD:
            // 添加
            if (add(request)) {
                res.type = 1;
                // res.msg = "Success!";
                strcpy(res.msg, "Success!");
            } else {
                res.type = 0;
                // res.msg = "Rule already exists";
                strcpy(res.msg, "Rule already exists");
            }
            break;
        case MESSAGE_TYPE_DEL:
            // 删除
            if (del(request)) {
                res.type = 1;
                // res.msg = "Success!";
                strcpy(res.msg, "Success!");
            } else {
                res.type = 0;
                // res.msg = "Rule not found";
                strcpy(res.msg, "Rule not found");
            }
            break;
    }
    return res;
}

bool Server::match(string s) {
    memset(trie->vis, false, sizeof(trie->vis));
    trie->ans.clear();

    trie->dfs(s.c_str(), 0);
    if (trie->ans.size() > 0) {
        return true;
    } else {
        return false;
    }
}

bool Server::add(string s) {
    ifstream fin(FILE_NAME, ios::in);
    if (!fin) {
        ofstream fou(FILE_NAME, ios::out | ios::app);
    }
    string line;
    while (getline(fin, line)) {
        // 去除末尾不可见字符
        if (line[line.size() - 1] == 13) {
            line.pop_back();
        }
        if (line.c_str() == s.c_str()) {
            return false;
        }
    }
    trie->insert(s.c_str(), rule_num);
    ofstream fou(FILE_NAME, ios::out | ios::app);
    fou << s << endl;
    rule_num++;
    return true;
}

bool Server::del(string s) {
    ifstream fin(FILE_NAME);
    if (!fin) {
        printf("文件不存在\n");
        return false;
    } else {
        string line;
        int line_num = 1;
        bool is_delete = false;
        while (getline(fin, line)) {
            // 去除末尾不可见字符
            if (line[line.size() - 1] == 13) {
                line.pop_back();
            }
            if (line == s) {
                _del_line(line_num);
                is_delete = true;
                continue;
            } else
                ++line_num;
        }
        if (is_delete) {
            init_trie();
            return true;
        } else {
            return false;
        }
    }
}

void Server::_del_line(int line_num) {
    ifstream in(FILE_NAME);
    string file_data = "";
    int line = 1;
    string line_data;
    while (getline(in, line_data)) {
        if (line != line_num) {
            file_data += line_data;
            file_data += "\n";
        }
        line++;
    }
    in.close();

    //写入文件
    ofstream out;
    out.open(FILE_NAME);
    out.flush();
    out << file_data;
    out.close();
}

void Server::close_socket() {
    close(sd);
}

int main(int argc, char **argv) {
    Server serv;
    if (serv.init_trie() == false) {
        cout << "未找到规则文件且无法创建" << endl;
        return -1;
    }
    if (serv.init_socket() == false) {
        cout << "初始化失败" << endl;
    }

    if ((serv.sd = serv.bind_listen()) == -1) {
        return -1;
    }

    timeval timeout;
    fd_set reads, cpy_reads;
    int fd_max = serv.sd, fd_num;
    socklen_t adr_sz;
    sockaddr_in clnt_addr;

    FD_ZERO(&reads);
    FD_SET(serv.sd, &reads);

    while (1) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if (fd_num == 0)
            continue;
        for (int i = 0; i < fd_max + 1; i++) {
            if (FD_ISSET(i, &cpy_reads)) {
                if (i == serv.sd) {
                    adr_sz = sizeof(clnt_addr);
                    int clnt_sock = accept(serv.sd, (sockaddr *)&clnt_addr, &adr_sz);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    cout << "connected client: " << clnt_sock << endl;
                } else {
                    Message msg;
                    int str_len = recv(i, (char *)&msg, sizeof(Message), 0);
                    if (str_len == 0) {
                        FD_CLR(i, &reads);
                        close(i);
                        cout << "closed client: " << i << endl;
                    } else {
                        cout << "Request from #" << i << ": " << msg.type << " " << msg.msg << endl;
                        Message res = serv.process_connection(msg.type, msg.msg);
                        send(i, (char *)&res, sizeof(Message), 0);
                        break;
                    }
                }
            }
        }
    }
    serv.close_socket();
    return 0;
}