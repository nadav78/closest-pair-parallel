#ifndef _POINT_UTILITIES_H
#define _POINT_UTILITIES_H

#include "point.h"

// Calculate the Euclidean distance between two points
double calculate_distance(struct Point p1, struct Point p2);

// Compare function for qsort to sort points by x-coordinate
int compare_x(const void *a, const void *b);

// Compare function for qsort to sort points by y-coordinate
int compare_y(const void *a, const void *b);

// Return the minimum of two double values
double min_distance(double a, double b);

// Find minimum distance between any two points in a strip
double strip_closest(struct Point *strip, int size, double min_dist);

// Simple function to find minimum distance with brute force
double brute_force(struct Point *points, int n);

// Helper function to read points from a binary file
int read_points_from_file(const char *filename, struct Point **points);

// Return the total number of points stored in the specified file.
int total_points(char *f_name);

// Return all input points from the specified file and populate the points_arr array
void read_points(char *f_name, struct Point *points_arr);

#endif /* _POINT_UTILITIES_H */