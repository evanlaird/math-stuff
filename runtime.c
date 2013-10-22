#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

// contains the Matrix struct
#include "ising_headers.h"

// Default matrix size (always square)
#define  DEFAULT_LATTICE_SIZE 20

// iterations before first measurement
#define ITER_FIRST_MEASURE  5000
// iterations between measurements
#define ITER_MEASURE        1000
// Total stops
#define ITER_TOTAL          10000


Matrix *Matrix_create(int length) {
    int i;
    int j;
    Matrix *mat = malloc(sizeof(Matrix));

    // make sure malloc didn't fuck up
    assert(mat != NULL);

    mat->sideLength = length ? length : DEFAULT_LATTICE_SIZE;
    mat->totalSize = mat->sideLength * mat->sideLength;

    // Square lattice points
    mat->points = malloc(mat->sideLength * sizeof(int*));
    for (i = 0; i < mat->sideLength; i++) {
        mat->points[i] = malloc(mat->sideLength * sizeof(int));
    }
    for (i = 0; i < mat->sideLength; i++) {
        for (j = 0; j < mat->sideLength; j++) {
            mat->points[i][j] = 1;
        }
    }

    return mat;
}

void Matrix_destroy(Matrix *matrix) {
    assert(matrix != NULL);

    free(matrix->points);
    free(matrix);
}

void die(const char *message) {
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

void print_usage() {
    printf("USAGE: ./runtime -[L|N|J|t|t]");
}

int main(int argc, char *argv[]) {

    /*
     * Command line arguments:
     *  -L [int]        : lattice size
     *  -J [decimal]    : coupling constant
     *  -N [int]        : number of monte-carlo steps
     *  --start [decimal]: start temp
     *  --stop [decimal] : stop temp
     */

    // Default values
    int     option       = 0;
    int     lattice_size = DEFAULT_LATTICE_SIZE;
    int     steps        = ITER_TOTAL;
    float   coupling     = 1.0;
    float   start        = 1.0;
    float   stop         = 2.0;

    while ((option = getopt(argc, argv, "L:J:N:t:T:")) != -1) {
        switch (option) {
            case 'L': 
                lattice_size = atoi(optarg);
                break;
            case 'J':
                coupling = atof(optarg);
                break;
            case 'N':
                steps = atoi(optarg);
                break;
            case 't':
                start = atof(optarg);
                break;
            case 'T':
                stop = atof(optarg);
                break;
            default: 
                print_usage();
                die(NULL);
                break;
        }
    }

    // validate the inputs
    if (start - stop < 0) {
        print_usage();
        die("Invalid temperatures");
    }
    return 0;
}
