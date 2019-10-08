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
#include <regex>
#include "trie.h"
using namespace std;

#define MAXLINE 1024
#define PORT 6666
#define FILE_NAME "rules.txt"

bool server::InitSock() {
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Create socket error: %s (errno: %d)\n", strerror(errno), errno);
        return false;
    } else {
        ifstream fin(FILE_NAME, ios::in);
        if (!fin) {
            printf("配置文件不存在\n");
        } else {
            storage.clear();
            string line;
            while (getline(fin, line)) {
                storage.push_back(line);
            }
        }
        return true;
    }
}

int server::BindListen() {
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

int server::AcceptConnection(int sd) {
    sockaddr_in clnt_addr;
    int nSize = sizeof(sockaddr_in);
    int clnt_sock = accept(sd, (sockaddr *)NULL, NULL);
    if (clnt_sock == -1) {
        printf("accept socket error: %s (errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    return clnt_sock;
}

Message server::ProcessConnection(int type, string request) {
    Message res;
    switch (type) {
        case MESSAGE_TYPE_MATCH:
            // TODO: 匹配
            if (Match(request)) {
                res.type = 1;
                res.msg = "Pattern Found!";
            } else {
                res.type = 0;
                res.msg = "Pattern Not Found!";
            }
            break;
        case MESSAGE_TYPE_ADD:
            // TODO: 添加
            if (Add(request)) {
                res.type = 1;
                res.msg = "Success!";
            } else {
                res.type = 0;
                res.msg = "Failed!";
            }
            break;
        case MESSAGE_TYPE_DEL:
            // TODO: 删除
            if (Del(request)) {
                res.type = 1;
                res.msg = "Success!";
            } else {
                res.type = 0;
                res.msg = "Failed!";
            }
            break;
    }
    return res;
}

bool server::Match(string s) {
    return match(s);
}

bool server::Add(string s) {
    if (find(storage.begin(), storage.end(), s) == storage.end()) {
        storage.push_back(s);
        ofstream fou(FILE_NAME, ios::out | ios::app);
        fou << s << endl;
        return true;
    } else {
        return false;
    }
}

bool server::Del(string s) {
    auto it = find(storage.begin(), storage.end(), s);
    if (it != storage.end()) {
        storage.erase(it);
        ifstream fin(FILE_NAME);
        if (!fin) {
            printf("文件不存在\n");
            return false;
        } else {
            string line;
            int line_num = 1;
            while (getline(fin, line)) {
                if (line == s) {
                    break;
                }
                ++line_num;
            }
            _DelLine(line_num);
            // cout << "Deleting #" << line_num << " line" << endl;
        }
        return true;
    } else {
        return false;
    }
}

void server::_DelLine(int line_num) {
    ifstream in(FILE_NAME);
    string file_data = "";
    int line = 1;
    string line_data;
    while (getline(in, line_data)) {
        if (line != line_num) {
            // file_data += "\n";
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

void server::CloseSocket() {
    close(sd);
}

int main() {
    server serv;
    if (serv.InitSock() == false) {
        cout << "初始化失败" << endl;
    }

    if ((serv.sd = serv.BindListen()) == -1) {
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
                        Message res = serv.ProcessConnection(msg.type, msg.msg);
                        // Message res(9, "Request received!");
                        // switch (msg.type) {
                        //     case 0:
                        //         res.type = 0;
                        //         res.msg = "Match Request!";
                        //         // serv.ProcessConnection()
                        //         break;
                        //     case 1:
                        //         res.type = 1;
                        //         res.msg = "Add Request!";
                        //         break;
                        //     case 2:
                        //         res.type = 2;
                        //         res.msg = "Delete Request!";
                        //         break;
                        //     default:
                        //         res.type = -1;
                        //         res.msg = "Wrong Request!!!";
                        //         break;
                        // }
                        send(i, (char *)&res, sizeof(Message), 0);
                        break;
                    }
                }
            }
        }
    }
    serv.CloseSocket();
    return 0;
}