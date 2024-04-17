#include <stdio.h>
#include <cstdlib>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 4
#define VECTOR_SIZE 97637

float *fArray;
pthread_mutex_t mutex;
float meanResult;
float meanResultParallel;

void *calculateMean(void *arg) {
    long threadId = (long)arg;
    long start = threadId * (VECTOR_SIZE / NUM_THREADS);
    long end = (threadId + 1) * (VECTOR_SIZE / NUM_THREADS);

    float localSum = 0.0;
    for (long i = start; i < end; i++) {
        localSum += fArray[i];
    }

    pthread_mutex_lock(&mutex);
    meanResultParallel += localSum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    printf("%s\n", "Members of group are:");
    printf("\n%s\n", "Sepehr - 810197637");
    printf("%s\n", "Abbas - 810198415");

    struct timeval start, end;

    fArray = new float[VECTOR_SIZE];

    if (!fArray) {
        printf("Memory allocation error!!\n");
        return 1;
    }

    // Initialize vectors with random numbers
    for (long i = 0; i < VECTOR_SIZE; i++)
        fArray[i] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));

    // Serial implementation for Mean
    gettimeofday(&start, NULL);

    float sum = 0.0;
    for (long i = 0; i < VECTOR_SIZE; i++)
        sum += fArray[i];

    meanResult = sum / VECTOR_SIZE;

    gettimeofday(&end, NULL);
    long seconds1 = (end.tv_sec - start.tv_sec);
    long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("\nSerial Result for Mean: Mean = %.2f and run time = %ld seconds and %ld micro seconds\n", meanResult, seconds1, micros1);
    float timeS = (seconds1 * 1000000) + micros1;

    // Parallel implementation for Mean using pthreads
    gettimeofday(&start, NULL);

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    meanResultParallel = 0.0;

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, calculateMean, (void *)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    meanResultParallel /= VECTOR_SIZE;

    pthread_mutex_destroy(&mutex);

    gettimeofday(&end, NULL);

    long seconds2 = (end.tv_sec - start.tv_sec);
    long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Parallel Result for Mean: Mean = %.2f and run time = %ld seconds and %ld micro seconds\n", meanResultParallel, seconds2, micros2);
    float timeP = (seconds2 * 1000000) + micros2;
    
    // Calculate speedup
    printf("\nThe speedup is: %f for %d number of threads\n", (float)timeS / (float)timeP, NUM_THREADS);

    delete[] fArray;
    return 0;
}
