sed -i '1i#define TEST_MEM_AFFINITY' mem_affinity_test.c

gcc -g -fopenmp -o mem_affinity mem_affinity_test.c \
 -I../fftw-3.3.10/api/ \
 -L../threads/.libs  -lfftw3_omp \
 -L../.libs/ -lfftw3 -lnuma -lpthread -lm

 sed -i '1d' mem_affinity_test.c