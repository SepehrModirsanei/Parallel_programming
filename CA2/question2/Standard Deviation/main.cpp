#include 	"stdio.h"
#include 	<sys/time.h>
#include 	<smmintrin.h>
#include    <math.h>

#define		VECTOR_SIZE		  	4194304

int main (void)
{	

	printf("%s\n", "Members of group are:");
	printf("\n%s\n", "Mehdi Jahani - 810196660");
	printf("%s\n", "Soudabeh Mohammad Hashemi - 810197688");
	
	struct timeval start, end;

	float meanResult;
	float standardDeviation;

	float parallelMeanResult;
	float parallelStandardDeviation;

    float fSTmpRes[4];
	float fSRes;
	float fVRes;

	float *fArray;
	fArray = new float [VECTOR_SIZE];

	if (!fArray) {
		printf ("Memory allocation error!!\n");
		return 1;
	}


	// Initialize vectors with random numbers
	for (long i = 0; i < VECTOR_SIZE; i++)
		fArray[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));


	// Serial implementation for Standard Deviation
	gettimeofday(&start, NULL);

	fSTmpRes[0] = fSTmpRes[1] = fSTmpRes[2] = fSTmpRes[3] = 0.0;
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[0] += fArray[i];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[1] += fArray[i + 1];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[2] += fArray[i + 2];
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[3] += fArray[i + 3];
	fSRes = fSTmpRes[0] + fSTmpRes[1] + fSTmpRes[2] + fSTmpRes[3];

	meanResult = fSRes / VECTOR_SIZE;

	fSTmpRes[0] = fSTmpRes[1] = fSTmpRes[2] = fSTmpRes[3] = 0.0;

	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[0] += (fArray[i]-meanResult)*(fArray[i]-meanResult);
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[1] += (fArray[i+1]-meanResult)*(fArray[i+1]-meanResult);
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[2] += (fArray[i+2]-meanResult)*(fArray[i+2]-meanResult);
	for (long i = 0; i < VECTOR_SIZE; i+=4)
		fSTmpRes[3] += (fArray[i+3]-meanResult)*(fArray[i+3]-meanResult);
	fSRes = fSTmpRes[0] + fSTmpRes[1] + fSTmpRes[2] + fSTmpRes[3];

	standardDeviation = sqrt(fSRes / VECTOR_SIZE);
    

	gettimeofday(&end, NULL);
	long seconds1 = (end.tv_sec - start.tv_sec);
	long micros1 = ((seconds1 * 1000000) + end.tv_usec) - (start.tv_usec);
	printf ("\nSerial Result for Standard Deviation: SD = %f and run time = %ld seconds and %ld micro seconds\n", standardDeviation, seconds1, micros1);
	float timeS = (seconds1 * 1000000) + micros1;


	// Parallel implementation for Standard Deviation
	gettimeofday(&start, NULL);

	__m128 vec;
	__m128 sum = _mm_set1_ps(0.0f);
	for (long i = 0; i < VECTOR_SIZE; i+=4) {
		vec = _mm_loadu_ps (&fArray[i]);
		sum = _mm_add_ps (sum, vec);
	}

	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_hadd_ps (sum, sum);
	fVRes = _mm_cvtss_f32 (sum);

    parallelMeanResult = fVRes / VECTOR_SIZE;

	sum = _mm_set1_ps(0.0f);
	__m128 mean = _mm_set1_ps(parallelMeanResult);
	__m128 size = _mm_set1_ps((float)VECTOR_SIZE);

		for (long i = 0; i < VECTOR_SIZE; i+=4) {
		vec = _mm_loadu_ps (&fArray[i]);
		vec = _mm_sub_ps (mean, vec);
		vec = _mm_mul_ps (vec, vec);
		sum = _mm_add_ps (sum, vec);
	}

	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_hadd_ps (sum, sum);
	sum = _mm_div_ps (sum, size);
	sum = _mm_sqrt_ps (sum);
	fVRes = _mm_cvtss_f32 (sum);

	parallelStandardDeviation = fVRes;



	gettimeofday(&end, NULL);

	long seconds2 = (end.tv_sec - start.tv_sec);
	long micros2 = ((seconds2 * 1000000) + end.tv_usec) - (start.tv_usec);
	printf ("Parallel Result for Mean: Mean = %f and run time = %ld seconds and %ld micro seconds\n", parallelStandardDeviation, seconds2, micros2);
	float timeP = (seconds2 * 1000000) + micros2;
	
	
	printf ("Speedup = %f\n\n", (float) (timeS)/(float) timeP);

	return 0;
}
