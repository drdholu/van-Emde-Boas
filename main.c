#include <stdio.h>
#include "veb.c"

int main() {
    vEBTree* tree = create_vEBTree(16); // u = 16
    vEB_insert(tree, 3);
    vEB_insert(tree, 10);
    vEB_insert(tree, 4);
    vEB_insert(tree, 12);
    printf("%d\n", vEB_predecessor(tree, 7)); 
    printf("%d\n", vEB_successor(tree, 12));
    return 0;
}