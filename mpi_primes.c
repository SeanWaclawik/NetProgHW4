/*
 * Network Programming
 * Homework 4
 *
 * Sean Waclawik
 * Garret Premo
 *
 */

#include <time.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>


int sizeVar = 0;

// TODO remove
void print(unsigned int* nums, unsigned int count);
int generatePrimes(unsigned int num, int mpi_commsize, int mpi_myrank);
void sig_handler(int signo);


int main(int argc, char **argv)
{
    double start, end;
    int mpi_commsize, mpi_myrank, i, c, vals;

    int trigger = 10;
    int msec = 0;

    clock_t difference, before = clock();

    unsigned int nums[8] = {10, 100, 1000, 1020, 192302, 43342, 789807, 4980098};
    sizeVar = sizeof(nums)/sizeof(nums[0]);

    // set sig handler
    signal(SIGUSR1, sig_handler);

    // Init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);

    start = MPI_Wtime();
    
    if (mpi_myrank == 0) {
        printf("N\tPrimes\n");
    }

    for (i = 0; i < sizeVar; ++i) {
        difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;

        if(msec > trigger && mpi_myrank == 0) {
            raise(SIGUSR1);
        }
        
        c = generatePrimes(nums[i], mpi_commsize, mpi_myrank);
        MPI_Allreduce(&c, &vals, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        // if done print result
        if (mpi_myrank == 0) {
            printf("%d\t%d\n", nums[i], vals+1);
        }
    }

    end = MPI_Wtime();

    if (mpi_myrank == 0) {
        printf("time with %d ranks: %f\n", mpi_commsize, (end - start)); 
    }   
    
    MPI_Finalize();
    
    return 0;
}


// Does the actual work of finding the primes
int generatePrimes(unsigned int num, int mpi_commsize, int mpi_myrank) {
    int flag, count, i = 0;
    count = 0;
    unsigned int curr = 3+(2*mpi_myrank);

    while (curr < num) {

        flag = 0; 

        for (i = 2; i <= sqrt(curr); ++i) { 
            // not prime if divisable
            if(curr % i == 0) {
                flag = 1;
                break;
            }
        
        }

        if (flag == 0) {
            count+=1;
        }

        if (curr == 0xFFFFFFFF) { 
            break;
        }
        
        curr+=(mpi_commsize*2);

    }

    return count;

}

// takes care of signal handling
void sig_handler(int signo)
{
    if (signo == SIGUSR1) {
        printf("<Signal recieved>\n");

        //MPI_Finalize(); 
        //exit(0);
    }
}

