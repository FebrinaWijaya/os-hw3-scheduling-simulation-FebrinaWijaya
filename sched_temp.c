#include "scheduling_simulator.h"

Task_node* task_queue_head; //for all tasks
Priority_queue_node *high_queue_head, *high_queue_tail; //high priority tasks
Priority_queue_node *low_queue_head, *low_queue_tail; //low priority tasks
Task_node* current; //current active task

int PID = 1;
int saved_timer_usec = 0;
int catcher_count = 0;
bool task_executing = false;
bool shell_active = true; //remove later

static ucontext_t uctx_shell_mode;
static ucontext_t uctx_simulation;
static ucontext_t uctx_saved;

#define DEBUG 1

void hw_suspend(int msec_10)
{
	//setTimer(0);
	current->task_state = TASK_WAITING;
	current->suspend_time = msec_10*10;
	Task_node* temp = current;
	current = NULL;
	swapcontext(&temp->uctx_task, &uctx_simulation);
	return;
}

void hw_wakeup_pid(int pid)
{
	Task_node* temp = task_queue_head;
	if(temp == NULL)
		return;
	else
	{
		while(temp!=NULL)
		{
			if(temp->pid == pid  && temp->task_state == TASK_WAITING)
			{
				temp->task_state = TASK_READY;
				temp->queue_time = 0;
				break;
			}
			temp = temp->next;
		}
	}
	return;
}

int hw_wakeup_taskname(char *task_name)
{
	int count = 0;
	Task_node* temp = task_queue_head;
	if(temp == NULL)
		return 0;
	else
	{
		while(temp!=NULL)
		{
			if(strcmp(temp->task_name, task_name) == 0 && temp->task_state == TASK_WAITING)
			{
				temp->task_state = TASK_READY;
				temp->queue_time = 0;
				count++;
			}
			temp = temp->next;
		}
	}
    return count;
}

void error(char *msg)
{
	perror(msg); 
	exit(EXIT_FAILURE);
}

int hw_task_create(char *task_name)
{
	//initialize node
	Task_node* node = (Task_node *) malloc(sizeof(Task_node));
	node->pid = PID;
	node->task_name = malloc(sizeof(char)*6);
	strcpy(node->task_name, task_name);
	node->task_state = TASK_READY;
	node->queue_time = 0;
	node->suspend_time = 0;
	node->priority = 'L';
	node->time_quantum = 'S';
	node->next = NULL;

	char* stack = (char*)malloc(STACK_SIZE);
	//char stack[STACK_SIZE];

    if (getcontext(&(node->uctx_task)) == -1)
        error("getcontext");
    (node->uctx_task).uc_stack.ss_sp = stack;
    (node->uctx_task).uc_stack.ss_size = STACK_SIZE;
    (node->uctx_task).uc_stack.ss_flags = 0;
    (node->uctx_task).uc_link = &uctx_simulation;

	if(strcmp(task_name,"task1") == 0)
	{
	    makecontext(&(node->uctx_task), task1, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else if(strcmp(task_name,"task2") == 0)
	{
	    makecontext(&(node->uctx_task), task2, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else if(strcmp(task_name,"task3") == 0)
	{
	    makecontext(&(node->uctx_task), task3, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else if(strcmp(task_name,"task4") == 0)
	{
	    makecontext(&(node->uctx_task), task4, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else if(strcmp(task_name,"task5") == 0)
	{
	    makecontext(&(node->uctx_task), task5, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else if(strcmp(task_name,"task6") == 0)
	{
	    makecontext(&(node->uctx_task), task6, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	if(strcmp(task_name,"test") == 0)
	{
	    makecontext(&(node->uctx_task), test, 0);
	    insert_task_q(node);
	    return PID++; // the pid of created task name
	}
	else 
		return -1;
    
}
void insert_task_q(Task_node* node)
{
	if(task_queue_head == NULL) task_queue_head = node;
	else
	{
		Task_node *temp = task_queue_head;
		while(temp->next!=NULL)
			temp = temp->next;
		temp->next = node;
	}
	
	insert_priority_q('L',node);
}
int remove_task_q(int pid)
{
	if(task_queue_head == NULL) return -1;
	Task_node *temp = task_queue_head;
	Task_node *prev;
	while(temp!=NULL)
	{
		if(temp->pid == pid)
			break;
		prev = temp;
		temp = temp -> next;
	}
	if(temp == NULL) return -1;
	if(temp == task_queue_head)
	{
		task_queue_head = temp -> next;
	}
	else
	{
		prev->next = temp->next;
	}
	if(temp == current) //removed task is current executing task
		current = NULL;
	remove_priority_q(temp->priority, temp);
	free(temp);
	return 0;
}
void insert_priority_q(char priority, Task_node *task_node)
{
	Priority_queue_node *node = (Priority_queue_node *) malloc(sizeof(Priority_queue_node));
	node->task = task_node;
	node->next = NULL;
	if(priority == 'L')
	{
		if(low_queue_head == NULL)
		{
			low_queue_head = node;
			low_queue_tail = low_queue_head;
			low_queue_tail->next = low_queue_head;
			return;
		}
		else
		{
			low_queue_tail->next = node;
			low_queue_tail = low_queue_tail->next;
			low_queue_tail->next = low_queue_head;
		}
	}
	else
	{
		if(high_queue_head == NULL)
		{
			high_queue_head = node;
			high_queue_tail = high_queue_head;
			high_queue_tail->next = high_queue_head;
			return;
		}
		else
		{
			high_queue_tail->next = node;
			high_queue_tail = high_queue_tail->next;
			high_queue_tail->next = high_queue_head;
		}
	}
	return;
}
int remove_priority_q(char priority, Task_node *task)
{
	Priority_queue_node* head;
	Priority_queue_node* temp;
	Priority_queue_node* prev;
	if(priority == 'L')
	{
		head = low_queue_head;
		temp = head;
		if(head!=NULL && head->task == task)
		{
			if(low_queue_head == low_queue_tail)
				low_queue_head = low_queue_tail = NULL;
			else
			{
				low_queue_head = low_queue_head->next;
				low_queue_tail->next = low_queue_head;
			}
			free(head);
			return 0;
		}
	}
	else
	{
		head = high_queue_head;
		temp = head;
		if(head!=NULL && head->task == task)
		{
			if(high_queue_head == high_queue_tail)
			{
				high_queue_head = high_queue_tail = NULL;
			}
			else
			{
				high_queue_head = high_queue_head->next;
				high_queue_tail->next = high_queue_head;
			}
			free(head);
			return 0;
		}
	}
	prev = temp;
	temp = temp->next;
	while(temp!=head)
	{
		if(temp->task == task)
			break;
		prev = temp;
		temp = temp->next;
	}
	if(temp == head) return -1;
	else
	{
		prev->next = temp->next;
		if(temp == high_queue_tail) high_queue_tail = prev;
		else if(temp == low_queue_tail) low_queue_tail = prev;
	}
	free(temp);
	return 0;
}
int set_task_priority(int pid, char priority)
{
	if(task_queue_head == NULL) return -1;
	Task_node *temp = task_queue_head;
	while(temp!=NULL)
	{
		if(temp->pid == pid)
			break;
		temp = temp -> next;
	}
	if(temp == NULL) return -1;
	else if(temp->priority != priority)
	{
		remove_priority_q(temp->priority, temp);
		temp->priority = priority;
		insert_priority_q(temp->priority, temp);
	}
	return 0;
}
int set_task_tquantum(int pid, char tquantum)
{
	if(task_queue_head == NULL) return -1;
	Task_node *temp = task_queue_head;
	while(temp!=NULL)
	{
		if(temp->pid == pid)
			break;
		temp = temp -> next;
	}
	if(temp == NULL) return -1;
	temp->time_quantum = tquantum;
	return 0;
}
void print_tasks()
{
		Task_node *temp = task_queue_head;
		while(temp!=NULL)
		{
			//print format: task1 TASK_READY 50 H L
			printf("%d %s\t",temp->pid, temp->task_name);
			if(temp->task_state == TASK_RUNNING)
				printf("TASK_RUNNING\t");
			else if(temp->task_state == TASK_READY)
				printf("TASK_READY\t");
			else if(temp->task_state == TASK_WAITING)
				printf("TASK_WAITING\t");
			else if(temp->task_state == TASK_TERMINATED)
				printf("TASK_TERMINATED\t");
			printf("%d\t%c\t%c\n", temp->queue_time, temp->priority, temp->time_quantum);
			temp = temp->next;
		}
}

void print_priority_q()
{
		printf("\nHigh priority tasks:\n");
		Priority_queue_node *temp = high_queue_head;
		//print format: task1 TASK_READY 50 H L
		if(temp!=NULL)
		{
			printf("%d %s\t",temp->task->pid, temp->task->task_name);
			if(temp->task->task_state == TASK_RUNNING)
				printf("TASK_RUNNING\t");
			else if(temp->task->task_state == TASK_READY)
				printf("TASK_READY\t");
			else if(temp->task->task_state == TASK_WAITING)
				printf("TASK_WAITING\t");
			else if(temp->task->task_state == TASK_TERMINATED)
				printf("TASK_TERMINATED\t");
			printf("%d\t%c\t%c\n", temp->task->queue_time, temp->task->priority, temp->task->time_quantum);
			temp = temp->next;
		}
		while(temp!=high_queue_head)
		{
			//print format: task1 TASK_READY 50 H L
			printf("%d %s\t",temp->task->pid, temp->task->task_name);
			if(temp->task->task_state == TASK_RUNNING)
				printf("TASK_RUNNING\t");
			else if(temp->task->task_state == TASK_READY)
				printf("TASK_READY\t");
			else if(temp->task->task_state == TASK_WAITING)
				printf("TASK_WAITING\t");
			else if(temp->task->task_state == TASK_TERMINATED)
				printf("TASK_TERMINATED\t");
			printf("%d\t%c\t%c\n", temp->task->queue_time, temp->task->priority, temp->task->time_quantum);
			temp = temp->next;	
		}

		printf("\nLow priority tasks:\n");
		temp = low_queue_head;
		if(temp!=NULL)
		{
			printf("%d %s\t",temp->task->pid, temp->task->task_name);
			if(temp->task->task_state == TASK_RUNNING)
				printf("TASK_RUNNING\t");
			else if(temp->task->task_state == TASK_READY)
				printf("TASK_READY\t");
			else if(temp->task->task_state == TASK_WAITING)
				printf("TASK_WAITING\t");
			else if(temp->task->task_state == TASK_TERMINATED)
				printf("TASK_TERMINATED\t");
			printf("%d\t%c\t%c\n", temp->task->queue_time, temp->task->priority, temp->task->time_quantum);
			temp = temp->next;
		}
		while(temp!=low_queue_head)
		{
			//print format: task1 TASK_READY 50 H L
			printf("%d %s\t",temp->task->pid, temp->task->task_name);
			if(temp->task->task_state == TASK_RUNNING)
				printf("TASK_RUNNING\t");
			else if(temp->task->task_state == TASK_READY)
				printf("TASK_READY\t");
			else if(temp->task->task_state == TASK_WAITING)
				printf("TASK_WAITING\t");
			else if(temp->task->task_state == TASK_TERMINATED)
				printf("TASK_TERMINATED\t");
			printf("%d\t%c\t%c\n", temp->task->queue_time, temp->task->priority, temp->task->time_quantum);
			temp = temp->next;
		}
}

void shell()
{
	char cmd[100];
	shell_active = true;
	while(printf("$ "))
	{
		scanf("%s",cmd);
		if(strcmp(cmd, "add") == 0)
		{
			// add TASK_NAME -t TIME_QUANTUM –p PRIORITY
			char *task_name;
			char opt[3];
			char arg[2];
			char tmp[100];
			char time_quantum = 'S'; 
			char priority = 'L';
			fgets(tmp,sizeof(tmp),stdin);
			task_name = strtok(tmp, " \n");
			int pid = hw_task_create(task_name);

			memset(opt,0,3);
			memset(arg,0,2);
			char *token;
			token = strtok(NULL, " \n");
			while(token!=NULL)
			{
				if(strcmp(token,"-t") == 0) {
					token = strtok(NULL, " \n");
					if(strcmp(token,"L") == 0)    time_quantum = 'L';
					set_task_tquantum(pid, time_quantum);
				}
				else if(strcmp(token,"-p") == 0) {
					token = strtok(NULL, " \n");
					if(strcmp(token,"H") == 0)    priority = 'H';
					set_task_priority(pid, priority);
				}
				token = strtok(NULL, " ");
			}
		}
		else if(strcmp(cmd, "ps") == 0)
		{
			print_tasks();
			if(DEBUG)
				print_priority_q();
		}
		else if(strcmp(cmd, "remove") == 0)
		{
			int pid;
			scanf("%d",&pid);
			int res = remove_task_q(pid);
			if(res<0)
				printf("pid to remove not found\n");
		}
		else if(strcmp(cmd, "start") == 0)
		{
			if(current == NULL) //first time start or running task is removed
			{
				shell_active = false;
				printf("simulating...\n");
				setTimer(saved_timer_usec); /* Restore interval timer */
				swapcontext(&uctx_shell_mode, &uctx_simulation);
				shell_active = true;
				if(DEBUG) printf("shell: Back to shell mode\n");
			}
			else
			{
				if(DEBUG) printf("shell: Returning from shell to task\n");
				setTimer(saved_timer_usec); /* Restore interval timer */
				shell_active = false;
				printf("simulating...\n");
		    	swapcontext(&uctx_shell_mode, &uctx_saved);
		    	shell_active = true;
			}
		}
	}	
}
void catcher( int sig ) {
    if(sig == SIGALRM)
    {
    	//printf( "Catched signal SIGALRM\n");
    	updateQueueTime(10);
    	
    	if(current!=NULL)
    	{
    		if(current->time_quantum == 'L')
    			catcher_count = (catcher_count+1)%2;
    		if(current->time_quantum == 'S' ||
    			(current->time_quantum == 'L' && catcher_count==0)) //count==0 -> 20 msec
    		{
    			Task_node* prev = current;
		    	reschedule();
		    	if(DEBUG)
			    	printf("catcher: swap to task %s(%d)\n", current->task_name, current->pid);
		    	swapcontext(&(prev->uctx_task), &(current->uctx_task));
    		}
    	}   	
    }
    else if(sig == SIGTSTP)
    {
    	if(!shell_active)
    	{
	    	printf( "Catched signal SIGSTP\n");
	    	struct itimerval value;
	    	getitimer(ITIMER_REAL, &value);
	    	saved_timer_usec = value.it_value.tv_usec;
	    	setTimer(0); // disable timer
	    	if(DEBUG) printf("catcher: Switch to shell mode\n");
	    	swapcontext(&uctx_saved, &uctx_shell_mode);
    	}
    }
    // else if(sig == SIGSEGV)
    // {
    // 	if(DEBUG) printf("catcher: segmentation fault\n");
    // 	setcontext(&uctx_simulation);
    // }
}
void test()
{
	printf("test task_executing\n");
	printf("test finished\n");
}
Task_node* addtest()
{
    hw_task_create("test");
    return task_queue_head;
}
void simulate()
{
	//Set a real time interval timer to send SIGALRM signal
	setTimer(10000);
	
	while(1)
    {
    	current = NULL;
    	reschedule(); //will modify Task_node *current and set timer
		while(current == NULL)
		{
			//printf("No task to execute\n");
			//swapcontext(&uctx_simulation, &uctx_shell_mode);
			reschedule();
		}
		task_executing = true;
		catcher_count = 0;
		if(DEBUG)
	    	printf("simulate: swap to task %s(%d)\n", current->task_name, current->pid);
		if (swapcontext(&uctx_simulation, &(current->uctx_task)) == -1)
	        error("swapcontext");
	    if(DEBUG)
	    	printf("simulate: returned from first task\n");

     	//handle task termination
     	while(task_executing == true)
     	{
     		//setTimer(0); //disable timer
	     	if(DEBUG) printf("control back to simulate()\n");
	     	if(current!=NULL) //running task is not removed; running task terminated
	     		current->task_state = TASK_TERMINATED;
	     	current = NULL;
	     	reschedule();
	     	if(DEBUG) printf("rescheduled\n");
	     	if(current == NULL)
			{
				printf("All tasks finished executing\n");
				task_executing = false;
				//swapcontext(&uctx_simulation, &uctx_shell_mode);
				break;
			}
	     	if(DEBUG)
	     	{
		    	printf("simulate: swap to task %s(%d)\n", current->task_name, current->pid);
	     	}
	     	catcher_count = 0;
			if(swapcontext(&uctx_simulation, &(current->uctx_task)) == -1)
			{
		        error("swapcontext");
			}	
     	}	     	
     }
}
void reschedule()
{
	if(current != NULL)
	{
		current->task_state = TASK_READY;
	}
	current = NULL;
	if(high_queue_head!=NULL)
	{
		Priority_queue_node *temp = high_queue_head;
		if(temp->task->task_state == TASK_READY)
		{
				current = temp->task;
				high_queue_head = temp->next;
				high_queue_tail = temp;
		}
		else
		{
			temp = temp->next;
			while(temp!=high_queue_head  && temp->task->task_state != TASK_READY)
				temp = temp->next;
			if(temp!=high_queue_head)
			{
				current = temp->task;
				high_queue_head = temp->next;
				high_queue_tail = temp;
			}
		}
	}
	if(current==NULL && low_queue_head!=NULL)
	{
		Priority_queue_node *temp = low_queue_head;
		if(temp->task->task_state == TASK_READY)
		{
				current = temp->task;
				low_queue_head = temp->next;
				low_queue_tail = temp;
		}
		else
		{
			temp = temp->next;
			while(temp!=low_queue_head && temp->task->task_state != TASK_READY)
				temp = temp->next;
			if(temp!=low_queue_head)
			{
				current = temp->task;
				low_queue_head = temp->next;
				low_queue_tail = temp;
			}
		}
	}
	if(current != NULL)
	{
		current->task_state = TASK_RUNNING;
	}
}

void setTimer(int time_usec)
{
	struct itimerval value, ovalue;

    value.it_interval.tv_sec = 0;        /* Zero seconds */
    value.it_interval.tv_usec = 10000;  /* Ten milliseconds */
    value.it_value.tv_sec = 0;           /* Zero seconds */
    value.it_value.tv_usec = time_usec;     /* Twenty milliseconds */

    setitimer( ITIMER_REAL, &value, &ovalue );
}

void updateQueueTime(int time)
{
	//printf("update task time\n");
	Task_node* temp = task_queue_head;
	if(temp == NULL)
		return;
	else
	{
		while(temp!=NULL)
		{
			if(temp->task_state == TASK_READY)
				temp->queue_time += time;
			else if(temp->task_state == TASK_WAITING)
			{
				temp->suspend_time -= time;
				if(temp->suspend_time <= 0)
					temp->task_state = TASK_READY;
			}
			temp = temp->next;
		}
	}
}
int main()
{
	char* stack = (char*)malloc(STACK_SIZE);

    if (getcontext(&uctx_simulation) == -1)
        error("getcontext");
    uctx_simulation.uc_stack.ss_sp = stack;
    uctx_simulation.uc_stack.ss_size = STACK_SIZE;
    uctx_simulation.uc_stack.ss_flags = 0;
    uctx_simulation.uc_link = NULL;
    makecontext(&uctx_simulation, simulate, 0);

    //setup for signal handler
	signal(SIGTSTP, catcher);
	signal(SIGALRM, catcher);
	//signal(SIGSEGV, catcher);

	shell();
	return 0;
}
