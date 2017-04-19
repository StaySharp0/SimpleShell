#include "util.h"

int main(int argc, char** argv){

	// init setting
	SHELL* ssh = (SHELL *)malloc(sizeof(SHELL));
	init_shell(ssh,argv);

	// run command loop
	switch(argc){
		case 1:
			run_mainloop(ssh);
			break;
		default:
			for(int i = 1; i < argc; i++){
				run_batchloop(ssh,argv[i]);	
			}
	}

	// cleanup
	destroy_shell(ssh);

	return EXIT_SUCCESS;
}