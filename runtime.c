#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "datatypes.h"

// Global state. Not great.
TempInfo *temps;
Matrix *lattice;

/*
 * **************************************************************
 */
// Lifetime management
void die(const char *message) {
    // Domain specifc, but I don't have a better way to do this...
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    if (lattice) {
        if (lattice->points) {
            free(lattice->points);
        }
        free(lattice);
    }

    if (temps) {
        if (temps->tempRange) {
            free(temps->tempRange);
        }
        free(temps);
    }

    exit(1);
}

void print_usage() {
    printf("USAGE: ./runtime -[L|N|J|t|t]");
}

int main(int argc, char *argv[]) {

    // Initialize the PRNG
    srand(time(NULL));

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
    float   t_step       = 0.1;

    // getopt() is an interesting stdlib function for argparsing.
    while ((option = getopt(argc, argv, "L:J:N:t:T:S:")) != -1) {
        switch (option) {
            case 'L': 
                lattice_size = abs(atoi(optarg));
                break;
            case 'J':
                coupling = atof(optarg);
                break;
            case 'N':
                steps = abs(atoi(optarg));
                break;
            case 't':
                start = atof(optarg);
                break;
            case 'T':
                stop = atof(optarg);
                break;
            case 'S':
                t_step = atof(optarg);
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

    // Setup structures to pass to ising_stepper
    lattice = Matrix_create(lattice_size);
    Matrix_print(lattice);           
    temps = TempInfo_create(start, stop, t_step);

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
    int i = 0;
    for (i = 0; i < side_length; i++) {
        px[i] = py[i] = i + 1;
        mx[i] = my[i] = i - 1;
    }
    px[side_length] = py[side_length] = 0;
    mx[0] = my[0] = side_length - 1;

    // This is the monte-carlo stepper
    // Every temp in range, for N steps, for every point
    int n,j,temp,step = 0;
    float current_temp = 0.f;
    for (temp = 0; temp < tempInfo->totalSteps; temp++) {
        current_temp = tempInfo->tempRange[temp];
        for (step = 0; step < stepInfo->totalSteps; step++) {
            for (n = 0; n < side_length; n++) {
                for (j = 0; j < side_length; j++) {
                }
            }
        }
    }
}
