#ifndef DATATYPES_H_
#define DATATYPES_H_

// Default matrix size (always square)
#define DEFAULT_LATTICE_SIZE 32
// iterations before first measurement
#define ITER_FIRST_MEASURE   5000
// iterations between measurements
#define ITER_MEASURE         1000
// Total stops
#define ITER_TOTAL           10000

// Basic matrix definition. Probably a better way.
// This array pointer at the end doesnt seem to work well
typedef struct {
    int sideLength;
    int totalSize;
    char **points; // Only storing (1,0,-1) in the matrix points
} Matrix;

Matrix *Matrix_create(int length);
void Matrix_destroy(Matrix *lattice);
void Matrix_print(Matrix *lattice);

// A nice way to package the temperature information
// FIXME: maybe just put the temp array in here too?
typedef struct {
    int totalSteps;
    float start;
    float stop;
    float step;
    float *tempRange;
} TempInfo;
TempInfo *TempInfo_create(float start, float stop, float step);
void TempInfo_destroy(TempInfo *temps);
void TempInfo_print(TempInfo *temps);

// Package Metropolis algorithm step information. Probably best to keep
// all of these numbers mod 2 because we will do lots of divsion / mod
// throughout.
typedef struct {
    int totalSteps;     // Total iterations through lattice
    int relaxationSteps;// Wait this many steps before measuring
    int loopCount;      // After relaxationSteps, measure when N % loopCount = 0
} StepInfo;
void StepInfo_print(StepInfo *stepInfo);

// Declaring this here for now because i don't know how to structure c programs
void die(const char *message);

#endif /*DATATYPES_H_*/
