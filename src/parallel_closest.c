#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <float.h>

#include "point.h"
#include "serial_closest.h"
#include "parallel_closest.h"
#include "point_utilities.h"

/*
 * Multi-process (parallel) implementation of the recursive divide-and-conquer
 * algorithm to find the minimal distance between any two pair of points in p[].
 * Assumes that the array p[] is sorted according to x coordinate.
 */
double closest_parallel(struct Point *p, int n, int pdmax, int *pcount) {
    // If n < 4 or pdmax == 0, use the serial implementation
    if (n < 4 || pdmax == 0) {
        *pcount = 0;
        return closest_serial(p, n);
    }

    int mid = n / 2;
    struct Point *left = p;
    struct Point *right = p + mid;
    int left_n = mid;
    int right_n = n - mid;

    int pipe_left[2];
    if (pipe(pipe_left) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid_left = fork();
    if (pid_left == -1) {
        perror("fork");
        exit(1);
    }

    if (pid_left == 0) {
        // Left child process
        close(pipe_left[0]); // Close read end
        int left_pcount = 0;
        double left_dist = closest_parallel(left, left_n, pdmax - 1, &left_pcount);
        if (write(pipe_left[1], &left_dist, sizeof(double)) == -1) {
            perror("write");
            exit(1);
        }
        close(pipe_left[1]); // Close write end
        exit(left_pcount + 1); // Include this process in the count
    }

    int pipe_right[2];
    if (pipe(pipe_right) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid_right = fork();
    if (pid_right == -1) {
        perror("fork");
        exit(1);
    }

    if (pid_right == 0) {
        // Right child process
        close(pipe_right[0]); // Close read end
        int right_pcount = 0;
        double right_dist = closest_parallel(right, right_n, pdmax - 1, &right_pcount);
        if (write(pipe_right[1], &right_dist, sizeof(double)) == -1) {
            perror("write");
            exit(1);
        }
        close(pipe_right[1]); // Close write end
        exit(right_pcount + 1); // Include this process in the count
    }

    // Parent process
    close(pipe_left[1]); // Close write end
    close(pipe_right[1]); // Close write end

    // Wait for both child processes to complete
    int status_left, status_right;
    if (waitpid(pid_left, &status_left, 0) == -1) {
        perror("waitpid");
        exit(1);
    }
    if (waitpid(pid_right, &status_right, 0) == -1) {
        perror("waitpid");
        exit(1);
    }

    // Check if both child processes exited normally
    if (!WIFEXITED(status_left) || !WIFEXITED(status_right)) {
        fprintf(stderr, "Child process did not exit normally\n");
        exit(1);
    }

    double left_dist, right_dist;
    if (read(pipe_left[0], &left_dist, sizeof(double)) == -1) {
        perror("read");
        exit(1);
    }
    if (read(pipe_right[0], &right_dist, sizeof(double)) == -1) {
        perror("read");
        exit(1);
    }
    close(pipe_left[0]); // Close read end
    close(pipe_right[0]); // Close read end

    // Calculate the total number of worker processes
    // Add the counts from both child processes
    *pcount = WEXITSTATUS(status_left) + WEXITSTATUS(status_right);

    // Find the minimum distance between the left and right halves
    double d = min_distance(left_dist, right_dist);

    // Create an array to hold points that are within distance d of the middle line
    struct Point *strip = (struct Point *)malloc(n * sizeof(struct Point));
    if (strip == NULL) {
        perror("malloc");
        exit(1);
    }
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (abs(p[i].x - p[mid].x) < d) {
            strip[j] = p[i];
            j++;
        }
    }

    // Find the minimum distance in the strip
    double strip_dist = strip_closest(strip, j, d);

    // Free the dynamically allocated memory
    free(strip);

    // Return the minimum distance found
    return min_distance(d, strip_dist);
}