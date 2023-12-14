#include 	"stdio.h"
#include 	<sys/time.h>
#include 	<smmintrin.h>
#include    <math.h>

#define		VECTOR_SIZE		  	4194304

int main (void)
{	
	printf("%s\n", "Members of group are:");
	printf("\n%s\n", "Sepehr - 810197637");
	printf("%s\n", "Abbas - 810198415");
	
	struct timeval start, end;

	float minSerial;
	int indexSerial;

	float *fArray;
	fArray = new float [VECTOR_SIZE];

	if (!fArray) {
		printf ("Memory allocation error!!\n");
		return 1;
	}

	// Initialize vectors with random numbers
	for (long i = 0; i < VECTOR_SIZE; i++)
		fArray[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));

	// Serial implementation
	gettimeofday(&start, NULL);
	minSerial = fArray[0];
	for (long i = 1; i < VECTOR_SIZE; i++)
		if(fArray[i]<minSerial){
			minSerial = fArray[i];
			indexSerial = i;
		}
	gettimeofday(&end, NULL);
	long seconds1 = (end.tv_sec - start.tv_sec);
	long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
	printf ("\nSerial Result: min = %f and index = %d and run time = %ld seconds and %ld micro seconds\n", minSerial, indexSerial, seconds1, micros1);
	float timeS = (seconds1 * 1000000) + micros1;

	// Parallel implementation
	gettimeofday(&start, NULL);
	__m128 vec;
	const __m128 increment = _mm_set1_ps(4.0);
	__m128 indices         = _mm_set_ps(3.0, 2.0, 1.0, 0.0);
	__m128 minindices      = indices;
	__m128 minvalues       = _mm_loadu_ps((float*)fArray);
	
	for (long i = 4; i < VECTOR_SIZE; i+=4) {
		indices = _mm_add_ps(indices, increment);

		vec = _mm_loadu_ps (&fArray[i]);
		
		__m128 lt = _mm_cmplt_ps(vec, minvalues);
		minindices = _mm_blendv_ps(minindices, indices, lt);
		minvalues  = _mm_min_ps(vec, minvalues);
		
	}

	float values_array[4];
    float indices_array[4];

    _mm_store_ps((float*)values_array, minvalues);
    _mm_store_ps((float*)indices_array, minindices);

    int  indexParallel = indices_array[0];
    float minParallel= values_array[0];
    for (int i=1; i < 4; i++) {
        if (values_array[i] < minParallel) {
            minParallel= values_array[i];
            indexParallel = indices_array[i];
        }
		else if (values_array[i] == minParallel) {
            indexParallel = std::fmin(indexParallel, size_t(indices_array[i]));
        }
    }

	gettimeofday(&end, NULL);
	long seconds2 = (end.tv_sec - start.tv_sec);
	long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
	printf ("Parallel Result: min = %f and index = %d and run time = %ld seconds and %ld micro seconds\n", minParallel, indexParallel, seconds2, micros2);
	float timeP = (seconds2 * 1000000) + micros2;
	
	printf ("Speedup = %f\n\n", (float) (timeS)/(float) timeP);

	return 0;
}
