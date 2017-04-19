//TODO : 탭에 대한 화면 스페이스 공간이 정확하게 잡히지 않는 문제가 발생


#include "more.h"

// implementation getch
int getch(void){
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );

    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );

    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    
    return ch;
}

// get terminal width, height
void get_size(int* row,int* col){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *row = w.ws_row;
    *col = w.ws_col;
}

// get string width
int displayed_width(const char* line){
	int i,count = 0;

	for(i = 0; line[i] != 0; i++){
		if(line[i] == TAB) count += 7;

		// printf("%c:%d\n",line[count],line[count]);
		count++;
	}

	return count;
}

// prints the specified number of lines.
int show(FILE* file,int row, int  col){
	int i,status;
	char* cl = NULL;
	size_t buf_size = 0;

	for(i = 0; i < row && (status = getline(&cl, &buf_size, file) != -1); i++){
		printf("%s", cl);
		if( col < displayed_width(cl) ) i++; 
	}
	free(cl);

	return status;

}
void more(char* filepath){
	FILE *file = fopen(filepath,"r");
	int row,col,status = 1;

	// show first page
	get_size(&row,&col);
	status = show(file,row-1,col);

	while( status != 0){
		switch(getch()){
			case ENTER:								// print 1 line 
				get_size(&row,&col);
				status = show(file,1,col);
				break;
			case SPACE:								// print 1 page
				get_size(&row,&col);
				status = show(file,row-1,col);
				break;
			case QUIT_q:							// exit
			case QUIT_Q: 
				status = 0; 
				break;
		}
	} 

	fclose(file);
}