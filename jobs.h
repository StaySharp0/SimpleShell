#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <string.h>

typedef struct job{
	pid_t pid;
	int status; // 0: Done 1:Running -1: default
	char* cl;
} JOB;

static int job_len = 0;
static JOB job_table[1024];

void err_job();
void set_job_cl(const char*);
void set_job_pid(pid_t);
void set_job_status(pid_t,int);
void show_jobs(void);