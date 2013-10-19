#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <assert.h>

#include "ising_headers.h"

// Default matrix size (always square)
#define  MATRIX_LENGTH 20

// iterations before first measurement
#define ITER_FIRST_MEASURE  10000
// iterations between measurements
#define ITER_MEASURE        1000


Matrix *Matrix_create(int length) {
    int i;
    int j;
    Matrix *mat = malloc(sizeof(Matrix));

    // make sure malloc didn't fuck up
    assert(mat != NULL);

    mat->sideLength = length ? length : MATRIX_LENGTH;
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

int main(int argc, char *argv[]) {

    Matrix *lattice = Matrix_create(20);

    Matrix_destroy(lattice);

    return 0;
}
