#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "point.h"

// Calculate the Euclidean distance between two points
double calculate_distance(struct Point p1, struct Point p2) {
    double dx = (double)p1.x - p2.x;
    double dy = (double)p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

// Compare function for qsort to sort points by x-coordinate
int compare_x(const void *a, const void *b) {
    struct Point *p1 = (struct Point *)a;
    struct Point *p2 = (struct Point *)b;
    return p1->x - p2->x;
}

// Compare function for qsort to sort points by y-coordinate
int compare_y(const void *a, const void *b) {
    struct Point *p1 = (struct Point *)a;
    struct Point *p2 = (struct Point *)b;
    return p1->y - p2->y;
}

// Return the minimum of two double values
double min_distance(double a, double b) {
    return (a < b) ? a : b;
}

// Find minimum distance between any two points in a strip
double strip_closest(struct Point *strip, int size, double min_dist) {
    // Sort by y-coordinate
    qsort(strip, size, sizeof(struct Point), compare_y);
    
    // Find pairs with distance less than current minimum
    double min = min_dist;
    for (int i = 0; i < size; i++) {
        // Check only points within min_dist in y direction
        for (int j = i + 1; j < size && (strip[j].y - strip[i].y) < min; j++) {
            double dist = calculate_distance(strip[i], strip[j]);
            if (dist < min) {
                min = dist;
            }
        }
    }
    return min;
}

// Simple function to find minimum distance with brute force
double brute_force(struct Point *points, int n) {
    double min = INFINITY;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dist = calculate_distance(points[i], points[j]);
            if (dist < min) {
                min = dist;
            }
        }
    }
    return min;
}

// Helper function to read points from a binary file
int read_points_from_file(const char *filename, struct Point **points) {
    FILE *file = fopen(filename, "rb");
    int count;

    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }
    
    if (fread(&count, sizeof(int), 1, file) != 1) {
        perror("Failed to read point count");
        fclose(file);
        return -1;
    }
    
    *points = malloc(count * sizeof(struct Point));
    if (*points == NULL) {
        perror("Failed to allocate memory for points");
        fclose(file);
        return -1;
    }
    
    if (fread(*points, sizeof(struct Point), count, file) != count) {
        perror("Failed to read points");
        free(*points);
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return count;
}

// Return the total number of points stored in the specified file.
int total_points(char *f_name) {
    struct stat st_buf;

    if (stat(f_name, &st_buf) != 0) {
        perror("stat");
        exit(1);
    }

    // Make sure that the file has a valid size.
    if ((st_buf.st_size - sizeof(int)) % sizeof(struct Point) != 0) {
        fprintf(stderr, "The size of the specified file is invalid!\n");
        exit(1);
    }

    return (st_buf.st_size - sizeof(int)) / sizeof(struct Point);
}

// Return all input points from the specified file and populate the points_arr array
void read_points(char *f_name, struct Point *points_arr) {
    int total, bytes_read;
    FILE *fp;

    fp = fopen(f_name, "rb");
    if (fp == NULL) {
        perror(f_name);
        exit(1);
    }

    bytes_read = fread(&total, sizeof(total), 1, fp);
    if (bytes_read == 0 || total < 0) {
        fprintf(stderr, "Failed to read number of points from %s.\n", f_name);
        exit(1);
    }

    bytes_read = fread(points_arr, sizeof(struct Point), total, fp);
    if (bytes_read != total) {
        fprintf(stderr, "Error reading %s.\n", f_name);
        exit(1);
    }

    if (fclose(fp)) {
        fprintf(stderr, "Error closing %s.\n", f_name);
        exit(1);
    }
}