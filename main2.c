#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EMPTY -1
#define MAX_COLLEGES 10

// Student structure
typedef struct Student {
    char name[50];
    int roll;
    int marks;
} Student;

// College structure
typedef struct College {
    char name[50];
    struct Branch* branch_ptr;
} College;

// Branch structure
typedef struct Branch {
    int seats;
    int cutoff;
} Branch;

// vEB tree data structure
typedef struct vEBTree {
    int universe_size;
    int min;
    int max;
    struct vEBTree* summary;
    struct vEBTree** clusters;
} vEBTree;

// Function prototypes
vEBTree* create_vEBTree(int universe_size);
int high(vEBTree* tree, int x);
int low(vEBTree* tree, int x);
int get_index(vEBTree* tree, int x, int y);
void vEB_insert(vEBTree* tree, int x);
int vEB_predecessor(vEBTree* tree, int x);

vEBTree* create_vEBTree(int universe_size) {
    vEBTree* tree = (vEBTree*)malloc(sizeof(vEBTree));
    if (tree == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    tree->universe_size = universe_size;
    tree->min = EMPTY;
    tree->max = EMPTY;

    if (universe_size <= 2) {
        tree->summary = NULL;
        tree->clusters = NULL;
    } else {
        int sqrt_size = 1 << (int)(ceil(log2(sqrt(universe_size))));
        tree->summary = create_vEBTree(sqrt_size);
        if (tree->summary == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        tree->clusters = (vEBTree**)malloc(sizeof(vEBTree*) * sqrt_size);
        if (tree->clusters == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < sqrt_size; i++) {
            tree->clusters[i] = create_vEBTree(sqrt_size);
            if (tree->clusters[i] == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
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
    // high(sqrt(u)) + low gives index of the number we want
    return x * (int)sqrt(tree->universe_size) + y;
}

void vEB_insert(vEBTree* tree, int x) {
    if (tree->min == EMPTY) {
        // If the tree is empty, set the min and max to x
        tree->min = tree->max = x;
    } else {
        if (x < tree->min) {
            // If x is less than the current min, swap x and the current min
            int temp = tree->min;
            tree->min = x;
            x = temp;
        }
        if (x > tree->max) {
            // If x is greater than the current max, update the max to x
            tree->max = x;
        }

        // If the universe size is greater than 2, recursively insert x into appropriate cluster
        if (tree->universe_size > 2) {
            // Calculate the cluster index and offset
            int cluster_index = high(tree, x);
            int offset = low(tree, x);

            // If the cluster is empty, insert into summary and update min/max of the cluster
            if (tree->clusters[cluster_index]->min == EMPTY) {
                vEB_insert(tree->summary, cluster_index);
                tree->clusters[cluster_index]->min = tree->clusters[cluster_index]->max = offset;
            } else {
                // Otherwise, recursively insert into the cluster
                vEB_insert(tree->clusters[cluster_index], offset);
            }
        }
    }
}

int vEB_predecessor(vEBTree* tree, int x) {
    if (tree->min != EMPTY && x < tree->min) {
        // printf("Returning EMPTY 1\n");
        return EMPTY;
    } else {
        if (tree->max != EMPTY && x > tree->max) {
            // printf("Returning max\n");
            return tree->max;
        } else {
            int high_x = high(tree, x);
            int low_x = low(tree, x);
            
            // Debugging print statements
            // printf("high_x: %d\n", high_x);
            // printf("universe_size: %d\n", tree->universe_size);
            if (high_x >= tree->universe_size) {
                // printf("high_x out of bounds\n");
                return EMPTY;
            }
            if (tree->clusters == NULL) {
                // printf("clusters is NULL\n");
                return EMPTY;
            }
            if (tree->clusters[high_x] == NULL) {
                // printf("clusters[%d] is NULL\n", high_x);
                return EMPTY;
            }
            
            if (tree->clusters[high_x] != NULL) {
                int max_low = tree->clusters[high_x]->max; // max of current cluster
                if (max_low != EMPTY && low_x > max_low) {
                    // look inside same cluster if max of cluster is < than x
                    int offset = vEB_predecessor(tree->clusters[high_x], low_x);
                    if (offset != EMPTY) {
                        // printf("Returning offset\n");
                        return get_index(tree, high_x, offset);
                    }
                }
            }
            // look in summary vector
            if (tree->summary != NULL) {
                int pred_cluster = vEB_predecessor(tree->summary, high_x);
                if (pred_cluster != EMPTY) {
                    int offset = tree->clusters[pred_cluster]->max;
                    if (offset != EMPTY) {
                        // printf("Returning offset from summary\n");
                        return get_index(tree, pred_cluster, offset);
                    }
                }
            }
            // printf("Returning min\n");
            return tree->min;
        }
    }
}

// Function to insert college cutoffs into the bit vector
void insert_college_cutoff(int cutoffs[], int index, int cutoff) {
    cutoffs[index] = cutoff;
    // printf("Inserted cutoff %d for college %d\n", cutoff, index);
}

int get_college_index(int cutoffs[], int marks) {
    for (int i = 0; i < MAX_COLLEGES; i++) {
        if (marks < cutoffs[i]) {
            return i; // College assigned based on first matching cutoff
        }
    }
    return MAX_COLLEGES - 1; // If marks exceed all cutoffs, return the index of the last college
}

int main() {
    // Initialize vEB tree for finding successor of student's marks
    vEBTree* marks_tree = create_vEBTree(100); // Adjust universe size as needed
    vEBTree* cutoff_tree = create_vEBTree(100); // Adjust universe size as needed

    // Initialize bit vector for storing college cutoffs
    int college_cutoffs[MAX_COLLEGES] = {0}; // Initialize all cutoffs to 0

    // Insert college cutoffs into the bit vector
    // TREE - COLLEGE - CUTOFF
    insert_college_cutoff(college_cutoffs, 2, 80);
    insert_college_cutoff(college_cutoffs, 1, 85); 
    insert_college_cutoff(college_cutoffs, 0, 90); 
    // Add more cutoffs as needed

    // NAME - ROLL - MARKS
    Student students[] = {
        {"Bob", 102, 78},
        {"Alice", 101, 88},
        {"Charlie", 103, 92},
        // Add more students as needed
    };
    int num_students = sizeof(students) / sizeof(students[0]);

    // Assign colleges to students based on their marks
    for (int i = 0; i < num_students; i++) {

        vEB_insert(marks_tree, students[i].marks); // Insert student's marks
        vEB_insert(cutoff_tree, college_cutoffs[i]); // Insert student's marks

        // Find predecessor college index based on marks
        int predecessor_college_index = vEB_predecessor(cutoff_tree, students[i].marks);

        // Check if student has a predecessor (no need to check EMPTY)
        if (predecessor_college_index != -1) {
            // Use predecessor_college_index to access the college number
            int cutoff_number = predecessor_college_index;

            // Use cutoff_number to get the cutoff (optional)
            int college_cutoff = college_cutoffs[cutoff_number]; // You can use this for further processing

            // Check if student meets cutoff for the predecessor college
            if (students[i].marks <= college_cutoff) {
                printf("%s (Roll: %d) assigned to College %d\n", students[i].name, students[i].roll, cutoff_number);
                // ... update seats or other operations
            }
        } else {
            // Student has the minimum mark (no predecessor)
            printf("%s (Roll: %d) has the minimum marks and gets no college.\n", students[i].name, students[i].roll);
        }
    }

    return 0;
}
