    #include <stdlib.h>
    #include <stdio.h>
    #include <limits.h>
    #include <sys/time.h>
    #include <omp.h>

    static int cmp(int const* self, int const* other)
    {
        if(*self > *other)
        {
            return 1;
        }
        if (*self < *other)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    int* merge (int *start, int* middle, int* end, int (*cmp)(int const*, int const*))
    {
        int * res = malloc(sizeof(int) * (end-start));
        int * out = res;

        int* start_iter = start;
        int* middle_iter = middle;


        #pragma omp task
        while(start_iter!=middle && middle_iter != end)
        {
            if(cmp(start_iter, middle_iter)<=0)
            {
                *out++ = *start_iter++;
            }
            else
            {
                *out++ = *middle_iter++;
            }
        }

        #pragma omp task
        while (start_iter!=middle)
        {
            *out++ = *start_iter++;
        }
        while (middle_iter != end)
        {
            *out++ = *middle_iter++;
        }
        return out;
    }





    int main(int argc, char**argv)
    {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        int* data = malloc(n * sizeof(*data));
        for(int i = 0; i < n; ++i)
            data[i] = (unsigned)i * (INT_MAX / 3);

        struct timeval start, stop;
        gettimeofday(&start, NULL);
	    int* idemi = data + n/4;
	
        #pragma omp parallel
        #pragma omp single
        {
            #pragma omp task
            qsort(data, n/2, sizeof(*data), (int (*)(void const*, void const*))cmp);
            #pragma omp task
            qsort(data + n/2, n-n/2, sizeof(*data), (int (*)(void const*, void const*))cmp);
            #pragma omp taskwait
            {
            struct timeval start, stop;
            gettimeofday(&start, NULL);
                merge(data, data+n/2, data+n, cmp);
            gettimeofday(&stop, NULL);
            printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
            }
        }
        gettimeofday(&stop, NULL);
        volatile __attribute__((unused)) int anchor = data[n/2]; // why that?
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }
 
