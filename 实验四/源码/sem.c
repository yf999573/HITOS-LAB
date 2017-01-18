#define __LIBRARY__  
#include <unistd.h>  
#include <linux/sched.h>  
#include <linux/kernel.h>  
#include <asm/segment.h>  
#include <asm/system.h>

#define SEM_COUNT 32
sem_t semaphores[SEM_COUNT];

void init_queue(sem_queue* queue)
{
	queue->front = queue->end = 0;
}

int is_empty(sem_queue* queue)
{
	return queue->front == queue->end ? 1 : 0;
}

int is_full(sem_queue* queue)
{
	return (queue->end + 1) % QUEUE_LEN == queue->front ? 1 : 0;
}

struct task_struct * get_task(sem_queue* queue)
{
	if (is_empty(queue))
	{
		printk("Queue is empty!\n");
		return NULL;
	}
	struct task_struct *current = queue->wait_tasks[queue->front];
	queue->front = (queue->front + 1) % QUEUE_LEN;
	return current;
}

int insert_task(sem_queue* queue, struct task_struct *task)
{
	if (is_full(queue))
	{
		printk("Queue is full!\n");
		return -1;
	}
	queue->wait_tasks[queue->end] = task;
	queue->end = (queue->end + 1) % QUEUE_LEN;
	return 1;
}

int sem_location(const char* name)
{
	int index;
	for (index = 0; index < SEM_COUNT; index++)
	{
		if (strcmp(name, semaphores[index].name) == 0 && semaphores[index].occupied == 1)
		{
			return index;
		}
	}
	return -1;
}

sem_t* sys_sem_open(const char* name, unsigned int value)
{
	char temp_name[16];
	char temp_char = 0;
	int index;
	for (index = 0; index < 16; index++)
	{
		temp_char = get_fs_byte(name + index);
		temp_name[index] = temp_char;
		if (temp_char == '\0') break;
	}
	if (temp_char >= 16)
	{
		printk("Semaphore name is too long!");
		return NULL;
	}
	if ((index = sem_location(temp_name)) != -1)
	{
		return &semaphores[index];
	}
	for (index = 0; index < SEM_COUNT; index++)
	{
		if (!semaphores[index].occupied)
		{
			strcpy(semaphores[index].name, temp_name);
			semaphores[index].occupied = 1;
			semaphores[index].value = value;
			init_queue(&semaphores[index].wait_queue);
			return &semaphores[index];
		}
	}
	printk("Numbers of semaphores are limited!\n");
	return NULL;
}

int sys_sem_wait(sem_t* sem)
{
	cli();
	sem->value--;
	if (sem->value < 0)
	{
		current->state = TASK_UNINTERRUPTIBLE;
		insert_task(&sem->wait_queue, current);
		schedule();
	}
	sti();
	return 0;
}

int sys_sem_post(sem_t* sem)
{
	cli();
	struct task_struct *p;
	sem->value++;
	if (sem->value <= 0)
	{
		p = get_task(&sem->wait_queue);
		if (p != NULL)
		{
			(*p).state = TASK_RUNNING;
		}
	}
	sti();
	return 0;
}

int sys_sem_unlink(const char *name)
{
	char temp_name[16];
	char temp_char = 0;
	int index;
	for (index = 0; index<16; index++)
	{
		temp_char = get_fs_byte(name + index);
		temp_name[index] = temp_char;
		if (temp_char == '\0') break;
	}
	if (temp_char >= 16)
	{
		printk("Semphore name is too long!");
		return -1;
	}
	int current = sem_location(temp_name);
	if (current != -1)
	{
		semaphores[current].value = 0;
		strcpy(semaphores[current].name, "\0");
		semaphores[current].occupied = 0;
		return 0;
	}
	return -1;
}