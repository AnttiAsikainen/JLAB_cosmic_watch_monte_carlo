/* Monte Carlo simulate the muon paths in cosmic muons */

// Input output 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Math library
#include <math.h>
// Time library
#include <time.h>
// Random number
#include "mt19937-64.h"
// Boolean variables 
#include <stdbool.h>

// Helper files:
#include "monte_carlo-options.h"
#include "monte_carlo-functions.h"




int main(int argc, char* argv[]){

    /* Declare variables */
    int _index;                  // Index of the simulation
    struct params Params;
    FILE* outputdata;
    FILE* outputparam;

    /* Read input */
    CheckAndReadInput(argc, argv, &outputparam, &outputdata, &Params);

    // Store a log of the parameters    
    store_params(argc, argv, outputparam, Params);

    _index = 0;
    init_genrand64(Params.seed);


    while( _index < Params.FinalIndex )
    {
        _index++;
        Get_length_theta(Params, outputdata);
    }

    fclose(outputparam);
    fclose(outputdata);

    exit(1);
}