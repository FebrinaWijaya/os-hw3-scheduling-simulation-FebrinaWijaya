#include "task.h"
#define DEBUG 0

void task1(void)   // may terminated
{
	if(DEBUG)
    	printf("task1: started\n");
	unsigned int a = ~0;
	//unsigned int a = 10;

	while (a != 0) {
		a -= 1;
	}
	if(DEBUG)
    	printf("task1: exiting\n");

}

void task2(void) // run infinite
{
	if(DEBUG)
    	printf("task2: started(will run infinitely)\n");
	unsigned int a = 0;

	while (1) {
		a = a + 1;
	}
}

void task3(void) // wait infinite
{
	if(DEBUG)
	{
    	printf("task3: started(will wait infinitely)\n");
    	printf("task3: suspended myself for 32768*10msec\n");
	}

	hw_suspend(32768); //32768
	fprintf(stdout, "task3: good morning~\n");
	fflush(stdout);
	if(DEBUG)
    	printf("task3: exiting\n");
}

void task4(void) // sleep 5s
{
	if(DEBUG)
    	printf("task4: started(sleep 5s)\n");
	hw_suspend(500);
	fprintf(stdout, "task4: good morning~\n");
	fflush(stdout);
	if(DEBUG)
    	printf("task4: exiting\n");
}

void task5(void)
{
	if(DEBUG)
	{
    	printf("task5: started\n");
    	printf("task5: create task3\n");
    }
	int pid = hw_task_create("task3");

	if(DEBUG)
    	printf("task5: suspend myself for 1000*10msec\n");
	hw_suspend(1000);
	fprintf(stdout, "task5: good morning~\n");
	fflush(stdout);

	hw_wakeup_pid(pid);
	fprintf(stdout, "Mom(task5): wake up pid %d~ (task3)\n", pid);
	fflush(stdout);
	if(DEBUG)
    	printf("task5: exiting\n");
}

void task6(void)
{
	if(DEBUG)
	{
    	printf("task6: started\n");
    	printf("task6: create 5 task3\n");
	}
	for (int num = 0; num < 5; ++num) {
		hw_task_create("task3");
	}
	if(DEBUG)
    	printf("task6: suspend myself for 1000*10msec\n");
	hw_suspend(1000);
	fprintf(stdout, "task6: good morning~\n");
	fflush(stdout);

	if(DEBUG)
    	printf("task6: wakeup all task3\n");
	int num_wake_up = hw_wakeup_taskname("task3");
	fprintf(stdout, "Mom(task6): wake up task3=%d~\n", num_wake_up);
	fflush(stdout);
	if(DEBUG)
    	printf("task6: exiting\n");
}
