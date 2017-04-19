#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 

#define ENTER 10
#define TAB 9
#define SPACE 32
#define QUIT_Q 81
#define QUIT_q 113

// implementation getch
int getch(void);

// get terminal width, height
void get_size(int*,int*);

// get string width
int displayed_width(const char*);

// prints the specified number of lines.
int show(FILE*,int,int);

// run
void more(char*);