#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <queue>

#include "patricia6.h"
#include <unistd.h>

// ビットは0から127
// 指定したビットを取得する
int in6_addr_get_bit(in6_addr address, int bit) {

    assert(bit < 128);
    assert(bit >= 0);
    int byte_index = bit / 8;
    int bit_index = 7 - (bit % 8);
    return (address.s6_addr[byte_index] >> bit_index) & 0b01;
}

// 指定したビットを0にする
int in6_addr_clear_bit(in6_addr *address, int bit) {

    assert(bit < 128);
    assert(bit >= 0);
    int byte_index = bit / 8;
    int bit_index = 7 - (bit % 8);
    address->s6_addr[byte_index] &= ~(0b01 << bit_index) ;
}

// 2つのアドレスを比べ、ビット列のマッチしてる長さを返す
int get_in6_addr_match_bits_len(in6_addr addr1, in6_addr addr2, int start_bit, int end_bit){

    // ビットは、0ビット目, 1ビット目と数える
    assert(start_bit < end_bit);
    assert(end_bit < 128);
    assert(start_bit >= 0);

    int count = 0;

    for(int i=0; i<=end_bit; i++){
        if(in6_addr_get_bit(addr1, i) != in6_addr_get_bit(addr2, i)) return count;
        count++;
    }

    return count;
}

// IPアドレスを、プレフックス長でクリアする
in6_addr in6_addr_clear_prefix(in6_addr addr, int prefix_len){

    for(int i=prefix_len; i<128; i++){
        in6_addr_clear_bit(&addr, i);
    }
    return addr;
}

// ノードを作成
patricia_node* create_node(struct in6_addr address, int bits_len, int is_prefix, patricia_node *parent) {
    
    patricia_node* node = (patricia_node*) malloc(sizeof(patricia_node));
    if (node == NULL) {
        fprintf(stderr, "failed to malloc for patricia node\n");
        return nullptr;
    }

    node->parent = parent;
    node->left = node->right = nullptr;
    node->address = address;
    node->bits_len = bits_len;
    node->is_prefix = is_prefix;
    
    return node;
}


// トライ木からIPアドレスを検索する
patricia_node* search(patricia_node *root, in6_addr address) {

    printf("Entering search\n");


    int current_bits_len = 0;
    patricia_node* current_node = root;
    patricia_node* next_node = nullptr;
    patricia_node* last_matched = nullptr;

    while(current_bits_len < 128){

        next_node = (in6_addr_get_bit(address, current_bits_len) == 0) ? current_node->left : current_node->right;

        if(next_node == nullptr){
            break;
        }

        int match_len = get_in6_addr_match_bits_len(address, next_node->address, current_bits_len, current_bits_len + next_node->bits_len-1);
        
        printf("Mach %d\n", match_len);
        
        if(next_node->bits_len != 0 and match_len != current_bits_len + next_node->bits_len){
            break;
        }

        if(next_node->is_prefix){
            last_matched = next_node;
        }

        current_node = next_node;
        current_bits_len += next_node->bits_len;
    }

    printf("Exited search\n");

    return last_matched;

}

// トライ木にエントリを追加する
patricia_node* insert(patricia_node *root, struct in6_addr address, int prefix_len) {

    int current_bits_len = 0;
    patricia_node* current_node = root;
    patricia_node* next_node;

    // 引数で渡された情報をきれいにする
    address = in6_addr_clear_prefix(address, prefix_len);

    // 枝を辿る
    while(true){

        if(in6_addr_get_bit(address, current_bits_len) == 0){ // 現在のノードから次に進むノードを決める
            next_node = current_node->left;
            if(next_node == nullptr){ // ノードを作成
                current_node->left = create_node(in6_addr_clear_prefix(address, current_bits_len + prefix_len - current_bits_len), prefix_len - current_bits_len, true, current_node);
                break;
            }
        }else{
            next_node = current_node->right;
            if(next_node == nullptr){ // ノードを作成
                current_node->right = create_node(in6_addr_clear_prefix(address, current_bits_len + prefix_len - current_bits_len), prefix_len - current_bits_len, true, current_node);
                break;
            }
        }

        int match_len = get_in6_addr_match_bits_len(address, next_node->address, 0, current_bits_len+next_node->bits_len-1);

        printf("Compare %d vs %d\n", match_len, current_bits_len + current_node->bits_len);
        if(match_len == current_bits_len + current_node->bits_len){ // 次のノードと全マッチ
            current_bits_len += next_node->bits_len;
            current_node = next_node;

        }else{ // 次のノードと途中までマッチ

            // 途中にノードを作成する必要がある
            int new_node_bits_len = match_len - current_bits_len;
            printf("Creation node len = %d\n", new_node_bits_len);
            patricia_node *new_node = create_node(in6_addr_clear_prefix(address, current_bits_len + new_node_bits_len), new_node_bits_len, false, current_node); // 新しく作る

            if(current_node->left == next_node){ // 上をつなぎなおす
                current_node->left = new_node;
            }else{
                current_node->right = new_node;
            }

            next_node->bits_len -= (match_len - current_bits_len);
            next_node->address = in6_addr_clear_prefix(next_node->address, current_bits_len + next_node->bits_len);
            next_node->parent = new_node;

            if(in6_addr_get_bit(current_node->address, match_len) == 0){ // 下をつなぎなおす&目的のノードを作る
                new_node->left = next_node;
                new_node->right = create_node(in6_addr_clear_prefix(address, prefix_len - match_len), prefix_len - current_bits_len - match_len, true, new_node);
            }else{
                new_node->right = next_node;
                new_node->left = create_node(in6_addr_clear_prefix(address, prefix_len - match_len), prefix_len - current_bits_len - match_len, true, new_node);
            }

            break;
        }        
    }

    return root;
}


int get_prefix_len(patricia_node *node){
    int sum = 0;
    patricia_node *current = node;
    while(true){
        sum += current->bits_len;
        if(current->parent == nullptr){
            break;
        }
        current = current->parent;
    }
    return sum;
}


int get_node_count(patricia_node *node){
    int sum = 0;
    patricia_node *current = node;
    while(true){
        if(current->parent == nullptr){
            break;
        }
        sum += 1;
        current = current->parent;
    }
    return sum;
}

// IPv6アドレスを文字列に変換する関数
const char *ipv6_to_string(const struct in6_addr *addr, char *str, size_t size) {
    inet_ntop(AF_INET6, addr, str, size);
    return str;
}

// DOT言語でIPv6パトリシア木を出力する再帰関数
void print_dot_patricia_trie(struct patricia_node *node, FILE *output) {
    if (node == NULL) {
        return;
    }

    char addr_str[INET6_ADDRSTRLEN];
    ipv6_to_string(&node->address, addr_str, sizeof(addr_str));

    // ノードを出力
    fprintf(output, "  \"%p\" [label=\"%s/%d\"];\n", (void *)node, addr_str, node->bits_len);

    // 左の子ノードを処理
    if (node->left) {
        fprintf(output, "  \"%p\" -> \"%p\" [label=\"Left\"];\n", (void *)node, (void *)(node->left));
        print_dot_patricia_trie(node->left, output);
    }

    // 右の子ノードを処理
    if (node->right) {
        fprintf(output, "  \"%p\" -> \"%p\" [label=\"Right\"];\n", (void *)node, (void *)(node->right));
        print_dot_patricia_trie(node->right, output);
    }
}


void dump_dot_patricia_trie(patricia_node *root){

    FILE *output = fopen("patricia_trie.dot", "w");
    if (output == NULL) {
        perror("ファイルを開けませんでした");
        return;
    }

    fprintf(output, "digraph PatriciaTrie {\n");

    print_dot_patricia_trie(root, output);

    fprintf(output, "}\n");

    fclose(output);
}



void dump_patricia_trie(patricia_node *root) {
    
    patricia_node *current_node;
    std::queue<patricia_node *> node_queue;
    node_queue.push(root);

    while (!node_queue.empty()) {
        current_node = node_queue.front();
        node_queue.pop();

        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(current_node->address), str, INET6_ADDRSTRLEN);

        printf("%s/%d (%d) %d nodes - %s\n", str, get_prefix_len(current_node), current_node->bits_len,get_node_count(current_node), current_node->is_prefix  ? "prefix" : "not prefix");

        if (current_node->left != nullptr) {
            node_queue.push(current_node->left);
        }
        if (current_node->right != nullptr) {
            node_queue.push(current_node->right);
        }
    }
}