#include <fftw3.h>
#include <numa.h>
#include <numaif.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define LOOP_NUM 5
#define START gettimeofday(&start, NULL)
#define END(x)                              \
    gettimeofday(&end, NULL);               \
    seconds = end.tv_sec - start.tv_sec;    \
    useconds = end.tv_usec - start.tv_usec; \
    opt_utime = (seconds * 10e6 + useconds) / (x)

int main(int argc, char* argv[])
{
    int rank = 1;
    int n = atoi(argv[1]);
    int howmany =  atoi(argv[2]);
    int batch_num = (n * howmany + 5000 - 1) / 5000;
    int idist = n, odist = n;
    int istride = 1, ostride = 1;
    int *inembed = &n, *onembed = &n;

    struct timeval start, end;
    long seconds, useconds;
    double sin_utime, opt_utime;

    fftw_complex *in, *out;
    fftw_init_threads();
    int node = batch_num / 8 + 1;
    if (node != 1 && node % 2 == 1)
        node++;
    fftw_plan_with_nthreads(batch_num);
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n * howmany);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n * howmany);

    fftw_plan plan = fftw_plan_many_dft(rank, &n, howmany, in, inembed, istride, idist, out, onembed, ostride, odist, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan); // warm-up
    START;
    for (int i = 0; i < LOOP_NUM; i++) {
        fftw_execute(plan);
    }
    END(LOOP_NUM);

#ifdef TEST_MEM_AFFINITY
    FILE* fp;
    if ((fp = fopen("tmp.txt", "r")) == NULL) {
        printf("file open failed\n");
        return 0;
    }
    double norm_utime;
    fscanf(fp, "%lf", &norm_utime);
    printf("FFTW mem affinity: %lf\t", opt_utime);
    if (norm_utime < opt_utime) {
        if (opt_utime / norm_utime < 1.05)
            printf("same performance:\n");
        else {
            printf("neg opt: %lf\n", norm_utime / opt_utime);
        }
    } else {
        if (norm_utime / opt_utime < 1.05)
            printf("same performance:\n");
        else
            printf("speedup: %lf\n", norm_utime / opt_utime);
    }

#else
    printf("Size: %d,\t Batch: %d,\t FFTW mem norm: %lf,\t", n, howmany, opt_utime);
    FILE* fp;
    if ((fp = fopen("tmp.txt", "w")) == NULL) {
        printf("file open failed\n");
        return 0;
    }
    fprintf(fp, "%lf", opt_utime);
#endif

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return 0;
}
