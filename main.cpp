
#include <cstdint>
#include <cstdio>
#include <arpa/inet.h>
#include <cstdlib>

#include "patricia6.h"
#include <unistd.h>

int main() {

    printf("Starting test code...\n");

    in6_addr sample_all_1;
    inet_pton(AF_INET6, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", &sample_all_1);

    in6_addr sample_all_0;
    inet_pton(AF_INET6, "0000:0000:0000:0000:0000:0000:0000:0000", &sample_all_0);

    in6_addr sample_all_half;
    inet_pton(AF_INET6, "ffff:ffff:ffff:ffff:0000:0000:0000:0000", &sample_all_half);

    for(int i=0;i<128;i++){
        if(in6_addr_get_bit(sample_all_1, i) != 1){ // in6_addr_get_bitですべて1が返ってくることをtest
            fprintf(stderr, "Error in in6_addr_get_bit(sample_all_1, i)\n");
            exit(EXIT_FAILURE);
        }

        if(in6_addr_get_bit(sample_all_half, i) != (i < 64)){ // in6_addr_get_bitで半分1が返ってくることをtest
            fprintf(stderr, "Error in in6_addr_get_bit(sample_all_half, i)\n");
            exit(EXIT_FAILURE);
        }
    }


    int test01 = get_in6_addr_match_bits_len(sample_all_1, sample_all_0, 0, 127);
    if(test01 != 0){
        fprintf(stderr, "Error in get_in6_addr_match_bits_len(sample_all_1, sample_all_0, 0, 127) = %d\n", test01);
        exit(EXIT_FAILURE);
    }

    int test11 = get_in6_addr_match_bits_len(sample_all_1, sample_all_1, 0, 127);
    if(test11 != 128){
       fprintf(stderr, "Error in get_in6_addr_match_bits_len(sample_all_1, sample_all_0, 0, 127) = %d\n", test11);
       exit(EXIT_FAILURE);
    }

    int test1half = get_in6_addr_match_bits_len(sample_all_1, sample_all_half, 0, 127);
    if(test1half != 64){
       fprintf(stderr, "Error in get_in6_addr_match_bits_len(sample_all_1, sample_all_half, 0, 127) = %d\n", test1half);
       exit(EXIT_FAILURE);
    }


    in6_addr root_node_addr;
    root_node_addr.s6_addr[0] = 0;
    patricia_node *root_node = create_node(root_node_addr, 0, false, nullptr);

    patricia_node *res_test_lu0 = search(root_node, sample_all_0);
    patricia_node *res_test_lu1 = search(root_node, sample_all_1);

    if(res_test_lu0 != nullptr){
       fprintf(stderr, "Error in search(root, sample_all_0)\n");
    }

    if(res_test_lu1 != nullptr){
       fprintf(stderr, "Error in search(root, sample_all_1)\n");
    }

    struct in6_addr sample_i1;
    inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", &sample_i1);

    insert(root_node, sample_all_0, 64); // 0の方向に64ビット分木をのばす
    insert(root_node, sample_all_1, 64); // 1の方向に64ビット分木をのばす

    insert(root_node, sample_i1, 128); // 128はプレフィックス長です

    // 検索テスト
    patricia_node* result = search(root_node, sample_i1);
    if (result != NULL) {
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(result->address), str, INET6_ADDRSTRLEN);
        printf("Found: %s\n", str);
    } else {
        printf("Not found.\n");
    }

    // ツリーの解放、他の処理...

    dump_patricia_trie(root_node);

    return 0;
}