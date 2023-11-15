
#include <cstdint>
#include <cstdio>
#include <arpa/inet.h>

#include "patricia6.h"

int main() {

    in6_addr root_addr;
    root_addr.s6_addr[0] = 0;
    patricia_node *root = create_node(root_addr, 0, false);

    struct in6_addr ip1;
    inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", &ip1);
    root = insert(root, ip1, 128); // 128はプレフィックス長です

    // 検索テスト
    patricia_node* result = search(root, ip1);
    if (result != NULL) {
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(result->address), str, INET6_ADDRSTRLEN);
        printf("Found: %s\n", str);
    } else {
        printf("Not found.\n");
    }

    // ツリーの解放、他の処理...

    return 0;
}