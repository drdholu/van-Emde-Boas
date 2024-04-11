#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "veb.h"

vebtree* create(int universeSize) {
    vebtree* V = (vebtree*)malloc(sizeof(vebtree));
    V->universeSize = universeSize;
    V->min = EMPTY;
    V->max = EMPTY;
    if (universeSize <= 2) {
        V->summary = NULL;
        V->cluster = NULL;
    } else {
        int clusters =  1 << (int)(ceil(log2(sqrt(universeSize))));
        V->summary = create(clusters);
        V->cluster = (vebtree**)malloc(clusters * sizeof(vebtree*));
        for (int i = 0; i < clusters; ++i)
            V->cluster[i] = create(clusters);
    }
    return V;
}

int high(struct vebtree* V, int x) {
    return x / (int)sqrt(V->universeSize);
}

int low(struct vebtree* V, int x) {
    return x % (int)sqrt(V->universeSize);

}

int veb_index(struct vebtree* V, int i, int j) {
    int clusters = (int)sqrt(V->universeSize);
    return i * clusters + j;
}

int minimum(struct vebtree* V) {
    return V->min;
}


int maximum(struct vebtree* V) {
    return V->max;
}

void insert(struct vebtree* V, int x) {
    if (V == NULL) {
        return;
    }
    if (V->min == EMPTY) {
        V->min = V->max = x;
    } else {
        if (x < V->min)
            swap(&x, &(V->min));
        if (V->universeSize > 2) {
            if (V->cluster[high(V, x)]->min == EMPTY) {
                insert(V->summary, high(V, x));
                insert(V->cluster[high(V, x)], low(V, x));
            } else {
                insert(V->cluster[high(V, x)], low(V, x));
            }
        }
        if (x > V->max)
            V->max = x;
    }
}

void deleteVEB(struct vebtree* V, int x) {
    if (V->min == V->max) {
        V->min = V->max = EMPTY;
    } else if (V->universeSize == 2) {
        if (x == 0) {
            V->min = 1;
        } else {
            V->min = 0;
            V->max = 0;
        }
    } else {
        if (x == V->min) {
            int firstCluster = minimum(V->summary);
            x = veb_index(V, firstCluster, minimum(V->cluster[firstCluster]));
            V->min = x;
        }
        delete(V->cluster[high(V, x)], low(V, x));
        if (minimum(V->cluster[high(V, x)]) == EMPTY) {
            delete(V->summary, high(V, x));
            if (x == V->max) {
                int summaryMax = maximum(V->summary);
                if (summaryMax == EMPTY)
                    V->max = V->min;
                else
                    V->max = veb_index(V, summaryMax, maximum(V->cluster[summaryMax]));
            }
        } else if (x == V->max) {
            V->max = veb_index(V, high(V, x), maximum(V->cluster[high(V, x)]));
        }
    }
}

int predecessor_index(struct vebtree* V, int x) {
    if (V->universeSize == 2) {
        if (x == 1 && V->min == 0)
            return 0;
        else
            return EMPTY;
    } else if (V->max != EMPTY && x > V->max) {
        return V->max;
    } else {
        int minLow = minimum(V->cluster[high(V, x)]);
        if (minLow != EMPTY && low(V, x) > minLow) {
            int offset = predecessor_index(V->cluster[high(V, x)], low(V, x));
            return veb_index(V, high(V, x), offset);
        } else {
            int predCluster = predecessor_index(V->summary, high(V, x));
            if (predCluster == EMPTY) {
                if (V->min != EMPTY && x > V->min)
                    return V->min;
                else
                    return EMPTY;
            } else {
                int offset = maximum(V->cluster[predCluster]);
                return veb_index(V, predCluster, offset);
            }
        }
    }
}


void dealloc_vebtree(struct vebtree* V) {
    if (V != NULL) {
        dealloc_vebtree(V->summary);
        if (V->cluster != NULL) {
            int clusters = (int)sqrt(V->universeSize);
            for (int i = 0; i < clusters; ++i) {
                dealloc_vebtree(V->cluster[i]);
            }
            free(V->cluster);
        }
        free(V);
    }
}

void read_branch_data_from_csv(Branch branches[], int* num_branches) {
    FILE* file = fopen(CSV_FILENAME, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", CSV_FILENAME);
        exit(1);
    }

    // Skip header
    char buffer[1024];
    fgets(buffer, sizeof(buffer), file);

    // Read branch data
    *num_branches = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && *num_branches < 10) {
        char name[50];
        int cutoff, seats;
        if (sscanf(buffer, "%49[^,],%d,%d", name, &cutoff, &seats) == 3) {
            strcpy(branches[*num_branches].name, name);
            branches[*num_branches].cutoff = cutoff;
            branches[*num_branches].seats = seats;
            (*num_branches)++;
        } else {
            printf("Error reading data from CSV file.\n");
        }
    }
    fclose(file);

    printf("Read %d branches:\n", *num_branches);
    for (int i = 0; i < *num_branches; i++) {
        printf("Branch %d: Name: %s, Cutoff: %d, Seats: %d\n", i+1, branches[i].name, branches[i].cutoff, branches[i].seats);
    }
}