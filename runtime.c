#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include "datatypes.h"
#include "array.h"

#define BUF_SIZE 128

// Global state. Not great.
TempInfo *temps;
Matrix *lattice;
StepInfo *stepInfo;

// Forward declares
void configureFlippingProbabilityArray(double (*)[5], const double (*)[5], const float);
int energyArrayIndexOfValue(double , const double (*)[5]);
void ising_stepper(Matrix *, StepInfo *, TempInfo *, float);
void W_describe_in_buffer(double (*)[5], char (*)[BUF_SIZE]);
void E_describe_in_buffer(const double (*)[5], char (*)[BUF_SIZE]);

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
    printf("USAGE: ./runtime -[L|N|J|t|T]");
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
    // Matrix_print(lattice);
    temps = TempInfo_create(start, stop, t_step);
    // TempInfo_print(temps);
    stepInfo = CreateStepInfo(steps, ITER_FIRST_MEASURE, ITER_MEASURE);

    printf("Monte-Carlo simulation of the Ising Model (2D)\n");
    printf("\tMatrix Size: %dx%d\n", lattice_size, lattice_size);
    printf("\tTemp Range : %.3f-%.3f\n", start, stop);
    printf("\t# of Steps : %d\n", steps);
    ising_stepper(lattice, stepInfo, temps, coupling);

    Matrix_destroy(lattice);
    TempInfo_destroy(temps);
    free(stepInfo);

    printf("Finished\n");
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

    // local handle to the matrix
    char **matrix = lattice->points;

    const int side_length = lattice->sideLength;
    // Modulo look-up table (plus)
    int *px = array_int(side_length, true);
    int *py = array_int(side_length, true);
    int *mx = array_int(side_length, true);
    int *my = array_int(side_length, true);
    /*
     * Initialize the modulo arrays. p_[i] = M[i] + 1, m_[i] = M[i] - 1 wrapped
     * on a torus (because, physics)
     */
    int i = 0;
    for (i = 0; i < side_length; i++) {
        px[i] = py[i] = i + 1;
        mx[i] = my[i] = i - 1;
        //printf("i:%2d  px[n]:%2d  py[j]:%2d  mx[n]:%2d  my[j]:%2d\n", i, px[i], py[i], mx[i], my[i]);
    }
    px[side_length - 1] = py[side_length - 1] = 0;
    //printf("side_length:%2d  px[31]:%2d  py[31]:%2d\n", side_length, px[side_length - 1], py[side_length - 1]);
    mx[0] = my[0] = side_length - 1;
    //printf("side_length - 1:%2d  mx[0]:%2d my[0]:%2d\n", side_length - 1, mx[0], my[0]);

    // Since dE = 2*j*sumNeighbors*currentSpin, these are the only possible values
    // Assuming J = 1, we have 2*sumNeigh in this list
    static const double E_vals[5] = {
         8.0,
         4.0,
         0.0,
        -4.0,
        -8.0
    };

    // Aggregators
    double Eavg = 0.0; // Average energy

    // This is the monte-carlo stepper
    // Every temp in range, for N steps, for every point
    int n = 0,j = 0,temp = 0,step = 0;
    float current_temp = 0.f;
    double dE = 0.0;
    double W[5] = {
         8.0,
         4.0,
         0.0,
        -4.0,
        -8.0
    }; // flipping probability
    double boltz = 0.0, flip = 0.0;
    for (temp = 0; temp < tempInfo->totalSteps; temp++) {
        current_temp = tempInfo->tempRange[temp];
        // printf("current_temp: %f\n", current_temp);
        // Calculate flipping probability for every temp.
        configureFlippingProbabilityArray(&W, &E_vals, current_temp);
        for (step = 0; step < stepInfo->totalSteps; step++) {
            // printf("Average Energy: %f\n", Eavg);
            Eavg = 0.0;
            //TODO: Rename n, j (wtf?) to (x,y) so I don't have a conniption
            for (n = 0; n < side_length; n++) {
                for (j = 0; j < side_length; j++) {
                    //printf("n:%d,  j:%d\n",n,j);
                    //printf("n:%2d  j:%2d  px[n]:%2d  py[j]:%2d  mx[n]:%2d  my[j]:%2d\n", n, j, px[n], py[j], mx[n], my[j]);
                    // Calculate dE for a given spin
                    //printf("%d\n", matrix[n][j]);
                    dE = 2.0 * coupling * matrix[n][j] *   (
                        matrix[px[n]][j] + matrix[mx[n]][j] +
                        matrix[n][py[j]] + matrix[n][my[j]]
                    );

                    // Aggregate the value
                    Eavg = Eavg - dE;

                    if (dE <= 0) {
                        matrix[n][j] = -matrix[n][j];
                    } else {
                        // Pick a random number and see if we flip
                        int idx = energyArrayIndexOfValue(dE, &E_vals);
                        if (idx < 0 || idx >= 5) {
                            char buffer [BUF_SIZE];
                            E_describe_in_buffer(&E_vals, &buffer);
                            sprintf(buffer, "\n Matrix points: %d (%d, %d, %d, %d)", \
                                matrix[n][j],
                                matrix[px[n]][j],
                                matrix[mx[n]][j],
                                matrix[n][px[j]],
                                matrix[n][mx[j]]
                            );
                            printf("skipping value: dE:%f j:%f E_vals:%s\n", dE, coupling, buffer);
                            continue;
                        } else {
                            boltz = W[idx];
                            flip = drand();
                            if (flip <= boltz) {
                                matrix[n][j] = -matrix[n][j];
                            }
                        }
                    }
                }
            }
        }
        printf("Finished Temperatue: %f\n", current_temp);
    }
}

/// Takes references to the W(probability) array and the energy values array
/// and divides by temperature. These are flipping energies.
void configureFlippingProbabilityArray(double (*W)[5], const double (*E_vals)[5], const float temp)
{
    int i = 0;
    for (i = 0; i < 5; i++) {
        (*W)[i] = (*E_vals)[i] / temp;
    }
}

void W_describe_in_buffer(double (*W)[5], char (*buffer)[BUF_SIZE])
{
    sprintf(*buffer, "(%f %f %f %f %f)", (*W)[0], (*W)[1], (*W)[2], (*W)[3], (*W)[4]);
}

void E_describe_in_buffer(const double (*E)[5], char (*buffer)[BUF_SIZE])
{
    sprintf(*buffer, "(%f %f %f %f %f)", (*E)[0], (*E)[1], (*E)[2], (*E)[3], (*E)[4]);
}

int energyArrayIndexOfValue(double dE, const double (*E_vals)[5])
{
    double delta = 1e-10;
    int i = 0;
    for (i = 0; i < 5; i++) {
        if ((dE - (*E_vals)[i]) < delta) {
            return i;
        }
    }

    return -1;
}
