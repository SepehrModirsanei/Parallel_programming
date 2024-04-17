#include "stdio.h"
#include <sys/time.h>
#include <cmath>
#include <omp.h>
#define NUM_THREADS 4
#define VECTOR_SIZE 500000

int main(void)
{
    printf("%s\n", "Members of the group are:");
    printf("\n%s\n", "Sepehr - 810197637");
    printf("%s\n", "Abbas - 810198415");

    struct timeval start, end;

    float meanResult;
    float standardDeviation;

    float parallelMeanResult;
    float parallelStandardDeviation;

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

    // Serial implementation for Standard Deviation
    gettimeofday(&start, NULL);

    float sum = 0.0;
    float meanSum = 0.0;

    // Calculate mean
    for (long i = 0; i < VECTOR_SIZE; i++)
        meanSum += fArray[i];

    meanResult = meanSum / VECTOR_SIZE;

    // Calculate sum of squared differences
    for (long i = 0; i < VECTOR_SIZE; i++)
        sum += (fArray[i] - meanResult) * (fArray[i] - meanResult);

    standardDeviation = std::sqrt(sum / VECTOR_SIZE);

    gettimeofday(&end, NULL);
    long seconds1 = (end.tv_sec - start.tv_sec);
    long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("\nSerial Result for Standard Deviation: SD = %f and run time = %ld seconds and %ld micro seconds\n", standardDeviation, seconds1, micros1);
    float timeS = (seconds1 * 1000000) + micros1;

    // Parallel implementation for Standard Deviation using OpenMP
    gettimeofday(&start, NULL);

    float parallelSum = 0.0;
    float parallelMeanSum = 0.0;
    float partialSums[NUM_THREADS] = {0.0};

#pragma omp parallel for num_threads(NUM_THREADS) reduction(+:parallelMeanSum) schedule(static)
    for (long i = 0; i < VECTOR_SIZE; i++)
    {
        int threadID = omp_get_thread_num();
        partialSums[threadID] += fArray[i];
    }

    // Combine partial sums
    for (int i = 0; i < NUM_THREADS; i++)
    {
        parallelMeanSum += partialSums[i];
    }

#pragma omp parallel for reduction(+:parallelSum) schedule(static)
    for (long i = 0; i < VECTOR_SIZE; i++)
    {
        parallelSum += (fArray[i] - parallelMeanSum / VECTOR_SIZE) * (fArray[i] - parallelMeanSum / VECTOR_SIZE);
    }

    parallelStandardDeviation = std::sqrt(parallelSum / VECTOR_SIZE);

    gettimeofday(&end, NULL);

    long seconds2 = (end.tv_sec - start.tv_sec);
    long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Parallel Result for Standard Deviation: SD = %f and run time = %ld seconds and %ld micro seconds\n", parallelStandardDeviation, seconds2, micros2);
    float timeP = (seconds2 * 1000000) + micros2;

    printf("Speedup = %f\n\n", (float)(timeS) / (float)timeP);

    delete[] fArray;
    return 0;
}
