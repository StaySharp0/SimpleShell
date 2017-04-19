#include "util.h"

/*
  Fn: simpleshell
*/
void init_shell(SHELL* ssh,char** argv){
	tmp = NULL;
	set_signal(ssh);
	set_env(ssh,argv);
	set_new_command(ssh);
	set_built_in(ssh);
}

void set_signal(SHELL* ssh){
	/*
	 * Set shell signal
	 */
	sigset_t set;

	sigfillset(&set); 						// Set all-signal
	sigdelset(&set,SIGCHLD); 				// Del (child exit) signal
	sigprocmask(SIG_SETMASK,&set,NULL); 	// Set (Ctrl-C) signal
    
	ssh->sign.sa_flags = SA_RESTART; 		// Set restart
	sigemptyset(&ssh->sign.sa_mask); 		// Dell all-signal
	ssh->sign.sa_handler = zombie_handler;	// Set Zombie Handling function
	sigaction(SIGCHLD, &ssh->sign, 0);		// Apply signal setting
}

void set_env(SHELL* ssh,char** argv){
	/*
	 * Set env variable(shell) 
	 */
	ssh->env_shell = calloc(4096,sizeof(char));	// for unix system path size 4096
	realpath(argv[0], ssh->env_shell);			// get real path
	setenv("shell",ssh->env_shell,1);			// set env shell = realpath

	//printf( "%s\n", getenv("shell"));
}

void set_new_command(SHELL* ssh){
	/*
	 * Set new-command function
	 */

	char* str_newCmd[] = { 			// (ADD-POINT)
		"clr",
		"environ",
		"dir",
		"pause",
		"proc"
	};	

	// Set new-command num
	ssh->num_newCmd = sizeof(str_newCmd)/sizeof(char*);


	// Set new-command name
	ssh->str_newCmd = calloc(ssh->num_newCmd,sizeof(char*));
	for(int i=0; i < ssh->num_newCmd; i++){
		ssh->str_newCmd[i] = calloc(strlen(str_newCmd[i])+1,sizeof(char));
		strcpy(ssh->str_newCmd[i],str_newCmd[i]);
	}

	// Set new-command (ADD-POINT)
	ssh->fn_newCmd = calloc(ssh->num_newCmd, sizeof(fn_cmd));
	ssh->fn_newCmd[0] = ssh_clr;
	ssh->fn_newCmd[1] = ssh_environ;
	ssh->fn_newCmd[2] = ssh_dir;
	ssh->fn_newCmd[3] = ssh_pause;
	ssh->fn_newCmd[4] = ssh_proc;

	// printf("%d\n", ssh->num_newCmd);
	// for(int i=0; i < ssh->num_newCmd; i++) printf("%s\n", ssh->str_newCmd[i]);
}

void set_built_in(SHELL* ssh){
	/*
	 * Set Built-in function
	 */

	char* str_builtIn[] = { 		// (ADD-POINT)
		"chdir", 
		"help", 
		"quit", 
		"cd", 
		"proc" 
	};

	// Set Built-in num
	ssh->num_builtIn = sizeof(str_builtIn)/sizeof(char*);

	// Set Built-in name
	ssh->str_builtIn = calloc(ssh->num_builtIn,sizeof(char*));
	for(int i=0; i < ssh->num_builtIn; i++){
		ssh->str_builtIn[i] = calloc(strlen(str_builtIn[i])+1,sizeof(char));
		strcpy(ssh->str_builtIn[i],str_builtIn[i]);
	}

	// Set Built-in (ADD-POINT)
	ssh->fn_builtIn = calloc(ssh->num_builtIn, sizeof(fn_cmd));
	ssh->fn_builtIn[0] = ssh_chdir;
	ssh->fn_builtIn[1] = ssh_help;
	ssh->fn_builtIn[2] = ssh_quit;
	ssh->fn_builtIn[3] = ssh_chdir;
	ssh->fn_builtIn[4] = ssh_proc;

	// printf("%d\n", ssh->num_builtIn);
	// for(int i=0; i < ssh->num_builtIn; i++) printf("%s\n", ssh->str_builtIn[i]);
}

void destroy_shell(SHELL* ssh){
	/*
	 * Free dynamic allocation variable
	 */
	free(ssh->env_shell);

	for(int i=0; i < ssh->num_builtIn; i++) {
		free(ssh->str_builtIn[i]);
	}
	free(ssh->str_builtIn);
	free(ssh->fn_builtIn);

	for(int i=0; i < ssh->num_newCmd; i++) {
		free(ssh->str_newCmd[i]);
	}
	free(ssh->str_newCmd);
	free(ssh->fn_newCmd);

	free(ssh);
}

void throw_error(char* err){
	/*
	 * Send error
	 */
	perror(err);
	exit(1);
}

void zombie_handler(){
    pid_t pid;
    int status;
    
    // When the child process exits, it executes
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0){

		// Change job status
		set_job_status(pid,0);
	}
}


/*
  Fn: for Command loop
*/
void run_mainloop(SHELL* ssh) {
	char* cl;
	char** cl_arr;
	int status = 1;

	do{
		printf("%s> ",getenv("PWD"));
		cl = read_Command(ssh);					
		set_job_cl(cl);								// set job command line
		cl_arr = split_Command(cl);
		status = launch_Command(ssh,cl,cl_arr);	

		free(cl);
		free(cl_arr);
		if(tmp != NULL){
			for(int i=0; i <= tmp_count; i++){
				free(tmp[i]);
			}	
			free(tmp);
		}
	} while( status ); 
}

void run_batchloop(SHELL* ssh,char* file_path){
	char* cl = NULL;
	size_t buf_size = 0;
	char** cl_arr;
	FILE *file = fopen(file_path,"r");

	while(getline(&cl, &buf_size, file) != -1) {
		cl_arr = split_Command(cl);
		launch_Command(ssh,cl,cl_arr);

		free(cl_arr);
		if(tmp != NULL){
			for(int i=0; i <= tmp_count; i++){
				free(tmp[i]);
			}	
			free(tmp);
		}
	}
	free(cl);
	fclose(file);
}

char* read_Command(SHELL* ssh) {
	char *cl = NULL;
	size_t buf_size = 0;

	if( getline(&cl, &buf_size, stdin) == -1){
		// EOF handling
		destroy_shell(ssh);
		printf("\n\n[exit simpleshell]\n\n");

		free(cl);
		exit(1);
	}

	return cl;
}

char** split_Command(char* cl){

	// split cl by use TOK_DELIMITER

	int 	buf_size = DEFAULT_TOK_SIZE,
			pos = 0;
	char** 	tokens = calloc(buf_size, sizeof(char*));
	char* 	token;
	
	if(!tokens) perror("");

	// init split
	token = strtok(cl, TOK_DELIMITER);

	for(; token != NULL; pos++){

		// Exceed arr's size -> reallocate arr
		if(pos >= buf_size){
			buf_size += DEFAULT_TOK_SIZE;
			tokens = (char**)realloc(tokens, buf_size * sizeof(char*));

			if(!tokens) perror("");
		}

		// store & split
		tokens[pos] = token;
		token = strtok(NULL, TOK_DELIMITER);

		//printf("%s\n",tokens[pos]);
	}

	tokens[++pos] = NULL;

  return tokens;
}

int launch_Command(SHELL* ssh,char* cl,char** cl_arr){
	if (cl_arr[0] == NULL) return 1;

	// Check built-in function
	for (int i=0; i < ssh->num_builtIn; i++) {
		if (strcmp(cl_arr[0], ssh->str_builtIn[i]) == 0)
			return (ssh->fn_builtIn[i])(cl,cl_arr);
	}

	// Check new-command function
	for (int i=0; i < ssh->num_newCmd; i++) {
		if (strcmp(cl_arr[0], ssh->str_newCmd[i]) == 0)
			return (ssh->fn_newCmd[i])(cl,cl_arr);
	}

   return execute_process(cl,cl_arr);
}

/*
  Fn: for excute process
*/

int execute_process(char* cl, char** cl_arr){
	pid_t child;
	int i, flag_bg=0;

	// get cl_len & check background excute;
	for(i = 0; cl_arr[i] != NULL; i++);
	if(strcmp(cl_arr[i-1],"&") == 0){
		cl_arr[i-1] = NULL;
		flag_bg = 1;
	}

	child = fork(); // Copy parent process
	
 	switch(child) {
 		case -1:
	 		perror("fork error");
 		case 0:	// Excute child proces
 			execute_child(cl,cl_arr,flag_bg);
 		default:
			if(flag_bg) {
				set_job_pid(child);
				set_job_status(child,1);			// Set job status:Done
				break;
			}
			waitpid(child, NULL, 0);				// Wait until the end of the child program
			tcsetpgrp(STDIN_FILENO, getpgid(0)); 	// Set foreground processing to shell.
			fflush(stdout);
 	}

	return 1;
}

void execute_child(char* cl,char** cl_arr,int flag_bg){
	int i;
	int pp[2];
	sigset_t set;
	pid_t pid = getpid();

	setpgid(0,0); // Set child-process grup pid

	if(!flag_bg)
		tcsetpgrp(STDIN_FILENO, pid); // Set foreground processing to child.

	sigfillset(&set); 					// Set (all) signal
	sigprocmask(SIG_UNBLOCK,&set,NULL);	// Del (Ctrl-C) signal

    // pre-work pipeline
    tmp = make_split_clarr(cl_arr,"|",&tmp_count);

	// printf("%d\n", count);
	// for(int t1=0; t1 <= count; t1++){
	// 	for(int t2= 0; new[t1][t2] != NULL; t2++)
	// 		printf("%s ", new[t1][t2]);
	// 	printf("\n");
	// }


    // create pipeline
    for(i=0; i < tmp_count; i++){
		pipe(pp);

		switch(fork()){
			case -1: 
				throw_error("fork err");
            case  0:
            	close(pp[0]);
                dup2(pp[1], STDOUT_FILENO);
                execute(tmp[i],flag_bg);
            default: 
            	close(pp[1]);
                dup2(pp[0], STDIN_FILENO);
		}
	}
	execute(tmp[i],flag_bg);
}

void execute(char** cl_arr,int flag_bg){
	// for(int i=0;cl_arr[i] !=NULL;i++) printf("%s\n", cl_arr[i]);

	// redirection
    parse_redirect(cl_arr);

    // Set env parent 
    setenv("parent",getenv("shell"),1);

    if ( execvp(cl_arr[0], cl_arr) == -1 && flag_bg){
    	err_job();
    }
	throw_error("execute error");
}

void parse_redirect(char** cl_arr) {
	int fd;

	for(int i = 0; cl_arr[i] != NULL; i++){ 
		
		if( strcmp(cl_arr[i],">") == 0){											//output redirection
			if( (fd = open(cl_arr[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
					throw_error("file open error");
			dup2(fd, STDOUT_FILENO);
		} 
		else if( strcmp(cl_arr[i],">>") == 0){										//output redirection(append)
			if( (fd = open(cl_arr[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0)
					throw_error("file open error");
			dup2(fd, STDOUT_FILENO);
		} 
		else if( strcmp(cl_arr[i],"<") == 0){										//input redirection
			if( (fd = open(cl_arr[i+1], O_RDONLY , 0644)) < 0)
					throw_error("file open error");
			dup2(fd, STDIN_FILENO);
		} else {
			continue;
		}

		close(fd);
		cl_arr[i] = NULL;
	}
}

char*** make_split_clarr(char** cl_arr, char* target,int* rtn_count){
	int buf_size = DEFAULT_TOK_SIZE;
	int count = 0, idx=0, pos=0;

	char*** new = calloc(buf_size,sizeof(char**));

	for(idx = 0; cl_arr[idx] != NULL; idx++){
		// Exceed arr's size -> reallocate arr
		if(count > buf_size){
			buf_size += DEFAULT_TOK_SIZE;
			new = (char***)realloc(new, buf_size * sizeof(char**));
		}

		if(strcmp(cl_arr[idx],target) == 0){
			new[count] = calloc(idx-pos+1,sizeof(char*));
			
			for(int j=pos; j < idx; j++) {
				new[count][j-pos] = cl_arr[j];
				// printf("%s(%d) ",new[count][j-pos],j-pos);
			}

			count++;
			pos = idx+1;
		} 
	}

	// Insert last "target" ~ NULL
	new[count] = calloc(idx-pos+1,sizeof(char*));

	for(int j=pos; j < idx; j++) {
		new[count][j-pos] = cl_arr[j];
		// printf("%s(%d) ",new[count][j-pos],j-pos);
	}

	if(count == 0){
		new = (char***)realloc(new, 1 * sizeof(char**));
		new[0] = cl_arr;
	}
	else if(count > 0) {
		new = (char***)realloc(new, count * sizeof(char**));
	}

	*rtn_count = count;

	return new;
}


/*
  Fn: builtin shell commands
*/
int ssh_quit(char* cl,char** cl_arr){

	return 0;
}

int ssh_chdir(char* cl,char** cl_arr){
	char path[DEFAULT_BUFFER_SIZE];

	if(cl_arr[1] == NULL){ 					// Directory non-exists
		printf("%s\n", getenv("PWD"));
	}
	else if (chdir(cl_arr[1]) == 0){ 		// Directory exists
		setenv("OLDPWD",getenv("PWD"),1);	// set env OLDPWD = current PWD
		getcwd(path,DEFAULT_BUFFER_SIZE);	// get real path
		
		setenv("PWD",path,1); 				// set env PWD = real path
	} else {
		perror("chdir");
	}

	return 1;
}
int ssh_help(char* cl,char** cl_arr){
	// Execute more program & open readme file
	more("readme");

	return 1;
}

/*
  Fn: new shell commands
*/
int ssh_clr(char* cl,char** cl_arr){
	// Execute clear(clr)
	strcpy(cl_arr[0],"clear");

	return execute_process(cl,cl_arr);
}
int ssh_environ(char* cl,char** cl_arr){
	// Execute env(environ)
	strcpy(cl_arr[0],"env");

	return execute_process(cl,cl_arr);
}
int ssh_dir(char* cl,char** cl_arr){
	// Execute ls(dir)
	strcpy(cl_arr[0],"ls");

	return execute_process(cl,cl_arr);
}
int ssh_pause(char* cl,char** cl_arr){
	// Execute read -p(pause)
	system("read -p \"Press enter to continue\"");

	return 1;
}
int ssh_proc(char* cl,char** cl_arr){
	// Execute jobs program, call show_jobs
	show_jobs();

	return 1;
}