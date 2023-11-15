#include <arpa/inet.h>

struct patricia_node {
    patricia_node *left, *right;
    in6_addr address; // IPv6アドレス
    int sub_bits_len; // このノードで比較するビットの位置
    int is_prefix; // このノードがプレフィックスを表すかどうか
} ;

patricia_node* create_node(struct in6_addr address, int bit, int is_prefix);