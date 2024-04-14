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

    Student students[MAX_STD];
    int num_students;
    read_student_data_from_csv(students, &num_students);

    // Allocate branches to students
    // int num_s2;
    // allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb, &num_s2);
    allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb);

    // Write allocated branches along with student roll numbers and marks to a new CSV file
    if(!rollflag_inCSV) write_allocated_branches_to_csv(students, num_students);

    // void print_allocated_roll_numbers(students, num_students, branches, num_branches, cutoff_veb) {
    // FILE* allocated_file = fopen("allocated_branches.csv", "r");
    //     if (allocated_file == NULL) {
    //         allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb);
    //     }

    //     // Skip header
    //     char buffer[1024];
    //     fgets(buffer, sizeof(buffer), allocated_file);

    //     // Read and print roll numbers
    //     int roll;
    //     int marks;
    //     char alllocated[50];
    //     while (fscanf(allocated_file, "%d,%d,%[^\n]", &roll,&marks, alllocated) == 3) {
    //         printf("%d\n", roll);
    //         if(roll == students.roll){
                
    //         }
    //     }
    //     fclose(allocated_file);
    // }


    dealloc_vebtree(cutoff_veb);

    return 0;
}


