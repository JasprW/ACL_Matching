// #include <sys/socket.h>
// #include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "Message.h"
#include "trie.h"

class Server {
   public:
    int sd;

    bool InitTrie();
    bool InitSock();                                           //初始socket
    int BindListen();                                          //绑定监听套接字
    int AcceptConnection(int sd);                              //接收客户端
    Message ProcessConnection(int type, std::string request);  //传送数据
    void CloseSocket();                                        //关闭套接字

   private:
    Trie* trie;
    int rule_num;
    // std::vector<std::string> storage;
    bool Match(std::string s);
    bool Add(std::string s);
    bool Del(std::string s);
    void _DelLine(int line);
};
