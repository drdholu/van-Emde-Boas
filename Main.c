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
int maximum(struct vebtree* V);
int successor(struct vebtree* V, int x);
int predecessor(struct vebtree* V, int x);
void dealloc_vebtree(struct vebtree* V);
void read_branch_data_from_csv(Branch branches[], int* num_branches);
void allocate_branch(Student* student, Branch branches[], int num_branches);

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
            if (minimum(V->cluster[high(V, x)]) == EMPTY) {
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

int minimum(struct vebtree* V) {
    return V->min;
}

int maximum(struct vebtree* V) {
    return V->max;
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
        int maxLow = maximum(V->cluster[high(V, x)]);
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

int predecessor(struct vebtree* V, int x) {
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
            int offset = predecessor(V->cluster[high(V, x)], low(V, x));
            return veb_index(V, high(V, x), offset);
        } else {
            int predCluster = predecessor(V->summary, high(V, x));
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

void initialize_seat_availability(Branch branches[], int num_branches, unsigned int* seat_availability) {
    *seat_availability = 0;
    for (int i = 0; i < num_branches; i++) {
        if (branches[i].seats > 0) {
            *seat_availability |= (1u << i); // Mark the seat as available
        }
    }
}

void initialize_cutoff_met(Branch branches[], int num_branches, unsigned int* cutoff_met) {
    *cutoff_met = 0;
    for (int i = 0; i < num_branches; i++) {
        if (branches[i].cutoff == 0) {
            *cutoff_met |= (1u << i); // Mark the cutoff condition as met
        }
    }
}

void allocate_branch(Student* student, Branch branches[], int num_branches) {
    // Initialize allocated branch as empty
    strcpy(student->branchAllocated, "None");

    // Find the highest preference branch with cutoff less than or equal to student marks
    for (int i = 0; i < MAX_PREFS; i++) {
        for (int j = 0; j < num_branches; j++) {
            if (strcmp(student->preference[i], branches[j].name) == 0 && student->marks >= branches[j].cutoff) {
                // Check if seats are available
                if (branches[j].seats > 0) {
                    strcpy(student->branchAllocated, branches[j].name);
                    branches[j].seats--; // Reduce available seats
                    return;
                }
            }
        }
    }

    // If no seats are available in any preference, try the next preference
    for (int i = 0; i < MAX_PREFS; i++) {
        for (int j = 0; j < num_branches; j++) {
            if (strcmp(student->preference[i], branches[j].name) == 0 && student->marks >= branches[j].cutoff) {
                // Check if seats are available
                if (branches[j].seats > 0) {
                    strcpy(student->branchAllocated, branches[j].name);
                    branches[j].seats--; // Reduce available seats
                    return;
                }
            }
        }
    }
}


int main() {
    Branch branches[10];
    int num_branches;
    read_branch_data_from_csv(branches, &num_branches);
    int count = 3;

    unsigned int seat_availability;
    initialize_seat_availability(branches, num_branches, &seat_availability);
    
    unsigned int cutoff_met;
    initialize_cutoff_met(branches, num_branches, &cutoff_met);    
    
    char choice;
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
        int allocated_branch_index = -1;
        for (int i = 0; i < MAX_PREFS; i++) {
            allocated_branch_index = find_branch_index(branches, num_branches, student.preference[i]);
            if (allocated_branch_index != -1 && branches[allocated_branch_index].seats > 0 &&
                student.marks >= branches[allocated_branch_index].cutoff) {
                // Allocate branch if it's found, seats are available, and cutoff is met
                strcpy(student.branchAllocated, branches[allocated_branch_index].name);
                branches[allocated_branch_index].seats--; // Reduce available seats
                seat_availability &= ~(1u << allocated_branch_index); // Mark the seat as allocated in the seat availability bit vector
                cutoff_met |= (1u << allocated_branch_index); // Mark cutoff as met in the cutoff bit vector
                break;
            }
        }

        // Print allocated branch
        if(allocated_branch_index !=-1)
        {
            printf("Allocated branch for student with rool number %d is %s \n", student.roll, student.branchAllocated);
            count--;
        }
        else
        {
            printf("No Branch Allocated for student with roll number %d \n", student.roll);
        }

        printf("Remaining seats: ");
        for (int i = 0; i < num_branches; i++) {
            if (seat_availability & (1u << i)) {
                printf("%s: %d ", branches[i].name, branches[i].seats);
            }
        }
        printf("\n");
    } while (count > 0);

    return 0;
}
