//TODO : 전역변수 -> 동적할당으로 변경
//		 백그라운드 에러 실행에 대한 Done이 되지 않도록 처리


#include "jobs.h"

void err_job(){
	// If current job fails &  free dynamic allocated cl
	if(job_len> 0 && job_table[--job_len].cl != NULL)
		free(job_table[job_len].cl);
}

void set_job_cl(const char* cl){
	
	// check that command is backgound run
	if(cl[strlen(cl)-2] == 38){	// 38 == &
		job_table[job_len].status = -1;
		job_table[job_len].cl = calloc(strlen(cl)+1,sizeof(char));
		strcpy(job_table[job_len].cl,cl);
	}
}

void set_job_pid(pid_t pid){
	// set current job of pid
	job_table[job_len].pid = pid;
	job_len++;
}

void set_job_status(pid_t pid, int status){
	int i;

	// change the status of the same pid
	for(i=0;i<job_len;i++){
		if(job_table[i].pid == pid){
			job_table[i].status = status;
		}
	}
}

void show_jobs(){
	int i;
	for(i=0;i<job_len;i++){
		if(job_table[i].status != -1 ){
			printf("[%d] ", i+1);
			switch(job_table[i].status){
				case 0: 								// 0: Done
					printf("Done\t\t"); 
					job_table[i].status = -1;			// set program status exit
					free(job_table[i].cl);				// free exit progam cl
					break;
				case 1: printf("Running\t\t"); break;	// 1: Running
			}
			printf("%s", job_table[i].cl);
		}
	}
}