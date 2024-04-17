#include "stdio.h"
#include <cstdlib>
#include <sys/time.h>
#include <omp.h>
#define NUM_THREADS 4


#define VECTOR_SIZE 97637

int main(void)
{
    printf("%s\n", "Members of group are:");
    printf("\n%s\n", "Sepehr - 810197637");
    printf("%s\n", "Abbas - 810198415");

    struct timeval start, end;

    float meanResult;
    float meanResultParallel;

    float *fArray;
    fArray = new float[VECTOR_SIZE];

    if (!fArray)
    {
        printf("Memory allocation error!!\n");
        return 1;
    }

    // Initialize vectors with random numbers
    for (long i = 0; i < VECTOR_SIZE; i++)
        fArray[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));

    // Serial implementation for Mean
    gettimeofday(&start, NULL);

    float fSTmpRes = 0.0;
    for (long i = 0; i < VECTOR_SIZE; i++)
        fSTmpRes += fArray[i];

    meanResult = fSTmpRes / VECTOR_SIZE;

    gettimeofday(&end, NULL);
    long seconds1 = (end.tv_sec - start.tv_sec);
    long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("\nSerial Result for Mean: Mean = %f and run time = %ld seconds and %ld micro seconds\n", meanResult, seconds1, micros1);
    float timeS = (seconds1 * 1000000) + micros1;

    // Parallel implementation for Mean using OpenMP
    gettimeofday(&start, NULL);

    float sum = 0.0;
#pragma omp parallel for reduction(+: sum) schedule(static) num_threads(NUM_THREADS)
    for (long i = 0; i < VECTOR_SIZE; i++)
    {
        sum += fArray[i];
    }

    meanResultParallel = sum / VECTOR_SIZE;

    gettimeofday(&end, NULL);

    long seconds2 = (end.tv_sec - start.tv_sec);
    long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Parallel Result for Mean: Mean = %f and run time = %ld seconds and %ld micro seconds\n", meanResultParallel, seconds2, micros2);
    float timeP = (seconds2 * 1000000) + micros2;

    printf("Speedup = %f\n\n", (float)(timeS) / (float)timeP);

    delete[] fArray;
    return 0;
}
