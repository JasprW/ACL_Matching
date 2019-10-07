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
    char recvline[MAXLINE], sendline[MAXLINE];
    struct sockaddr_in servaddr;
    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Create socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s (errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    while (1) {
        fputs("Input message(Q for quit): ", stdout);
        fgets(sendline, MAXLINE, stdin);

        if (!strcmp(sendline, "q\n") || !strcmp(sendline, "Q\n")) {
            break;
        } else if (sendline[1] != ' ' && !(sendline[0] >= '0' && sendline[0] <= '3')) {
            cout << "Usage: " << endl;
            continue;
        } else {
            // TODO
            string str = "";
            for (int i = 2; i < MAXLINE; i++) {
                if (sendline[i] != '\n') {
                    str += sendline[i];
                } else {
                    break;
                }
            }
            Message msg(sendline[0] - '0', str);
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