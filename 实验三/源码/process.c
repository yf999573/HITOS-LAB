#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#define HZ	100
void cpuio_bound(int last, int cpu_time, int io_time);
struct tms start_time, current_time;
clock_t utime, stime;
int sleep_time = 0;
int main(int argc, char * argv[])
{
	pid_t n_proc[10];
	for(int i=0;i<10;i++)
	{
		n_proc[i] = fork();
		if(n_proc[i] == 0 )
		{
			cpuio_bound(20,2*i,20-2*i); 
			return 0; 
		}
	}
	for(int i=0;i<10;i++) printf("Child PID: %d\n",n_proc[i]);
	wait(&i); 
	return 0;
}
void cpuio_bound(int last, int cpu_time, int io_time)
{
	do
	{
		times(&start_time);
		while (((utime + stime) / HZ)  < cpu_time)
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		}
		last -= cpu_time;
		if (last <= 0 ) break;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
		sleep_time = 0;
	} while (last > 0);
	return;
}