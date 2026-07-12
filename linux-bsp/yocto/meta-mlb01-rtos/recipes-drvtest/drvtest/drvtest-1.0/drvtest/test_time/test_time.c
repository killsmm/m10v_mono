/******************************************************************************/
/**
 *  @file   timer_test.c
 *  @brief  test for timer
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>


double get_dtime(void);


/******************************************************************************/
/**
 *  @brief  timer_test
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main( int argc, char** argv )
{
    double d0, d1, d2, d3, d4;
    struct timeval tv[10];
    struct timespec ctv[10];
    struct timespec req, tp;
    int i;

    d0 = get_dtime();
    usleep(1000);
    d1 = get_dtime();
    usleep(10000);
    d2 = get_dtime();
    usleep(100000);
    d3 = get_dtime();
    sleep(1);
    d4 = get_dtime();

    printf("-----------------\n");
    printf("usleep 1000  : %f\n", d1 - d0);
    printf("usleep 10000 : %f\n", d2 - d1);
    printf("usleep 100000: %f\n", d3 - d2);
    printf(" sleep       : %f\n", d4 - d3);

    req.tv_sec  = 0;
    req.tv_nsec = 1000000; // 1ms

    for (i = 0; i < 10; i++) {
        nanosleep(&req, NULL);
        gettimeofday(tv + i, NULL);
    }

    printf("-----------------\n");
    for (i = 0; i < 10; i++) {
        printf("nanosleep %ld-%06ld\n", (long)tv[i].tv_sec, (long)tv[i].tv_usec);
    }

    req.tv_sec  = 0;
    req.tv_nsec = 1000000; // 1ms

    for (i = 0; i < 10; i++) {
        nanosleep(&req, NULL);
        clock_gettime(CLOCK_MONOTONIC, &tp);
        ctv[i].tv_sec  = tp.tv_sec;
        ctv[i].tv_nsec = tp.tv_nsec;
    }

    printf("-----------------\n");
    for (i = 0; i < 10; i++) {
        printf("MONOTONIC %ld %06ld\n", (long)ctv[i].tv_sec, (long)ctv[i].tv_nsec);
    }


    return 0;
}

double get_dtime(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
}
