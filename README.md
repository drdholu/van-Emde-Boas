# vEB Trees Allocation Framework

## Overview
This is a simple branch allocation system designed to allocate branches to students based on their marks and preferences. It utilizes a special data structure called the van Emde Boas tree (vEB tree) to efficiently store and retrieve branch cutoff marks.

## Data Structures Used
1. **Branch**: Represents a branch with attributes such as cutoff marks, available seats, and name.
2. **Student**: Represents a student with attributes including roll number, marks, branch preferences, and allocated branch.
3. **vEB Tree (van Emde Boas Tree)**: A specialized data structure used to maintain a dynamic set of integers from a universe of size `universeSize`. It supports operations like insert, delete, minimum, maximum, predecessor, and successor efficiently in O(log log U) time, where U is the size of the universe.

## Functions Used
1. **create**: Creates a new vEB tree with the specified universe size.
2. **insert**: Inserts an element into the vEB tree.
3. **delete**: Deletes an element from the vEB tree.
4. **minimum**: Finds the minimum element in the vEB tree.
5. **maximum**: Finds the maximum element in the vEB tree.
6. **predecessor**: Finds the predecessor of a given element in the vEB tree.
7. **read_branch_data_from_csv**: Reads branch data from a CSV file and populates the `Branch` array.
8. **main**: The main function of the program, responsible for reading input from the user, allocating branches to students, and deallocating memory.

## van Emde Boas Tree (vEB Tree)
The van Emde Boas tree (vEB tree) is a specialized data structure designed to maintain a dynamic set of integers. It achieves efficient time complexity for operations like insert, delete, minimum, maximum, predecessor, and successor by dividing the universe into smaller sub-universes recursively.

## Application
The application allows users to input student roll numbers, marks, and branch preferences. Based on this input and the available branches with their cutoff marks, the system allocates the most suitable branch to each student, considering their preferences and marks.

