#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "Message.h"

#define MAXLINE 1024
#define PORT 6666

using namespace std;

int main(int argc, char **argv) {
    int sockfd, str_len, recv_len;
    char recvline[MAXLINE], send_line[MAXLINE];
    struct sockaddr_in serv_addr;
    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Create socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("inet_pton error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("connect error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    while (1) {
        fputs("Input message(Q for quit): ", stdout);
        fgets(send_line, MAXLINE, stdin);

        if (!strcmp(send_line, "q\n") || !strcmp(send_line, "Q\n")) {
            break;
        } else if (send_line[1] != ' ' || !(send_line[0] >= '0' && send_line[0] <= '3')) {
            cout << "Usage: <ACT> <REQUEST>" << endl
                 << "ACT: 0 Query, 1 Add, 2 Delete" << endl
                 << "REQUEST: string" << endl;
            continue;
        } else {
            string str = "";
            for (int i = 2; i < MAXLINE; i++) {
                if (send_line[i] != '\n') {
                    str += send_line[i];
                } else {
                    break;
                }
            }
            Message msg(send_line[0] - '0', str);
            if (msg.type < 0 || msg.type > 2) {
                cout << "Wrong Request!" << endl;
                continue;
            }
            send(sockfd, &msg, sizeof(Message), 0);
            Message res;
            recv(sockfd, (char *)&res, sizeof(Message), 0);
            cout << res.type << " " << res.msg << endl;
        }
    }
    close(sockfd);
    return 0;
}