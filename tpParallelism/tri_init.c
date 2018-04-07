    #include <stdlib.h>
    #include <stdio.h>
    #include <limits.h>
    #include <sys/time.h>

    static int cmp(int const* self, int const* other) {
        return self - other;
    }

    int main(int argc, char**argv) {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        int* data = malloc(n * sizeof(*data));
        for(int i = 0; i < n; ++i)
            data[i] = (unsigned)i * (INT_MAX / 3);

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        qsort(data, n, sizeof(*data), (int (*)(void const*, void const*))cmp);
        gettimeofday(&stop, NULL);
        volatile __attribute__((unused)) int anchor = data[n/2]; // why that?
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }

