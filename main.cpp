#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "patricia6.h"

int main() {

    printf("Starting test code...\n");

    in6_addr sample_all_1;
    inet_pton(AF_INET6, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", &sample_all_1);

    in6_addr sample_all_0;
    inet_pton(AF_INET6, "0000:0000:0000:0000:0000:0000:0000:0000", &sample_all_0);

    in6_addr sample_all_half;
    inet_pton(AF_INET6, "ffff:ffff:ffff:ffff:0000:0000:0000:0000", &sample_all_half);

    for (int i = 0; i < 128; i++) {
        if (in6_addr_get_bit(sample_all_1, i) != 1) { // in6_addr_get_bitですべて1が返ってくることをtest
            fprintf(stderr, "Error in in6_addr_get_bit(sample_all_1, i)\n");
            exit(EXIT_FAILURE);
        }

        if (in6_addr_get_bit(sample_all_half, i) != (i < 64)) { // in6_addr_get_bitで半分1が返ってくることをtest
            fprintf(stderr, "Error in in6_addr_get_bit(sample_all_half, i)\n");
            exit(EXIT_FAILURE);
        }
    }

    int test01 = in6_addr_get_match_bits_len(sample_all_1, sample_all_0, 127);
    if (test01 != 0) {
        fprintf(stderr, "Error in in6_addr_get_match_bits_len(sample_all_1, sample_all_0, 127) = %d\n", test01);
        exit(EXIT_FAILURE);
    }

    int test11 = in6_addr_get_match_bits_len(sample_all_1, sample_all_1, 127);
    if (test11 != 128) {
        fprintf(stderr, "Error in in6_addr_get_match_bits_len(sample_all_1, sample_all_0, 127) = %d\n", test11);
        exit(EXIT_FAILURE);
    }

    int test1half = in6_addr_get_match_bits_len(sample_all_1, sample_all_half, 127);
    if (test1half != 64) {
        fprintf(stderr, "Error in in6_addr_get_match_bits_len(sample_all_1, sample_all_half, 127) = %d\n", test1half);
        exit(EXIT_FAILURE);
    }

    in6_addr root_node_addr;
    memset(&root_node_addr, 0, sizeof(root_node_addr));
    patricia_node *root_node = create_patricia_node(root_node_addr, 0, false, nullptr);

    patricia_node *res_test_lu0 = patricia_trie_search(root_node, sample_all_0);
    patricia_node *res_test_lu1 = patricia_trie_search(root_node, sample_all_1);

    if (res_test_lu0 != nullptr) {
        fprintf(stderr, "Error in patricia_trie_search(root, sample_all_0)\n");
    }

    if (res_test_lu1 != nullptr) {
        fprintf(stderr, "Error in patricia_trie_search(root, sample_all_1)\n");
    }

    // --------- ここまでテスト ---------

    in6_addr sample_all_f4;
    inet_pton(AF_INET6, "ffff::0", &sample_all_f4);

    in6_addr sample_all_f8;
    inet_pton(AF_INET6, "ffff:ffff::0", &sample_all_f8);

    in6_addr sample_all_f12;
    inet_pton(AF_INET6, "ffff:ffff:ffff::0", &sample_all_f12);

    in6_addr sample_i1;
    inet_pton(AF_INET6, "2001:0db8:85a3:0000:0100:8a2e:0370:7334", &sample_i1);

    in6_addr sample_i2;
    inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", &sample_i2);

    in6_addr sample_i3;
    inet_pton(AF_INET6, "2001:0db8:0000:0001:0000:0000:0000:0000", &sample_i3);

    in6_addr sample_i4;
    inet_pton(AF_INET6, "2001:0db8:0000:0009:0000:0000:0000:0000", &sample_i4);

    in6_addr sample_i5;
    inet_pton(AF_INET6, "4000:0000:0000:0000:0000:0000:0000:0000", &sample_i5);

    patricia_trie_insert(root_node, sample_all_f4, 16);

    patricia_trie_insert(root_node, sample_all_f8, 32);

    patricia_trie_insert(root_node, sample_all_0, 64); // 0の方向に64ビット分木をのばす
    patricia_trie_insert(root_node, sample_all_1, 64); // 1の方向に64ビット分木をのばす

    patricia_trie_insert(root_node, sample_all_half, 128);

    patricia_trie_insert(root_node, sample_i3, 64);
    patricia_trie_insert(root_node, sample_i3, 64);
    patricia_trie_insert(root_node, sample_i4, 64);

    patricia_trie_insert(root_node, sample_all_0, 64);

    patricia_trie_insert(root_node, sample_i5, 64);
    patricia_trie_insert(root_node, sample_i5, 64);

    printf("%s\n", in6_addr_to_bits_string(sample_all_half, 0, 127));

    // 検索テスト
    patricia_node *result = patricia_trie_search(root_node, sample_i1);
    if (result != nullptr) {
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &result->address, str, INET6_ADDRSTRLEN);
        printf("Found: %s\n", str);
    } else {
        printf("Not found\n");
    }

    dump_patricia_trie_text(root_node);

    dump_patricia_trie_dot(root_node);

    return 0;
}