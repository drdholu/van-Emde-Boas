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

    Student student;
    char choice;
    do {
        printf("Enter student roll number: ");
        scanf("%d", &student.roll);
        printf("Enter student marks: ");
        scanf("%d", &student.marks);
        printf("Enter student branch preferences (up to %d branches):\n", MAX_PREFS);
        for (int i = 0; i < MAX_PREFS; i++) {
            printf("Preference %d: ", i + 1);
            scanf("%s", student.preference[i]);
        }

        int branch_index = predecessor_index(cutoff_veb, student.marks);
        // int cutoff_index = low(cutoff_veb->cluster[high(cutoff_veb, branch_index)], branch_index);
        // printf("Cutoff index %d\n");
        // int branch_index = predecessor_index(cutoff_veb, student.marks);
        int cutoff_index = -1;
        for (int i = 0; i < num_branches; ++i) {
            if (branches[i].cutoff == branch_index) {
                cutoff_index = i;
                break;
            }
        }

        if (cutoff_index != -1) {
            printf("Branch allocated for student with roll number %d: %s\n", student.roll, branches[cutoff_index].name);
        } else {
            printf("No branch allocated for student with roll number %d.\n", student.roll);
        }

        printf("Do you want to continue? (Y/N): ");
        scanf(" %c", &choice);
    } while (choice == 'Y' || choice == 'y');

    dealloc_vebtree(cutoff_veb);

    return 0;
}



