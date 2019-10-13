# C++ ACL rule matching server
* Use Trie for rule storage.
* Use select for I/O reuse.
* Query, add and delete rule from client.
* Use port 6666 as default.


## Usage
### server
```
g++ server.cpp trie.cpp message.cpp -o server
./server
```
### client
```
g++ client.cpp message.cpp -o client
./client <IP-Address>
```
### request
```
<request_type> <message_string>
```
<request_type>: 0 for query, 1 for add, 2 for delete
<message_string>: URL or rule to query, add or delete.