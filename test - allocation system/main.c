#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "veb.c"

// // Function to output vEB tree in Graphviz DOT format
// void output_veb_graphviz(FILE *fp, vebtree *V, int clusterIndex, int parentNode) {
//     if (V == NULL || V->min == EMPTY) return;

//     // Output current node
//     fprintf(fp, " node%d [label=\"min: %d | max: %d\"];\n", clusterIndex, V->min, V->max);
//     if (parentNode != -1) {
//         fprintf(fp, " node%d -> node%d;\n", parentNode, clusterIndex);
//     }

//     // Output clusters
//     if (V->universeSize > 2) {
//         int clusters = (int)sqrt(V->universeSize);
//         fprintf(fp, " subgraph cluster%d {\n", clusterIndex);
//         fprintf(fp, "  label=\"Clusters\";\n");
//         fprintf(fp, "  style=dotted;\n");
//         for (int i = 0; i < clusters; ++i) {
//             int childNode = clusterIndex * clusters + i + 1;
//             output_veb_graphviz(fp, V->cluster[i], childNode, clusterIndex);
//         }
//         fprintf(fp, " }\n");
//     }
// }

// // Function to output students data in Graphviz DOT format
// void output_students_graphviz(FILE *fp, Student students[], int num_students) {
//     fprintf(fp, " node [shape=plaintext];\n");
//     fprintf(fp, " edge [arrowhead=none];\n");
//     for (int i = 0; i < num_students; ++i) {
//         fprintf(fp, " \"%d (Marks: %d)\" -> \"%s\"\n", students[i].roll, students[i].marks, students[i].branchAllocated);
//     }
// }

// // Function to visualize vEB tree and students data
// void visualize_data(vebtree *V, Student students[], int num_students) {
//     FILE *fp = fopen("visualization.dot", "w");
//     if (fp == NULL) {
//         printf("Error opening file for writing visualization.\n");
//         return;
//     }

//     fprintf(fp, "digraph G {\n");
//     fprintf(fp, " rankdir=TB;\n");

//     // Output vEB tree
//     output_veb_graphviz(fp, V, 0, -1);

//     // Output students data
//     output_students_graphviz(fp, students, num_students);

//     fprintf(fp, "}\n");
//     fclose(fp);

//     // Generate PNG image from DOT file using Graphviz
//     system("dot -Tpng visualization.dot -o visualization.png");
//     printf("Visualization created: visualization.png\n");
// }

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
    allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb);

    if(!rollflag_inCSV) write_allocated_branches_to_csv(students, num_students);

    // visualize_data(cutoff_veb, students, num_students);

    dealloc_vebtree(cutoff_veb);

    return 0;
}


