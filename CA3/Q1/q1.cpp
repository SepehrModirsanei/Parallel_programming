#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#define NUM_THREADS 4
#define VECTOR_SIZE 4194304

int main(void)
{
    printf("%s\n", "Members of group are:");
    printf("\n%s\n", "Sepehr - 810197637");
    printf("%s\n", "Abbas - 810198415");

    struct timeval start, end;

    float minSerial;
    int indexSerial;

    float *fArray;
    fArray = new float[VECTOR_SIZE];

    if (!fArray)
    {
        printf("Memory allocation error!!\n");
        return 1;
    }

    // Initialize vectors with random numbers
#pragma omp parallel for num_threads(NUM_THREADS)
    for (long i = 0; i < VECTOR_SIZE; i++)
    {
        fArray[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));
    }

    // Serial implementation
    gettimeofday(&start, NULL);
    minSerial = fArray[0];
    for (long i = 1; i < VECTOR_SIZE; i++)
    {
        if (fArray[i] < minSerial)
        {
            minSerial = fArray[i];
            indexSerial = i;
        }
    }
    gettimeofday(&end, NULL);
    long seconds1 = (end.tv_sec - start.tv_sec);
    long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("\nSerial Result: min = %f and index = %d and run time = %ld seconds and %ld micro seconds\n", minSerial, indexSerial, seconds1, micros1);
    float timeS = (seconds1 * 1000000) + micros1;

    // Parallel implementation
    gettimeofday(&start, NULL);
    float minParallel = fArray[0];
    int indexParallel = 0;

    // Arrays to store local minimums and indices for each thread
    float minValues[NUM_THREADS];
    int minIndices[NUM_THREADS];

#pragma omp parallel num_threads(NUM_THREADS)
    {
        int threadID = omp_get_thread_num();
        minValues[threadID] = fArray[0];
        minIndices[threadID] = 0;

#pragma omp for schedule(static)
        for (long i = 1; i < VECTOR_SIZE; i++)
        {
            if (fArray[i] < minValues[threadID])
            {
                minValues[threadID] = fArray[i];
                minIndices[threadID] = i;
            }
        }
    }

    // Reduction step to find the global minimum and index
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (minValues[i] < minParallel)
        {
            minParallel = minValues[i];
            indexParallel = minIndices[i];
        }
    }

    gettimeofday(&end, NULL);
    long seconds2 = (end.tv_sec - start.tv_sec);
    long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Parallel Result: min = %f and index = %d and run time = %ld seconds and %ld micro seconds\n", minParallel, indexParallel, seconds2, micros2);
    float timeP = (seconds2 * 1000000) + micros2;

    printf("Speedup = %f\n\n", (float)(timeS) / (float)timeP);

    delete[] fArray;
    return 0;
}
