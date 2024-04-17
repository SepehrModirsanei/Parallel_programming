#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 16
#define VECTOR_SIZE 400000000

float *fArray;
pthread_mutex_t mutex;
float minParallel;
int indexParallel;

void *initializeArray(void *arg) {
    long start = (long)arg;
    long end = start + VECTOR_SIZE / NUM_THREADS;

    // Each thread initializes its portion of the array
    float localMin = fArray[start];
    int localIndex = start;

    for (long i = start + 1; i < end; i++) {
        if (fArray[i] < localMin) {
            localMin = fArray[i];
            localIndex = i;
        }
    }

    // Return local minimum and its index
    float *result = new float[2];
    result[0] = localMin;
    result[1] = static_cast<float>(localIndex);
    
    return result;
}



int main(void) {
    printf("%s\n", "Members of group are:");
    printf("\n%s\n", "Sepehr - 810197637");
    printf("%s\n", "Abbas - 810198415");

    struct timeval start, end;

    float minSerial;
    int indexSerial;

    fArray = new float[VECTOR_SIZE];

    if (!fArray) {
        printf("Memory allocation error!!\n");
        return 1;
    }
     for (long i = 0; i < VECTOR_SIZE; i++) {
        fArray[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));
    }




    

    gettimeofday(&start, NULL);

    // Serial implementation
    minSerial = fArray[0];
    for (long i = 1; i < VECTOR_SIZE; i++) {
        if (fArray[i] < minSerial) {
            minSerial = fArray[i];
            indexSerial = i;
        }
    }
    gettimeofday(&end, NULL);
    long seconds2 = (end.tv_sec - start.tv_sec);
    long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
    float timeS = (seconds2 * 1000000) + micros2;
    printf("\nSerial Result: min = %f and index = %d and %f seconds \n", minSerial, indexSerial,timeS);
   

    // Parallel implementation
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    minParallel = fArray[0];
    indexParallel = 0;


    // Create threads
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, initializeArray, (void *)(i * (VECTOR_SIZE / NUM_THREADS)));
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    gettimeofday(&start, NULL);
    for (long i = 1; i < VECTOR_SIZE; i++) {
        if (fArray[i] < minParallel) {
            minParallel = fArray[i];
            indexParallel = i;
        }
    }

    gettimeofday(&end, NULL);
    long seconds1 = (end.tv_sec - start.tv_sec);
    long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
    float timeP = (seconds1 * 1000000) + micros1;
   
    printf("Parallel Result: min = %f and index = %d and run time = %f seconds \n", minParallel, indexParallel, timeP);


    // Check if results match
    if (minSerial == minParallel ) {
        printf("Results match between serial and parallel implementations.\n");
    } else {
        printf("Results do not match between serial and parallel implementations.\n");
    }

    // Calculate speedup
    printf("\nThe speedup is: %f for %d number of threads\n", (float)timeS / (float)timeP, NUM_THREADS);

    delete[] fArray;
    return 0;
}
