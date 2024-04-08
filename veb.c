#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "veb.h"

vEBTree* create_vEBTree(int universe_size) {
    vEBTree* tree = (vEBTree*)malloc(sizeof(vEBTree));
    tree->universe_size = universe_size;
    tree->min = EMPTY;
    tree->max = EMPTY;

    if (universe_size <= 2) {
        tree->summary = NULL;
        tree->clusters = NULL;
    } else {
        int sqrt_size = 1 << (int)(ceil(log2(sqrt(universe_size))));
        tree->summary = create_vEBTree(sqrt_size);
        tree->clusters = (vEBTree**)malloc(sizeof(vEBTree*) * sqrt_size);
        for (int i = 0; i < sqrt_size; i++) {
            tree->clusters[i] = create_vEBTree(sqrt_size);
        }
    }

    return tree;
}

int high(vEBTree* tree, int x) {
    return x / (int)sqrt(tree->universe_size);
}

int low(vEBTree* tree, int x) {
    return x % (int)sqrt(tree->universe_size);
}

int get_index(vEBTree* tree, int x, int y) {
    return x * (int)sqrt(tree->universe_size) + y;
}

void vEB_insert(vEBTree* tree, int x) {
    if (tree->min == EMPTY) {
        tree->min = tree->max = x;
    } else {
        if (x < tree->min) {
            int temp = tree->min;
            tree->min = x;
            x = temp;
        }
        if (tree->universe_size > 2) {
            if (tree->clusters[high(tree, x)]->min == EMPTY) {
                vEB_insert(tree->summary, high(tree, x));
                tree->clusters[high(tree, x)]->min = low(tree, x);
                tree->clusters[high(tree, x)]->max = low(tree, x);
            } else {
                vEB_insert(tree->clusters[high(tree, x)], low(tree, x));
            }
        }
        if (x > tree->max) {
            tree->max = x;
        }
    }
}

void vEB_delete(vEBTree* tree, int x) {
    if (tree->min == tree->max) {
        tree->min = tree->max = EMPTY;
    } else if (tree->universe_size == 2) {
        if (x == 0) {
            tree->min = 1;
        } else {
            tree->min = 0;
        }
        tree->max = tree->min;
    } else {
        if (x == tree->min) {
            int first_cluster = tree->summary->min;
            x = get_index(tree, first_cluster, tree->clusters[first_cluster]->min);
            tree->min = x;
        }
        vEB_delete(tree->clusters[high(tree, x)], low(tree, x));
        if (tree->clusters[high(tree, x)]->min == EMPTY) {
            vEB_delete(tree->summary, high(tree, x));
            if (x == tree->max) {
                int summary_max = tree->summary->max;
                if (summary_max == EMPTY) {
                    tree->max = tree->min;
                } else {
                    tree->max = get_index(tree, summary_max, tree->clusters[summary_max]->max);
                }
            }
        }
    }
}

// int vEB_successor(vEBTree* tree, int x) {
//     if (tree->universe_size == 2) {
//         if (x == 0 && tree->max == 1) {
//             return 1;
//         } else {
//             return EMPTY;
//         }
//     } else if (tree->min != EMPTY && x < tree->min) {
//         return tree->min;
//     } else {
//         int high_x = high(tree, x);
//         int low_x = low(tree, x);
//         int max_low = tree->clusters[high_x]->max;
//         if (max_low != EMPTY && low_x < max_low) {
//             int offset = vEB_successor(tree->clusters[high_x], low_x);
//             return get_index(tree, high_x, offset);
//         } else {
//             int succ_cluster;
//             if (tree->summary != NULL && high_x < tree->summary->max) {
//                 succ_cluster = vEB_successor(tree->summary, high_x);
//                 int offset = tree->clusters[succ_cluster]->min;
//                 return get_index(tree, succ_cluster, offset);
//             } else {
//                 return EMPTY;
//             }
//         }
//     }
// }

// Algo from MIT_OCW_VEB
int vEB_successor(vEBTree* tree, int x) {
    if (tree->min != EMPTY && x < tree->min) {
        return tree->min;
    } else {
        int high_x = high(tree, x);
        int low_x = low(tree, x);
        int max_low = tree->clusters[high_x]->max;
        if (max_low != EMPTY && low_x < max_low) {
            int offset = vEB_successor(tree->clusters[high_x], low_x);
            return get_index(tree, high_x, offset);
        } else {
            if (tree->summary != NULL && high_x < tree->summary->max) {
                int succ_cluster = vEB_successor(tree->summary, high_x);
                int offset = tree->clusters[succ_cluster]->min;
                return get_index(tree, succ_cluster, offset);
            } else {
                return EMPTY;
            }
        }
    }
}


// int vEB_predecessor(vEBTree* tree, int x) {
//     if (tree->universe_size == 2) {
//         if (x == 1 && tree->min == 0) {
//             return 0;
//         } else {
//             return EMPTY;
//         }
//     } else if (tree->max != EMPTY && x > tree->max) {
//         return tree->max;
//     } else {
//         int high_x = high(tree, x);
//         int low_x = low(tree, x);
//         int min_low = tree->clusters[high_x]->min;
//         if (min_low != EMPTY && low_x > min_low) {
//             int offset = vEB_predecessor(tree->clusters[high_x], low_x);
//             return get_index(tree, high_x, offset);
//         } else {
//             int pred_cluster;
//             if (tree->summary != NULL && high_x > tree->summary->min) {
//                 pred_cluster = vEB_predecessor(tree->summary, high_x);
//                 int offset = tree->clusters[pred_cluster]->max;
//                 return get_index(tree, pred_cluster, offset);
//             } else {
//                 return tree->min != EMPTY && x > tree->min ? tree->min : EMPTY;
//             }
//         }
//     }
// }

// Algo from MIT_OCW_VEB
int vEB_predecessor(vEBTree* tree, int x) {
    if (tree->max != EMPTY && x > tree->max) {
        return tree->max;
    } else {
        if (tree->min != EMPTY && x < tree->min) {
            return EMPTY;
        } else {
            int high_x = high(tree, x);
            int low_x = low(tree, x);
            int min_low = tree->clusters[high_x]->min;

            if (min_low != EMPTY && low_x > min_low) {
                int offset = vEB_predecessor(tree->clusters[high_x], low_x);
                return get_index(tree, high_x, offset);
            } else {
                if (tree->summary != NULL && high_x > tree->summary->min) {
                    int pred_cluster = vEB_predecessor(tree->summary, high_x);
                    int offset = tree->clusters[pred_cluster]->max;
                    return get_index(tree, pred_cluster, offset);
                } else {
                    return tree->min != EMPTY && x > tree->min ? tree->min : EMPTY;
                }
            }
        }
    }
}




