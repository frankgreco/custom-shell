#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

void execute(char* cmd, char** arr);

void parse_file(char** arr, char* cmd);

void do_parse_file(char** arr, char* cmd, char* file_name);

int main(int argc, const char* argv[]){

	char cwd[1024];
	char input[1024];
	char* cmd;
	char** arr = malloc(sizeof(char*) * 1024);
	char* tmp;
	int flag = 1;
        do {
		if (getcwd(cwd, sizeof(cwd)) != NULL) fprintf(stdout, "custom_shell_%s(pid:%d):~ %s$ ", getlogin(), getpid(), cwd);

		//read in input
        	fgets(input, 1024, stdin);
		//lets not waste memory
		tmp = strcpy((char*)malloc(sizeof(char) * (strlen(input) + 1)), input);

		//if user enters 'exit' terminate program
		if(strcmp(tmp, "exit\n") == 0){
			flag = 0;
			exit(0);
		}else{
			//new line characters is at the end of tmp, lets get rid of it
			char *newline = strchr( tmp, '\n' );
			if ( newline ) *newline = 0;

			//parse every argument into an array of strings
			cmd = strtok(tmp, " ");

			if(strcmp(cmd, "multi") == 0){ //parse commands from a file
				parse_file(arr, cmd);

				//try something. try redirecting to stdout even if not directed elsewhere....might work
				dup2(fileno(stdout), STDOUT_FILENO);

			}else{ //parse commands from command line
				//if(strcmp(cmd, "./main") == 0) printf("\nWELCOME TO MY SHELL! My PID is: %d\n\n", getpid());
				execute(cmd, arr);
			}
		}

	} while ( flag );

	//if program finished successfully
	return 0;
}


void parse_file(char** arr, char* cmd){
	char* file_name;
        char* output_command;
        char* output_file_name;

	int saved_stdout;
        // Save current stdout for use later just in case we modify it 
        saved_stdout = dup(1);

        file_name = strtok(NULL, " ");

        if((output_command = strtok(NULL, " ")) != NULL){
                if(((output_file_name = strtok(NULL, " ")) != NULL) && ((strcmp("output", output_command)) == 0)){
                        //redirect
                        FILE* output_file = fopen(output_file_name, "w+");
                        if(output_file == NULL){
                                printf("%s\n", strerror(errno));
                        }else{
                                //do redirect
				dup2(fileno(output_file), 1);
                                fclose(output_file);
                        }
                }else printf("command not recognized\n");
        }


	do_parse_file(arr, cmd, file_name);

	// Restore stdout. NOTE: it doesn't matter wheather or not we redirected it. Worse case is that we are restoring to stdout from stdout...no issues here
	dup2(saved_stdout, 1);
	close(saved_stdout);
}

void do_parse_file(char** arr, char* cmd, char* file_name){
	//char* file_name;
	char* line = NULL;
        size_t len = 0;
        ssize_t read;
	//char* output_command;
	//char* output_file_name;
        //file_name = strtok(NULL, " ");

	/*if((output_command = strtok(NULL, " ")) != NULL){
		if(((output_file_name = strtok(NULL, " ")) != NULL) && ((strcmp("output", output_command)) == 0)){
			//redirect
			FILE* output_file = fopen(output_file_name, "w+");
			if(output_file == NULL){
				printf("%s\n", strerror(errno));
			}else{
				//do redirect
				dup2(fileno(output_file), STDOUT_FILENO);
				fclose(output_file);
			}
		}else printf("command not recognized\n");
	}*/


        FILE* file = fopen(file_name, "r");
	if(file == NULL){
		printf("%s\n", strerror(errno));
	}else{
	        while ((read = getline(&line, &len, file)) != -1) {
        		char *newline = strchr( line, '\n' );
                	if ( newline ) *newline = 0;
                	cmd = strtok(line, " ");
                	if(strcmp(cmd, "multi") == 0){ //nested multi command
                		//recursion
				parse_file(arr, cmd);
                	}else{
                        	execute(cmd, arr);
                	}
		}
        	fclose(file);
	}
}


void execute(char* cmd, char** arr){

	pid_t pid = fork();

	if(pid == 0){ //child

		int i = 0;
        	arr[i++] = strcpy((char*)malloc(sizeof(char) * (strlen(cmd)+1)), cmd);
        	while(cmd != NULL){
                	cmd = strtok(NULL, " ");
                	if(cmd != NULL) arr[i++] = strcpy((char*)malloc(sizeof(char) * (strlen(cmd)+1)), cmd);
       	 	}

        	//to execute the command, we'll use execvp so we can pass in an n number of arguments
        	execvp(arr[0], arr);

		exit(errno);
	}else if(pid > 0){ //parent
		int status;
    		wait(&status);
    		if(WIFEXITED(status))
         		if(WEXITSTATUS(status) != 0) printf("command not recognized!\n");
	}

}
