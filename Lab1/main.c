/******************************************************************
 * SOURCES ;
 * 
 * Author 1: Adrian Statescu mergesortv@gmail.com http://adrianstatescu.com
 * Description: C Program to compute PI using a Monte Carlo Method.
 * MIT license
 * 
 * Author 2: mrigankdoshy
 * Description: monte-carlo-simulation-pi
 * Availability: https://github.com/mrigankdoshy/monte-carlo-simulation-pi/blob/master/Pi/OpenMP/main.c
 * 
 * Author 3: Jonathan Troyer
 * Description: OC Parallel Programming Lab 1 template
 * Availability: https://gitlab.com/OC-ParallelProgramming/lab-1/-/blob/master/main.c
 * 
 ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

// Returns a random value between -1 and 1
double getRand(unsigned int *seed) {
    return (double) rand_r(seed) * 2 / (double) (RAND_MAX) - 1;
}

/* For both Sequential and Parallel functions, 
 * I used Adrian Statescu monte carlo's algorithm, 
 * but followed mrigankdoshy for better naming strategy.*/

/**************************************************************************************/
long double Calculate_Pi_Sequential(long long number_of_tosses) {
    
    //variable numberInCircle keeps count of randomly generated points that fall in circle
    long long numberInCircle = 0;
    unsigned int seed = (unsigned int) time(NULL);

    //loop schedule validates random points (x,y) if they are within the circle 
    for (long long int i = 0; i < number_of_tosses; i++) {
        double x = getRand(&seed);                          
        double y = getRand(&seed);
        double distanceSquared = x*x + y*y;
        
        if (distanceSquared <= 1) 
        	numberInCircle++;
    }
    double pi = (double) numberInCircle / number_of_tosses * 4;
 
 printf("Approximate PI = %g", pi);

  return pi;
}

/*****************************************************************************************/
long double Calculate_Pi_Parallel(long long number_of_tosses) {
     
     //variable numberInCircle keeps count of randomly generated points that fall in circle
     long long numberInCircle = 0;

    //this directive specifies the number of threads to execute
    #pragma omp parallel num_threads(omp_get_max_threads())
        {
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) omp_get_thread_num();
        
        //cummulative count of validated points (those in circle) from different threads is summed up
        #pragma omp for reduction(+: numberInCircle)
        
        //loop schedule validates random points (x,y) if they are within the circle
            for (long long int i = 0; i < number_of_tosses; i++) {
                double x = getRand(&seed);
                double y = getRand(&seed);
                double distanceSquared = x*x + y*y;
        
                if (distanceSquared <= 1) 
        	    numberInCircle++;
            }
        }

    double pi = (double) numberInCircle / number_of_tosses * 4;
 
    printf("Approximate PI = %g", pi);

    return pi;
}
/*******************************************************************************************/

int main() {
    struct timeval start, end;

    long long num_tosses = 10000000;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long double sequential_pi = Calculate_Pi_Sequential(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long double parallel_pi = Calculate_Pi_Parallel(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    // This will print the result to 10 decimal places
    printf("?? = %.10Lf (sequential)\n", sequential_pi);
    printf("?? = %.10Lf (parallel)", parallel_pi);

    return 0;
}
