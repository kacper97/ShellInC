#include "job_control.h"   // remember to compile with module job_control.c 

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;          /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	
	
		while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("COMMAND->");  //input for command
		fflush(stdout); 	 //stdout is the file into which the kernel writes its output 
	
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue;// if empty command just wait
		if(strcmp(args[0],"exit") ==0) exit(1); //if exit typed leave
	
	int pid_fork=fork();  // declaring work

	// child process
    if (pid_fork == 0) {
		
			//fclose(stdin); // close child's stdin
			//fopen("/dev/null", "r"); // open a new stdin that is always empty
			execvp(args[0],args);
			// If an error occurs, print error and exit
			
			fprintf (stderr, "unknown command: %s\n", args[0]);
			exit(1);
			
		}  else {
        // parent process, waiting on child process
		if (background==1) {
			//pid_wait=pid_fork; // wait becomes fork
			//status=analyze_status(status,&info); //declaring what status is
			printf("\nBackground job running... pid %d,command %s\n\n",pid_fork,args[0],status_strings[status],info);
			
		} else { 
			pid_wait=waitpid(pid_fork, &status, WUNTRACED );
			status= analyze_status(status,&info);	
			if(info !=1){
			printf("\nForeground pid: %d, command %s, %s , info: %i\n\n\n",pid_fork,args[0],status_strings[status], info);
			}
			}
		}
	}
}
 





