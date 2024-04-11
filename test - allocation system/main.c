#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "veb.c"

int main() {
    Branch branches[10];
    int num_branches;
    read_branch_data_from_csv(branches, &num_branches);

    vebtree* cutoff_veb = create(200);
    for (int i = 0; i < num_branches; ++i) {
        insert(cutoff_veb, branches[i].cutoff);
    }

    Student students[10];
    int num_students;
    read_student_data_from_csv(students, &num_students);

    // Allocate branches to students
    allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb);

    // Write allocated branches along with student roll numbers and marks to a new CSV file
    write_allocated_branches_to_csv(students, num_students);

    dealloc_vebtree(cutoff_veb);

    return 0;
}


