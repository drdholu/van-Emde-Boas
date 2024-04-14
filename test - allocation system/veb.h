#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EMPTY -1
#define MAX_PREFS 3
#define MAX_STD 50
#define CSV_FILENAME "college.csv"
#define CSV_STUDENT_FILENAME "students.csv"

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
    int alloted;
} Student;

typedef struct vebtree {
    int universeSize;
    int min;
    int max;
    struct vebtree* summary;
    struct vebtree** cluster;
} vebtree;

void swap(int* a, int* b);
vebtree* create(int universeSize);
int high(struct vebtree* V, int x);
int low(struct vebtree* V, int x);
int veb_index(struct vebtree* V, int i, int j);
void insert(struct vebtree* V, int x);
void deleteVEB(struct vebtree* V, int x);
int minimum(struct vebtree* V);
int maximum(struct vebtree* V);
int predecessor(struct vebtree* V, int x);
void dealloc_vebtree(struct vebtree* V);
void read_branch_data_from_csv(Branch branches[], int* num_branches);
void read_student_data_from_csv(Student students[], int* num_students);
void allocate_branches_to_students(Student students[], int num_students, Branch branches[], int num_branches, vebtree* cutoff_veb);
// void allocate_branches_to_students(Student students[], int num_students, Branch branches[], int num_branches, vebtree* cutoff_veb, int* num_s2);
void write_allocated_branches_to_csv(Student students[], int num_students);