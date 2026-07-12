/******************************************************************************/
/**
 *  @file   test_freerun_timer.c
 *  @brief  test for freerun timer
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <signal.h>
#include <string.h>

#define SEC_MSEC	(1000)
#define SEC_USEC	(SEC_MSEC * 1000)
#define SEC_NSEC	(SEC_USEC * 1000)

#define NSEC2USEC(ns)	((ns) / (SEC_NSEC / SEC_USEC))
#define NSEC2MSEC(ns)	((ns) / (SEC_NSEC / SEC_MSEC))

#define USEC2NSEC(us)	((us) * (SEC_NSEC / SEC_USEC))
#define MSEC2NSEC(ms)	((ms) * (SEC_NSEC / SEC_MSEC))
#define MSEC2USEC(ms)	((ms) * (SEC_USEC / SEC_MSEC))

#define TS_DIFF(new, old) { \
	.tv_sec = (new.tv_nsec > old.tv_nsec) ? new.tv_sec - old.tv_sec : new.tv_sec - old.tv_sec - 1,	\
	.tv_nsec = (new.tv_nsec > old.tv_nsec) ? new.tv_nsec - old.tv_nsec : new.tv_nsec - old.tv_nsec + SEC_NSEC, \
	}

static struct timespec prev_ts;
static clockid_t clkid = CLOCK_MONOTONIC;

static void event_handler(int sig, siginfo_t *si, void *dummy)
{
	struct timespec now_ts;
	clock_gettime(clkid, &now_ts);

	/* Show differences of the time */ {
		struct timespec diffts = TS_DIFF(now_ts, prev_ts);
		printf("Time difference: %ld sec %ld usec\n", (long)diffts.tv_sec,
			(long)NSEC2USEC(diffts.tv_nsec));
	}
	prev_ts = now_ts;
}

static void source_handler(int sig)
{
	struct timespec now_ts;
	clock_gettime(clkid, &now_ts);

	/* Show differences of the time */ {
		struct timespec diffts = TS_DIFF(now_ts, prev_ts);
		printf("Time difference: %ld sec %ld usec\n", (long)diffts.tv_sec,
			(long)NSEC2USEC(diffts.tv_nsec));
	}
	prev_ts = now_ts;
}

/******************************************************************************/
/**
 *  @brief  test_freerun_timer
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
	struct itimerval new_value;
	struct itimerspec its;
	struct sigaction sa;
	struct sigevent sev;
	timer_t timerid;
	int i;
	int s = atoi(argv[3]);

	if(strcmp(argv[1], "clock") == 0){
		// event timer test for user space
		clock_gettime(clkid, &prev_ts);

		if(strcmp(argv[2], "s") == 0){
			new_value.it_value.tv_sec = s;
			new_value.it_value.tv_usec = 0;
		}
		else if(strcmp(argv[2], "ms") == 0){
			new_value.it_value.tv_sec = 0;
			new_value.it_value.tv_usec = MSEC2USEC(s);
		}
		else if(strcmp(argv[2], "us") == 0){
			new_value.it_value.tv_sec = 0;
			new_value.it_value.tv_usec = s;
		}
		else{
			perror("argument error\n");
			return -4;
		}

		new_value.it_interval = new_value.it_value;

		signal(SIGALRM, source_handler);

		setitimer(ITIMER_REAL, &new_value, NULL);

		for (i=0; i < 10; i++)
			sleep(1);
	}
	else if(strcmp(argv[1], "event") == 0){
		/* Setup signal handler */
		sa.sa_flags = SA_SIGINFO;
		sa.sa_sigaction = event_handler;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
			perror("sigaction");
			return -1;
		}

		/* Setup timer */
		sev.sigev_notify = SIGEV_SIGNAL;
		sev.sigev_signo = SIGRTMIN;
		sev.sigev_value.sival_ptr = &timerid;
		if (timer_create(clkid, &sev, &timerid) == -1) {
			perror("timer_create");
			return -2;
		}

		printf("timer ID is 0x%lx\n", (long)timerid);

		clock_gettime(clkid, &prev_ts);

		/* Start the interval timer to 1ms */
		if(strcmp(argv[2], "s") == 0){
			its.it_value.tv_sec = s;
			its.it_value.tv_nsec = 0;
		}
		else if(strcmp(argv[2], "ms") == 0){
			its.it_value.tv_sec = 0;
			its.it_value.tv_nsec = MSEC2NSEC(s);
		}
		else if(strcmp(argv[2], "us") == 0){
			its.it_value.tv_sec = 0;
			its.it_value.tv_nsec = USEC2NSEC(s);
		}
		else{
			perror("argument error\n");
			return -4;
		}
		its.it_interval = its.it_value;

		if (timer_settime(timerid, 0, &its, NULL) == -1) {
			perror("timer_settime");
			return -3;
		}

		/* Sleep a while */
		if(strcmp(argv[2], "s") == 0){
			printf("Sleeping 10 times for %ds\n", s);
		}
		else if(strcmp(argv[2], "ms") == 0){
			printf("Sleeping 10 times for %dms\n", s);
		}
		else if(strcmp(argv[2], "us") == 0){
			printf("Sleeping 10 times for %dus\n", s);
		}

		for (i=0; i < 10; i++)
			sleep(1);

		timer_delete(timerid);
	}
	else{
		printf("argument error!\n");
	}

    return 0;
}
