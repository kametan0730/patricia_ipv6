#ifndef CURO_PATRICIA_TRIE_H
#define CURO_PATRICIA_TRIE_H

#include <arpa/inet.h>

struct patricia_node {
    patricia_node *left, *right, *parent;
    in6_addr address; // IPv6アドレス
    int bits_len; // このノードで比較するビットの位置
    int is_prefix; // このノードがプレフィックスを表すかどうか
    void* data;
} ;

int in6_addr_get_bit(struct in6_addr address, int bit);
int get_in6_addr_match_bits_len(in6_addr addr1, in6_addr addr2, int start_bit, int end_bit);

patricia_node* create_node(struct in6_addr address, int bits_len, int is_prefix, patricia_node *parent);
patricia_node* search(patricia_node *root, in6_addr address);
patricia_node* insert(patricia_node *root, struct in6_addr address, int prefix_len);
void dump_dot_patricia_trie(patricia_node *root);
void dump_patricia_trie(patricia_node *root);

#endif