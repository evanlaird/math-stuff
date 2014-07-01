#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "datatypes.h"

int random_int();

/*
 * Never done this before. Going to put convenience methods defined here and
 * see if I can call them by just importing the .h file...
 */

/*
 * **************************************************************
 */
// Matrix creation and destruction
Matrix *Matrix_create(unsigned int length) {
    Matrix *mat = malloc(sizeof(Matrix));

    // make sure malloc didn't fuck up
    assert(mat != NULL);
    if (mat == NULL) {
        die("Failed to allocate space for matrix");
    }

    int i, j;

    mat->sideLength = length ?: DEFAULT_LATTICE_SIZE;
    mat->totalSize = mat->sideLength * mat->sideLength;

    // Square lattice points allocation
    mat->points = malloc(mat->sideLength * sizeof(char*));
    for (i = 0; i < mat->sideLength; i++) {
        mat->points[i] = malloc(mat->sideLength * sizeof(char));
    }

    // Matrix initialization
    // FIXME: should probably randomize (1|-1)
    for (i = 0; i < mat->sideLength; i++) {
        for (j = 0; j < mat->sideLength; j++) {
            mat->points[i][j] = random_int();
        }
    }

    return mat;
}

void Matrix_destroy(Matrix *lattice) {
    assert(lattice != NULL);

    if (lattice->points) {
        int i = 0;
        for (i = 0; i < lattice->sideLength; i++) {
            free(lattice->points[i]);
        }
        free(lattice->points);
    }
    free(lattice);
}

void Matrix_print(Matrix *lattice) {
    assert(lattice != NULL);
    assert(lattice->points != NULL);

    // Print out a user-friendly description of the matrix here, with related info
    int n, m = 0;
    printf("(Matrix *)lattice at <%p>\n", lattice);
    printf("{\n");
    char zero_one_padding[] = "  ",
         negative_one_pad[] = " ";
    for (n = 0; n < lattice->sideLength; n++) {
        for (m = 0; m < lattice->sideLength; m++) {
            int point = lattice->points[n][m];
            printf("%s%d", ((point == -1) ? negative_one_pad : zero_one_padding), point);
        }
        printf("\n");
    }
    printf("}\n");
}

double mGet(Matrix *lattice, int i, int j) {
    if (!lattice || !lattice->points) {
        return 0;
    }

    return lattice->points[i][j];
}

/*
 * **************************************************************
 */
// Temp structure creation/destruction
TempInfo *TempInfo_create(float start, float stop, float step) {
    TempInfo *temps = malloc(sizeof(TempInfo));

    assert(temps != NULL);
    if (temps == NULL) {
        die("Failed to allocate space for TempInfo");
    }

    int i = 0;

    // Just take absolute values here, because negative temps don't make any
    // physical sense, and a negative step can be achieved via
    // backwards-traversing the data post-simulation
    temps->start = fabs(start);
    temps->stop = fabs(stop);
    temps->step = fabs(step);

    // Create the tempRange array, so we can just pull values from there
    temps->totalSteps = ceil((temps->stop - temps->start) / temps->step);
    temps->tempRange = malloc(temps->totalSteps * sizeof(float));
    assert(temps->tempRange != NULL);
    if (temps->tempRange == NULL) {
        die("Failed to allocate space for TempInfo->tempRange");
    }
    // the steps-1 thing is because I always want the final temperature to be
    // the given `stop` value
    for (i = 0; i < temps->totalSteps - 1; i++) {
        temps->tempRange[i] = temps->start + (i * step);
    }
    temps->tempRange[temps->totalSteps-1] = temps->stop;

    return temps;
}

void TempInfo_destroy(TempInfo *temps) {
    assert (temps != NULL);

    if (temps->tempRange) {
        free(temps->tempRange);
    }
    free(temps);
}

void TempInfo_print(TempInfo *temp) {
    assert(temp != NULL);
    printf("(TempInfo *)temp at <%p>\n", temp);
    printf("start temp: %f\n", temp->start);
    printf("stop temp:  %f\n", temp->stop);
    printf("step size:  %f\n", temp->step);
    printf("total steps:%d\n", temp->totalSteps);
}

/*
 * **************************************************************
 */
// StepInfo creation/destruction
StepInfo *CreateStepInfo(int totalSteps, int relaxationSteps, int loopCount)
{
    StepInfo *stepInfo = malloc(sizeof(StepInfo));
    assert(stepInfo != NULL);
    if (stepInfo == NULL) {
        die("Failure initializing StepInfo");
    }

    stepInfo->totalSteps = totalSteps;
    stepInfo->relaxationSteps = relaxationSteps;
    stepInfo->loopCount = loopCount;

    return stepInfo;
}


/*
 * **************************************************************
 */
// Random number generation is, frankly, a pain in the ass. The rand() function
// simply returns a uniform, probably not very good randon number on the range
// [0, RAND_MAX], where RAND_MAX on my 64 bit mac is 2^31 - 1. Since I only care
// about random numbers like so [-1,0,1], I'm going to cheat and just do a mod 3 - 1
// trick here. This should be replaced by something more suitable for MC simulations
int random_int() {
    //TODO: make this better
    return (rand() % 3 - 1);
}

/// Returns a random double between [0,1]
double drand() {
    return (double)rand() / (double)RAND_MAX;
}
