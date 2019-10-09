#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_TYPE_MATCH 0
#define MESSAGE_TYPE_ADD 1
#define MESSAGE_TYPE_DEL 2
#define MAX_STR 256

class Message {
   public:
    int type;
    char msg[MAX_STR];
    Message(int id = -1, const char* str = "");
};

#endif