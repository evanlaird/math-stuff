// Basic matrix definition. Probably a better way.
// This array pointer at the end doesnt seem to work well
typedef struct {
    int sideLength;
    int totalSize;
    char **points; // Only storing (1,0,-1) in the matrix points
} Matrix;

// A nice way to package the temperature information
typedef struct {
    float start;
    float stop;
    float step;
} TempInfo;

// Package Metropolis algorithm step information. Probably best to keep
// all of these numbers mod 2 because we will do lots of divsion / mod
// throughout.
typedef struct {
    int totalSteps;     // Total iterations through lattice
    int relaxationSteps;// Wait this many steps before measuring
    int loopCount;      // After relaxationSteps, measure when N % loopCount = 0
} StepInfo;
