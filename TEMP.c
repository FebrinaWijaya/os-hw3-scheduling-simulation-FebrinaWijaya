#include "scheduling_simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

static ucontext_t uctx_main, uctx_func1;

#define DEBUG 1

void error(char *msg)
{
	perror(msg); 
	exit(EXIT_FAILURE);
}

void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    return 0;
}

int hw_task_create(char *task_name)
{

    return 0; // the pid of created task name
}

void test()
{
	printf("test started\n");
	// if(DEBUG)
 //    	printf("test: swapcontext(&uctx_func1, &uctx_main)\n");
	// if (swapcontext(&uctx_func1, &uctx_main) == -1)
 //        error("swapcontext");
    printf("test finished\n");
}

volatile int sigcount=0;
#define LOOP_LIMIT  1E12

void catcher( int sig ) {

    struct itimerval value;
    if(sig == SIGALRM)
    {
    	printf( "Catched signal SIGALRM\n");
    }
    else if(sig == SIGTSTP)
    {
    	printf( "Catched signal SIGSTP\n");
    }
    sigcount++;

    //if( sigcount > 1 ) {

        /*
         * Disable the real time interval timer
         */

        getitimer( ITIMER_REAL, &value );
        if(DEBUG)
        {
        	printf( "value.tv_sec = %d\n", value.it_value.tv_sec);
        	printf( "value.tv_usec = %d\n", value.it_value.tv_usec);
        }
        
        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = 0;

        setitimer( ITIMER_REAL, &value, NULL );
    //}
}

int main()
{
	char func1_stack[16384];

    if (getcontext(&uctx_func1) == -1)
        error("getcontext");
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    uctx_func1.uc_link = &uctx_main;
    makecontext(&uctx_func1, test, 0);
    if(DEBUG)
    	printf("main: swapcontext(&uctx_main, &uctx_func1)\n");
    if (swapcontext(&uctx_main, &uctx_func1) == -1)
         error("swapcontext");
    // if(DEBUG)
    // 	printf("main: swapcontext(&uctx_main, &uctx_func1)\n");
    // if (swapcontext(&uctx_main, &uctx_func1) == -1)
    //     error("swapcontext");

    //for timer
    int result = 0;
    printf("Before setting TIMER\n");
    struct itimerval value, ovalue;
    int which = ITIMER_REAL;

    struct sigaction sact;
    volatile double count;
    time_t t;

    sigemptyset( &sact.sa_mask );
    sact.sa_flags = 0;
    sact.sa_handler = catcher;
    sigaction( SIGALRM, &sact, NULL );
    sigaction(SIGTSTP, &sact, NULL );

    /*
     * Set a real time interval timer to repeat every 200 milliseconds
     */

    value.it_interval.tv_sec = 0;        /* Zero seconds */
    value.it_interval.tv_usec = 0;  /* Two hundred milliseconds */
    value.it_value.tv_sec = 0;           /* Zero seconds */
    value.it_value.tv_usec = 500000;     /* Five hundred milliseconds */


    result = setitimer( ITIMER_REAL, &value, &ovalue );
    printf("After setting TIMER\n");

    while(sigcount < 2);

    if(DEBUG)
    	printf("main: exiting\n");
    //exit(EXIT_SUCCESS);
	return 0;
}
