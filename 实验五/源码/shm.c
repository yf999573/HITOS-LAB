#include <asm/segment.h>
#include <linux/kernel.h>
#include <string.h>
#include <linux/sched.h>

#define SHM_COUNT 32
#define SHM_NAME_SIZE 32

struct shm_table_struct
{
	int occupied;
	char name[SHM_NAME_SIZE];
	long address;
} shm_tables[SHM_COUNT];

int find_shm_index(char* name)
{
	int shm_index;
	for (shm_index = 0; shm_index < SHM_COUNT; shm_index++)
	{
		if (!strcmp(name, shm_tables[shm_index].name) && shm_tables[shm_index].occupied == 1)
		{
			return shm_index;
		}
	}
	return -1;
}

int sys_shmget(char* name)
{
	int index, shm_index;
	char temp_name[SHM_NAME_SIZE];
	for (index = 0; index < SHM_NAME_SIZE; index++)
	{
		temp_name[index] = get_fs_byte(name + index);
		if (temp_name[index] == '\0') break;
	}
	shm_index = find_shm_index(temp_name);
	if (shm_index != -1) return shm_index;
	for (index = 0; index < SHM_COUNT; index++)
	{
		if (shm_tables[index].occupied == 0)
		{
			strcpy(shm_tables[index].name, temp_name);
			shm_tables[index].occupied = 1;
			shm_tables[index].address = get_free_page();
			return index;
		}
	}
	printk("SHM Size Limited!\n");
	return -1;
}

void* sys_shmat(int shm_index)
{
	if (shm_tables[shm_index].occupied != 1)
	{
		printk("SHM Not Exist!\n");
		return (void*) -1;
	}
	put_page(shm_tables[shm_index].address, current->brk + current->start_code);
	return (void*)current->brk;
}