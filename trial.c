#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EMPTY -1
#define MAX_PREFS 3
#define CSV_FILENAME "data.csv"

typedef struct Branch {
    int cutoff;
    int seats;
    char name[50];
} Branch;

typedef struct Student {
    int roll;
    int marks;
    char preference[MAX_PREFS][50];
    char branchAllocated[50];
} Student;

typedef struct vebtree {
    int universeSize;
    int min;
    int max;
    struct vebtree* summary;
    struct vebtree** cluster;
} vebtree;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

vebtree* create(int universeSize);
int high(struct vebtree* V, int x);
int low(struct vebtree* V, int x);
int veb_index(struct vebtree* V, int i, int j);
void insert(struct vebtree* V, int x);
void delete(struct vebtree* V, int x);
int minimum(struct vebtree* V);
int successor(struct vebtree* V, int x);
void dealloc_vebtree(struct vebtree* V);
void read_branch_data_from_csv(Branch branches[], int* num_branches);
void allocate_branch(Student* student, Branch branches[], int num_branches, vebtree* seat_availability_veb, vebtree* cutoff_met_veb);


vebtree* create(int universeSize) {
    vebtree* V = (vebtree*)malloc(sizeof(vebtree));
    V->universeSize = universeSize;
    V->min = EMPTY;
    V->max = EMPTY;
    if (universeSize <= 2) {
        V->summary = NULL;
        V->cluster = NULL;
    } else {
        int clusters = (int)sqrt(universeSize);
        V->summary = create(clusters);
        V->cluster = (vebtree**)malloc(clusters * sizeof(vebtree*));
        for (int i = 0; i < clusters; ++i)
            V->cluster[i] = create(clusters);
    }
    return V;
}

int high(struct vebtree* V, int x) {
    int clusters = (int)sqrt(V->universeSize);
    return x / clusters;
}

int low(struct vebtree* V, int x) {
    int clusters = (int)sqrt(V->universeSize);
    return x % clusters;
}

int veb_index(struct vebtree* V, int i, int j) {
    int clusters = (int)sqrt(V->universeSize);
    return i * clusters + j;
}

void insert(struct vebtree* V, int x) {
    if (V->min == EMPTY) {
        V->min = V->max = x;
    } else {
        if (x < V->min)
            swap(&x, &(V->min));
        if (V->universeSize > 2) {
            if (V->summary->min == EMPTY) {
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

void delete(struct vebtree* V, int x) {
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
        if (V->cluster[high(V, x)]->min == EMPTY) {
            delete(V->summary, high(V, x));
            if (x == V->max) {
                int summaryMax = V->summary->max;
                if (summaryMax == EMPTY)
                    V->max = V->min;
                else
                    V->max = veb_index(V, summaryMax, V->cluster[summaryMax]->max);
            }
        } else if (x == V->max) {
            V->max = veb_index(V, high(V, x), V->cluster[high(V, x)]->max);
        }
    }
}

int minimum(struct vebtree* V) {
    return V->min;
}

int successor(struct vebtree* V, int x) {
    if (V->universeSize == 2) {
        if (x == 0 && V->max == 1)
            return 1;
        else
            return EMPTY;
    } else if (V->min != EMPTY && x < V->min) {
        return V->min;
    } else {
        int maxLow = V->cluster[high(V, x)]->max;
        if (maxLow != EMPTY && low(V, x) < maxLow) {
            int offset = successor(V->cluster[high(V, x)], low(V, x));
            return veb_index(V, high(V, x), offset);
        } else {
            int succCluster = successor(V->summary, high(V, x));
            if (succCluster == EMPTY)
                return EMPTY;
            else {
                int offset = minimum(V->cluster[succCluster]);
                return veb_index(V, succCluster, offset);
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
        }
    }
    fclose(file);
}

int find_branch_index(Branch branches[], int num_branches, const char* branchName) {
    for (int i = 0; i < num_branches; ++i) {
        if (strcmp(branches[i].name, branchName) == 0) {
            return i;
        }
    }
    return -1; // Branch not found
}

void initialize_seat_availability(Branch branches[], int num_branches, vebtree* seat_availability_veb) {
    for (int i = 0; i < num_branches; i++) {
        if (branches[i].seats > 0) {
            insert(seat_availability_veb, i); // Mark the seat as available
        }
    }
}

void initialize_cutoff_met(Branch branches[], int num_branches, vebtree* cutoff_met_veb) {
    for (int i = 0; i < num_branches; i++) {
        if (branches[i].cutoff == 0) {
            insert(cutoff_met_veb, i); // Mark the cutoff condition as met
        }
    }
}

void allocate_branch(Student* student, Branch branches[], int num_branches, vebtree* seat_availability_veb, vebtree* cutoff_met_veb) {
    // Initialize allocated branch as empty
    strcpy(student->branchAllocated, "None");

    // Find the highest preference branch with cutoff less than or equal to student marks
    for (int i = 0; i < MAX_PREFS; i++) {
        for (int j = 0; j < num_branches; j++) {
            if (strcmp(student->preference[i], branches[j].name) == 0 && successor(seat_availability_veb, j) != EMPTY &&
                student->marks >= branches[j].cutoff && successor(cutoff_met_veb, j) != EMPTY) {
                // Allocate branch if it's found, seats are available, and cutoff is met
                strcpy(student->branchAllocated, branches[j].name);
                delete(seat_availability_veb, j); // Mark the seat as allocated in the seat availability VEB tree
                insert(cutoff_met_veb, j); // Mark cutoff as met in the cutoff VEB tree
                return;
            }
        }
    }
}

int main() {
    Branch branches[10];
    int num_branches;
    read_branch_data_from_csv(branches, &num_branches);

    vebtree* seat_availability_veb = create(num_branches);
    vebtree* cutoff_met_veb = create(num_branches);

    // printf("seat availibilty and cutoff_met_veb Working");

    initialize_seat_availability(branches, num_branches, seat_availability_veb);
    initialize_cutoff_met(branches, num_branches, cutoff_met_veb);

    // printf("Initialization Working");
    
    int count = 3;
    do {
        // Input student marks and preferences
        Student student;
        printf("Enter student roll number: ");
        scanf("%d", &student.roll);
        printf("Enter student marks: ");
        scanf("%d", &student.marks);
        printf("Enter student branch preferences (up to 3 branches):\n");
        for (int i = 0; i < MAX_PREFS; i++) {
            printf("Preference %d: ", i + 1);
            scanf("%s", student.preference[i]);
        }

        // Allocate branch
        allocate_branch(&student, branches, num_branches, seat_availability_veb, cutoff_met_veb);

        printf("Allocation working");

        // Print allocated branch
        if (strcmp(student.branchAllocated, "None") != 0) {
            printf("Allocated branch for student with roll number %d is %s \n", student.roll, student.branchAllocated);
            count--;
        } else {
            printf("No Branch Allocated for student with roll number %d \n", student.roll);
        }

        // Print remaining seats
        printf("Remaining seats: ");
        for (int i = 0; i < num_branches; i++) {
            if (successor(seat_availability_veb, i) != EMPTY) {
                printf("%s: %d ", branches[i].name, branches[i].seats);
            }
        }
        printf("\n");
    } while (count > 0);

    dealloc_vebtree(seat_availability_veb);
    dealloc_vebtree(cutoff_met_veb);

    return 0;
}
