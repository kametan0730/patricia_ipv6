#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>

#include "patricia6.h"

patricia_node* create_node(struct in6_addr address, int bit, int is_prefix) {
    patricia_node* node = (patricia_node*)malloc(sizeof(patricia_node));
    if (node == NULL) {
        fprintf(stderr, "failed to malloc for patricia node\n");
        return nullptr;
    }

    node->left = node->right = NULL;
    node->address = address;
    node->sub_bits_len = bit;
    node->is_prefix = is_prefix;
    
    return node;
}

int get_bit(struct in6_addr address, int bit) {
    int byte_index = bit / 8;
    int bit_index = 7 - (bit % 8);
    return (address.s6_addr[byte_index] >> bit_index) & 0b1;
}

// ビットのmatchしてるindexを返す(nビット目までは等しい)
int get_in6_bits_match_index(in6_addr addr1, in6_addr addr2, int start_bit, int end_bit){

    // ビットは、0ビット目, 1ビット目と数える
    assert(start_bit < end_bit);
    assert(end_bit < 128);
    assert(start_bit >= 0);

    for(int i=start_bit;i<end_bit;i++){
        if(get_bit(addr1, i) != get_bit(addr2, i)) return i;
    }

    return end_bit;
}

patricia_node* insert(patricia_node *root, struct in6_addr address, int prefix_len) {

    int current_bits_len = 0;
    patricia_node* current_node = root;

    // 枝を辿る
    while(current_bits_len <= prefix_len){

        patricia_node** next_node_ptr;
        int match_index = get_in6_bits_match_index(address, current_node->address, current_bits_len, current_bits_len+current_node->sub_bits_len);

        if(match_index == current_bits_len + current_node->sub_bits_len){ // 次のノードと全マッチ
            current_bits_len += current_node->sub_bits_len;
            next_node_ptr = (get_bit(address, current_bits_len) == 0) ? &current_node->left : &current_node->right;
            if(*next_node_ptr == nullptr){
                *next_node_ptr = create_node(address,  prefix_len - current_bits_len, true); // ノードを作成
                current_node = *next_node_ptr;
                break;
            }
            current_node = *next_node_ptr;

        }else if(match_index == current_bits_len){ // 次のノードと全アンマッチ



        }else{ // 次のノードと途中までマッチ

            // 途中にノードを作成する必要がある
            
            if(get_bit(current_node->address, 1) == 0){
                *next_node_ptr = current_node->left;
            }else{
                *next_node_ptr = current_node->right;
            }

            // 途中に新しいノードを作成
            patricia_node* exists_next_node = *next_node_ptr;
            *next_node_ptr = create_node(current_node->address, current_node->sub_bits_len - 4, current_node->is_prefix);

            // 既存ノードの変更
            current_node->sub_bits_len -= current_bits_len + current_node->sub_bits_len - match_index;
        }

        current_node->address
        
    }







    patricia_node* current = root;
    patricia_node* parent = NULL;

    // 挿入位置を見つける
    while (current != NULL && current->bit < prefix_len) {
        parent = current;
        current = get_bit(address, current->bit) ? current->right : current->left;
    }

    // 新しいノードを挿入
    patricia_node* creation_node = create_node(address, prefix_len, true);
    if (parent == NULL) {
        root = creation_node;
    } else if (get_bit(address, parent->bit)) {
        parent->right = creation_node;
    } else {
        parent->left = creation_node;
    }

    // 新しいノードの子を設定
    creation_node->left = (creation_node == parent) ? NULL : creation_node;
    creation_node->right = (creation_node == parent) ? NULL : creation_node;

    return root;
}

// IPv6アドレスを検索する関数
patricia_node* search(patricia_node *root, in6_addr address) {

    int current_bit = 0;

    patricia_node* current_node = root;
    patricia_node* last_matched = nullptr;

    while(current_node != nullptr && current_bit < 128){

        if(current_node->is_prefix){
            last_matched = current_node;
        }

        current_bit += 

        current_node = (get_bit(address, current_bit) == 0) ? current_node->left : current_node->right;
    }

    return last_matched;



    while (current_node != NULL && current->bit < 128) {
        if (current->is_prefix) {
            lastMatched = current;
        }
        current = get_bit(address, current->bit) ? current->right : current->left;
    }

    return lastMatched;
}

