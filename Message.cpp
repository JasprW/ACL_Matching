#include "message.h"
#include <string.h>
#include <cstring>
#include <string>
using namespace std;

Message::Message(int id, const char* str) : type(id) {
    strcpy(msg, str);
}