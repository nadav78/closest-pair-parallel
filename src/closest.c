#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "point.h"
#include "point_utilities.h"
#include "serial_closest.h"
#include "parallel_closest.h"


void print_usage() {
    fprintf(stderr, "Usage: closest -f filename -d pdepth\n\n");
    fprintf(stderr, "    -d Maximum process tree depth\n");
    fprintf(stderr, "    -f File that contains the input points\n");

    exit(1);
}

int main(int argc, char **argv) {
    int n = -1;
    long pdepth = -1;
    char *filename = NULL;
    int pcount = 0;
    int opt;

    while ((opt = getopt(argc, argv, "f:d:")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'd':
            pdepth = atol(optarg);
            break;
        default: /* '?' */
            print_usage();
            }
        }

    if (filename == NULL || pdepth == -1) {
        print_usage();
    }

    // Read the points
    n = total_points(filename);
    struct Point points_arr[n];
    read_points(filename, points_arr);

    // Sort the points
    qsort(points_arr, n, sizeof(struct Point), compare_x);

    // Calculate the result using the parallel algorithm.
    double result_p = closest_parallel(points_arr, n, pdepth, &pcount);
    printf("The smallest distance: is %.2f (total worker processes: %d)\n", result_p, pcount);

    exit(0);
}
