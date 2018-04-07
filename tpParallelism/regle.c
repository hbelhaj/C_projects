    #include <stdlib.h>
    #include <stdio.h>
    #include <stddef.h>
    #include <sys/time.h>

    static void rule_incr(int* pos, int level) {
      *pos = level;
      struct timespec duration = {0, 10000000};
      nanosleep(&duration, NULL);
    }

    static int ilog(unsigned val) {
      int n = 0;
      while(val)
      {
        n += 1;
        val >>=2;
      }
      
      return n;
    }

    static void rule_aux(int* start, int* stop, int level) {
      if(level==0) return;
      ptrdiff_t n = stop - start;

      #pragma omp task
      rule_incr(start + n/2, level);
      rule_aux(start, start + n/2, level - 1);
      rule_aux(start + n/2, stop, level - 1);

          

    }

    static void rule(int* start, int* stop) {
      #pragma omp parallel
      #pragma omp master
      rule_aux(start, stop, 1 + ilog(stop - start));
    }

    int main(int argc, char**argv) {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        // is that the best data type?
        int* data = calloc(n, sizeof(*data));

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        rule(data, data + n);
        gettimeofday(&stop, NULL);
        for(int j = 0; j < n; ++j) {
          for(int k = 0; k < data[j]; ++k)
            putchar('#');
          putchar('\n');
        }
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }
