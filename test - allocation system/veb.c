#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "veb.h"


// check to prevent re-seat allocation to same students
int rollflag_inCSV = 0;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

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
        deleteVEB(V->cluster[high(V, x)], low(V, x));
        if (minimum(V->cluster[high(V, x)]) == EMPTY) {
            deleteVEB(V->summary, high(V, x));
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

    // printf("Read %d branches:\n", *num_branches);
    // for (int i = 0; i < *num_branches; i++) {
    //     printf("Branch %d: Name: %s, Cutoff: %d, Seats: %d\n", i+1, branches[i].name, branches[i].cutoff, branches[i].seats);
    // }
}

void read_student_data_from_csv(Student students[], int* num_students) {
    FILE* file = fopen("students.csv", "r");
    if (file == NULL) {
        printf("Error opening file students.csv.\n");
        exit(1);
    }

    // Skip header
    char buffer[1024];
    fgets(buffer, sizeof(buffer), file);

    // Read student data
    *num_students = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && *num_students < MAX_STD) {
        int roll, marks;
        char preferences[MAX_PREFS * 50]; // Assuming maximum length of preferences string
        if (sscanf(buffer, "%d,%d,%[^\n]", &roll, &marks, preferences) == 3) {
            students[*num_students].roll = roll;
            students[*num_students].marks = marks;
            char* token = strtok(preferences, " ");
            int i = 0;
            while (token != NULL && i < MAX_PREFS) {
                strcpy(students[*num_students].preference[i], token);
                token = strtok(NULL, " ");
                i++;
            }
            // printf("roll: %d, marks: %d, preference: ", students[*num_students].roll, students[*num_students].marks);
            // for(int i =0; i<MAX_PREFS; i++){
            //     printf("%s ", students[*num_students].preference[i]);
            // }
            // printf("\n");
            (*num_students)++;
        } else {
            printf("Error reading data from students.csv file.\n");
        }
    }
    fclose(file);

    printf(RED"Read %d students.\n"RESET, *num_students);
}

void allocate_branches_to_students(Student students[], int num_students, Branch branches[], int num_branches, vebtree* cutoff_veb) {
    
    FILE* allocated_file = fopen("allocated_branches.csv", "r");
    for (int i = 0; i < num_students; i++) 
    {
        
        // if (allocated_file == NULL) {
        //     allocate_branches_to_students(students, num_students, branches, num_branches, cutoff_veb);
        // }

        // Skip header
        char buffer[1024];
        fgets(buffer, sizeof(buffer), allocated_file);

        // Read and print roll numbers
        int roll;
        int marks;
        char alllocated[50];
        while (fscanf(allocated_file, "%d,%d,%[^\n]", &roll,&marks, alllocated) == 3) {
            if(roll == students[i].roll){
            printf("%d\n", roll);
                rollflag_inCSV = 1;
                break;
            }
        }
        if(rollflag_inCSV == 1) continue;
        // if(students[i].alloted) continue;

        int branch_index = predecessor_index(cutoff_veb, students[i].marks);
        int preference_index = -1;
        int cutoff_index = -1;
        for(int j=0;j<num_branches;j++)
        {
            if(branches[j].cutoff == branch_index)
            {
                cutoff_index = j;
                break;
            }
        }
        // printf("%d %s", cutoff_index, branches[cutoff_index].name);
        while (branch_index != -1)
        {
            for (int j = 0; j < num_branches; ++j) 
            {
                if (branches[j].cutoff == branch_index && branches[j].seats > 0) 
                {
                    // Allocate branch to student
                    strcpy(students[i].branchAllocated, branches[j].name);

                    // to find index of students[i].preference[k]
                    int pref_index = 0;
                    
                    
                    // Check if the allocated branch matches any of the student's preferences
                    int preference_matched = 0;
                    int branchFlag = 0;
                    for (int k = 0; k < MAX_PREFS; k++) {
                        if (strcmp(students[i].branchAllocated, students[i].preference[k]) == 0) {
                            // students[i].alloted = 1;
                            preference_matched = 1;
                            break;
                        }
                        for(int l = 0; l<num_branches; l++){
                            if(strcmp(branches[l].name, students[i].preference[k]) == 0){
                                // printf("pref_index -> %d\n", pref_index);
                                pref_index = l;
                                break;
                            }
                        }
                        if (branches[pref_index].cutoff < branches[j].cutoff) {
                            // printf("real seat -> %s\n", branches[pref_index].name);
                            strcpy(students[i].branchAllocated, branches[pref_index].name);
                            // students[i].alloted = 1;
                            branchFlag = 1;
                            break;
                        }
                    }


                    // If no preference matched, remove the allocated branch
                    if (!preference_matched && !branchFlag) {
                        strcpy(students[i].branchAllocated, "No Branch Allocated");
                    } else {
                        if(preference_matched){
                            // Update available seats for the allocated branch
                            if(branches[j].seats>0){
                                branches[j].seats--;
                            }
                            else if(branches[j].seats == 0){
                                strcpy(students[i].branchAllocated, "No Branch Allocated");
                                break;
                            }
                        
                        }
                        if(branchFlag){

                            if(branches[pref_index].seats>0) branches[pref_index].seats--;
                            else if(branches[j].seats == 0){
                                strcpy(students[i].branchAllocated, "No Branch Allocated");
                                break;
                            }
                        }
                        
                            // Update the CSV file with the updated seats information
                            FILE* file = fopen(CSV_FILENAME, "w");
                            if (file == NULL) {
                                printf("Error opening file %s for updating seats.\n", CSV_FILENAME);
                                exit(1);
                            }

                            // Write header
                            fprintf(file, "Branch,Cutoff,Seats\n");

                            // Write updated branch information
                            for (int k = 0; k < num_branches; k++) {
                                fprintf(file, "%s,%d,%d\n", branches[k].name, branches[k].cutoff, branches[k].seats);
                            }

                            fclose(file);

                    }
                    
                    // Exit loop after allocation or removal
                    preference_index = j;
                    break;
                }
            }
            if (preference_index != -1)
                break; // Allocation or removal successful, exit loop
            // Try next preference
            // branch_index = predecessor_index(cutoff_veb, branch_index-1);
        }
        // if (preference_index == -1) {
        //     printf("No branch allocated for student with roll number %d.\n", students[i].roll);
        // }
    }
    fclose(allocated_file);
}

void write_allocated_branches_to_csv(Student students[], int num_students) {
    FILE* file = fopen("allocated_branches.csv", "w");
    if (file == NULL) {
        printf("Error opening file allocated_branches.csv for writing.\n");
        exit(1);
    }

    // Write header
    fprintf(file, "Roll,Marks,Allocated Branch\n");

    // Write allocated branches for each student
    for (int i = 0; i < num_students; i++) {
        fprintf(file, "%d,%d,%s\n", students[i].roll, students[i].marks, students[i].branchAllocated);
    }

    fclose(file);
}