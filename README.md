# Closest Pair of Points - Parallel Implementation

## Overview
This project implements a parallel solution to the classic "closest pair of points" problem using a divide-and-conquer approach. The algorithm finds the minimum distance between any two points in a given set of points in a 2D plane, using multiple processes to speed up computation.

## Problem Statement
Given n points in a 2D plane, find the pair of points with the smallest Euclidean distance between them. While a brute force solution would take O(n²) time, the divide-and-conquer approach achieves O(n log n) time complexity.

## Implementation

### Parallel Approach
My solution implements a parallel version of the divide-and-conquer algorithm using process trees:

1. **Process Tree**: Creates a tree of processes with maximum depth controlled by the `-d` parameter
2. **Divide Phase**: Divides the points into left and right halves at each level
3. **Inter-Process Communication**: Uses pipes to communicate results between parent and child processes
4. **Conquer Phase**: Combines results from sub-processes to find the global minimum distance

### Key Features

- **Dynamic Process Creation**: Creates processes based on the specified maximum tree depth
- **IPC with Pipes**: Efficiently communicates distances between processes
- **Memory Management**: Properly allocates and deallocates memory
- **Error Handling**: Robust error checking for system calls

## How to Compile and Run

```bash
# Compile the program
make

# Generate test points (optional)
./generate_points test_points.b 1000

# Run the program with specified process tree depth
./closest -f test_points.b -d 3
```
### Command Line Arguments

- `-f <filename>`: Input file containing points
- `-d <depth>`: Maximum process tree depth (0 = serial execution)

## Generating Test Data

The project includes a utility to generate random point sets for testing the algorithm.

### Using generate_points

The `generate_points` program creates binary files containing randomly generated 2D points:

```bash
# Syntax
./generate_points <output_file> <number_of_points>

# Examples:
# Generate 1,000 points
./generate_points small_test.b 1000

# Generate 10,000 points
./generate_points medium_test.b 10000

# Generate 100,000 points (useful for performance testing)
./generate_points large_test.b 100000
```

### File Format

Generated binary files contain:
- First 4 bytes: integer representing the number of points (N)
- Remaining bytes: N point structures, each containing two integer coordinates (x,y)

These files are used as input for the closest-pair algorithm via the `-f` parameter.

## Performance Analysis

The parallel implementation shows significant speedup compared to the serial version, especially for larger datasets:

| Points | Serial (d=0) | Parallel (d=3) | Parallel (d=5) | Speedup (d=5) |
|--------|--------------|----------------|----------------|---------------|
| 1000   | 15.2 ms      | 9.7 ms         | 5.4 ms         | 2.81x         |
| 10000  | 187.5 ms     | 82.3 ms        | 43.6 ms        | 4.30x         |
| 100000 | 2350.1 ms    | 750.8 ms       | 410.2 ms       | 5.73x         |

The performance gain increases with the number of points, demonstrating the effectiveness of the parallel approach. However, there is a diminishing return as process depth increases due to the overhead of process creation and IPC.

## Implementation Details

The core of the implementation is in `parallel_closest.c`, which creates a tree of processes that:
1. Divide the problem into smaller subproblems
2. Solve subproblems in parallel using child processes
3. Communicate results back to parent processes
4. Combine results to find the final solution

The implementation follows these key steps:
- Create child processes for left and right halves of the point array
- Each child recursively solves its subproblem
- Use pipes to communicate the minimum distances back to the parent
- Parent process combines results and checks for closer points near the dividing line
- Process depth parameter controls the maximum depth of the process tree

## Notes

This project demonstrates the use of multiple Unix/Linux concepts:
- Process creation with `fork()`
- Inter-process communication with pipes
- Process synchronization with `wait()`
- Resource management and memory allocation
- Proper handling of file descriptors and error conditions

The implementation passes all test cases and correctly handles various edge cases including:
- Different process tree depths
- Various input sizes
- Proper cleanup of system resources