#include <stdio.h>

#define EMPTY -1

typedef struct vEBTree {
    int universe_size;
    int min;
    int max;
    struct vEBTree* summary;
    struct vEBTree** clusters;
} vEBTree;