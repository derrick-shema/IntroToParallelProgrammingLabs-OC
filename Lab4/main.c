#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>

//reduced number of elements to be able to run on my computer and avoid segmentation fault
static const long Num_To_Sort = 10000000;


// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
// Function to swap two elements 

/*Merge-Sort Algorithm taken from this resource;
* https://www.interviewbit.com/tutorial/merge-sort-algorithm/*/

void merge(int *Arr, int start, int mid, int end) {
	// create a temp array
	int temp[end - start + 1];

	// crawlers for both intervals and for temp
	int i = start, j = mid+1, k = 0;

	// traverse both arrays and in each iteration add smaller of both elements in temp 
	while(i <= mid && j <= end) {
		if(Arr[i] <= Arr[j]) {
			temp[k] = Arr[i];
			k += 1; i += 1;
		}
		else {
			temp[k] = Arr[j];
			k += 1; j += 1;
		}
	}

	// add elements left in the first interval 
	while(i <= mid) {
		temp[k] = Arr[i];
		k += 1; i += 1;
	}

	// add elements left in the second interval 
	while(j <= end) {
		temp[k] = Arr[j];
		k += 1; j += 1;
	}

	// copy temp to original interval
	for(i = start; i <= end; i += 1) {
		Arr[i] = temp[i - start];
    }

}

// Arr is an array of integer type
// start and end are the starting and ending index of current interval of Arr

void mergeSort_s(int *Arr, int start, int end) {
	if(start < end) {
		int mid = (start + end) / 2;
		mergeSort_s(Arr, start, mid);
		mergeSort_s(Arr, mid+1, end);
		merge(Arr, start, mid, end);
	}
}

//Pragma directives based on this StackOverflow resource: https://stackoverflow.com/questions/13811114/parallel-merge-sort-in-openmp
void mergeSort_p(int *Arr, int start, int end) {
    
        if(start < end) {
            int mid = (start + end) / 2;
          
                #pragma omp task
                mergeSort_p(Arr, start, mid);
                mergeSort_p(Arr, mid+1, end);
            #pragma omp taskwait
            merge(Arr, start, mid, end);
	    }
    
}

int main() {
    int low, size = 0;
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = rand_r(&seed);
            size++;
        }
    }



    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    mergeSort_s(arr_s,low,size-1);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    mergeSort_p(arr_p, low, size-1);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}

