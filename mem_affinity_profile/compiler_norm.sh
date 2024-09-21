gcc -fopenmp -o norm mem_affinity_test.c \
 -I../fftw-3.3.10/api/ \
 -L../threads/.libs  -lfftw3_omp \
 -L../.libs/ -lfftw3 -lnuma -lpthread -lm