# vEB Trees Allocation Framework

## Introduction

The van Emde Boas Tree, commonly known as vEB tree, is a powerful data structure designed for maintaining a dynamic set of integers within a predefined universe. Unlike traditional data structures such as binary search trees (BSTs) that offer \(O(\log n)\) time complexity for operations, vEB trees provide superior performance with \(O(\log \log U)\) time complexity, where \(U\) represents the universe size.

## Understanding vEB Trees

### Data Structure

- **Universe Size**: \(U\), representing the range of integers from \(0\) to \(U - 1\).
- **Minimum and Maximum Values**: Pointers to the smallest and largest elements in the tree.
- **Summary and Cluster Structures**: Hierarchical components used for efficient traversal and lookup operations.

### Algorithm Strategy

vEB trees employ a divide and conquer strategy, recursively partitioning the universe into smaller segments and utilizing summary and cluster structures to achieve fast lookup times.

## Time Complexities

- **Insertion/Deletion**: \(O(\log \log U)\)
- **Predecessor/Successor Query**: \(O(\log \log U)\)
- **Minimum/Maximum Query**: \(O(1)\)

These complexities enable efficient operations over large ranges of integers.

## Function Implementation

vEB trees include functions for:

- Creating/Deleting trees
- Inserting/Deleting elements
- Finding predecessor/successor
- Finding minimum/maximum

## Comparison with Other Structures

Compared to BSTs, vEB trees offer significantly faster lookup and query times, making them ideal for large integer ranges.

## Applications

vEB trees find applications in:

- Sparse Arrays
- Priority Queues
- Network Routing
- Memory Management

## Application Implementation

In our application, we leverage vEB trees to efficiently allocate branches to students based on their marks and preferences. We update available seats for each branch and write allocated branches along with student details to a CSV file.

## Functions Used
1. **create**: Creates a new vEB tree with the specified universe size.
2. **insert**: Inserts an element into the vEB tree.
3. **delete**: Deletes an element from the vEB tree.
4. **minimum**: Finds the minimum element in the vEB tree.
5. **maximum**: Finds the maximum element in the vEB tree.
6. **predecessor**: Finds the predecessor of a given element in the vEB tree.
7. **read_branch_data_from_csv**: Reads branch data from a CSV file and populates the `Branch` array.
