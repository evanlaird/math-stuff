#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "datatypes.h"

/*
 * Never done this before. Going to put convenience methods defined here and
 * see if I can call them by just importing the .h file...
 */

/*
 * **************************************************************
 */
// Matrix creation and destruction 
Matrix *Matrix_create(int length) {
    Matrix *mat = malloc(sizeof(Matrix));

    // make sure malloc didn't fuck up
    assert(mat != NULL);
    if (mat == NULL) {
        die("Failed to allocate space for matrix");
    }
    
    int i, j;

    mat->sideLength = length ? length : DEFAULT_LATTICE_SIZE;
    mat->totalSize = mat->sideLength * mat->sideLength;

    // Square lattice points allocation
    mat->points = malloc(mat->sideLength * sizeof(int*));
    for (i = 0; i < mat->sideLength; i++) {
        mat->points[i] = malloc(mat->sideLength * sizeof(int));
    }

    // Matrix initialization
    // FIXME: should probably randomize (1|-1)
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
// Temp structure creation/destruction
TempInfo *TempInfo_create(float start, float stop, float step) {
    TempInfo *temps = malloc(sizeof(TempInfo));

    assert(temps != NULL);
    if (temps == NULL) {
        die ("Failed to allocate space for TempInfo");
    }

    int i = 0;

    // Just take absolute values here, because negative temps don't make any
    // physical sense, and a negative step can be achieved via
    // backwards-traversing the data post-simulation
    temps->start = fabs(start);
    temps->stop = fabs(stop);
    temps->step = fabs(step);

    // Create the tempRange array, so we can just pull values from there
    temps->totalSteps = ceil((temps->stop - temps->stop) / temps->step);
    temps->tempRange = malloc(temps->totalSteps * sizeof(float));
    // the steps-1 thing is because I always want the final temperature to be
    // the given `stop` value
    for (i = 0; i < temps->totalSteps - 1; i++) {
        temps->tempRange[i] = temps->start + (i * step);
    }
    temps->tempRange[temps->totalSteps] = temps->stop;
    
    return temps;
}

void TempInfo_destroy(TempInfo *temps) {
    assert (temps != NULL);

    if (temps->tempRange) {
        free(temps->tempRange);
    }
    free(temps);
}
