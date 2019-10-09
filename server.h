#include <string>
#include "message.h"
#include "trie.h"

class Server {
   public:
    int sd;

    bool init_trie();
    bool init_socket();                                         //初始socket
    int bind_listen();                                          //绑定监听套接字
    Message process_connection(int type, std::string request);  //传送数据
    void close_socket();                                        //关闭套接字

   private:
    Trie* trie;
    int rule_num;
    bool query(std::string s);
    bool add(std::string s);
    bool del(std::string s);
    void _del_line(int line);
};
