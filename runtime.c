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
#define  DEFAULT_LATTICE_SIZE 32

// iterations before first measurement
#define ITER_FIRST_MEASURE  5000
// iterations between measurements
#define ITER_MEASURE        1000
// Total stops
#define ITER_TOTAL          10000

// function defines
void die(const char *message);

/*
 * **************************************************************
 */
// Matrix creation and destruction 
Matrix *Matrix_create(int length) {
    int i, j;
    Matrix *mat = malloc(sizeof(Matrix));

    // make sure malloc didn't fuck up
    assert(mat != NULL);
    if (mat == NULL) {
        die("Failed to allocate space for matrix");
    }

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

void Matrix_destroy(Matrix *lattice) {
    assert(lattice != NULL);

    free(lattice->points);
    free(lattice);
}

void update_matrix(Matrix *lattice, int sideLength, int cpl, int N, float t, float T) {
    // Assumes a square matrix
    int i, j;

    // Matrix iterations
    for (i = 0; i < sideLength; i++ ) for (j = 0; j < sideLength; j++) {
        continue;
    }
}

/*
 * **************************************************************
 */
// Lifetime management
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

    // getopt() is an interesting stdlib function for argparsing.
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
    if (stop - start < 0) {
        print_usage();
        die("Invalid temperatures");
    }
    return 0;
}

void ising_stepper(Matrix *lattice, StepInfo *stepInfo, TempInfo *tempInfo, float coupling) {
    /**
     * Here, we need to loop through every lattice point (n^2) a total of 
     * `steps` times (steps * n^2), for every temperature in range `stop` -
     * `start`. So we have (range * (steps * (n*n))), which is a
     * quadruply-nested loop. Therefore, an order of magnitude increase in
     * lattice size can lengthen our time by 10^4! We're on the O(n^4) regime
     * here and need to optimze every possible way
     * 
     */

    int side_length = lattice->sideLength;
    // Modulo look-up table (plus)
    int px[side_length], py[side_length];
    // (minus)
    int mx[side_length], my[side_length];
    /*
     * Initialize the modulo arrays. p_[i] = M[i] + 1, m_[i] = M[i] - 1 wrapped
     * on a torus (because, physics)
     */
    for (int i = 0; i < side_length; i++) {
        px[i] = py[i] = i + 1;
        mx[i] = my[i] = i - 1;
    }

    // Calculate temperature range


    px[side_length] = py[side_length] = 0;
    mx[0] = my[0] = side_length - 1;

    // Every temp in range, for N steps, for every point
    for (int n = 0; n < side_length; n++) {
        for (int j = 0; j < side_length; j++) {
        }
    }
}
