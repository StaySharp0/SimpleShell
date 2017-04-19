#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include "more.h"
#include "jobs.h"

// MACRO variables
#define DEFAULT_BUFFER_SIZE 512
#define DEFAULT_TOK_SIZE 	64
#define TOK_DELIMITER 		" \t\r\n\a"

// command Function
typedef int (*fn_cmd)(char*,char **);

typedef struct simpleshell{
	// for signal haddling
	struct sigaction sign;
	
	// for envrion
	char* 	env_shell;

	// for built-in function
	int 	num_builtIn;
	char** 	str_builtIn;
	fn_cmd* fn_builtIn;

	// for new-command function
	int 	num_newCmd;
	char** 	str_newCmd;
	fn_cmd* fn_newCmd;
} SHELL;

static char*** tmp; 	// for make_split_clarr
static int tmp_count; 	// tmp index

/*
  Fn: simpleshell
*/
void init_shell(SHELL*,char**);
void set_signal(SHELL*);
void set_env(SHELL*,char**);
void set_new_command(SHELL*);
void set_built_in(SHELL*);
void destroy_shell(SHELL*);
void throw_error(char*);
void zombie_handler();
/*
  Fn: for Command loop
*/
void run_mainloop(SHELL*);
void run_batchloop(SHELL*,char*);
char* read_Command(SHELL*);
char** split_Command(char*);
int launch_Command(SHELL*,char*,char**);

int execute_process(char*,char**);
void execute_child(char*,char**,int);
void execute(char**,int);
void parse_redirect(char**);
char*** make_split_clarr(char**,char*,int*);


/*
  Fn: builtin shell commands
*/
int ssh_chdir(char*,char**);
int ssh_help(char*,char**);
int ssh_quit(char*,char**);


/*
  Fn: new shell commands
*/
int ssh_clr(char*,char**);
int ssh_environ(char*,char**);
int ssh_dir(char*,char**);
int ssh_pause(char*,char**);
int ssh_proc(char*,char**);