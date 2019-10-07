#include <string>

#define MAX_PACK_SIZE 10240
#define MAX_FILE_NAME_LENGTH 256
#define MESSAGE_TYPE_MATCH 0
#define MESSAGE_TYPE_ADD 1
#define MESSAGE_TYPE_DEL 2

class Message {
   public:
    int type;
    std::string msg;
    Message(int id = -1, std::string str = "") : type(id), msg(str){};
    // int getType() { return type; }
    // std::string getMsg() { return msg; }
};