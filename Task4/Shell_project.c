#include "job_control.h"   // remember to compile with module job_control.c 

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

job* list;
/*part of task 4 which is to include jobs*/
static void jobs()
{
    if(empty_list(list) != 1)
    {
        print_job_list(list);
    } else
    {
        printf("The list is empty.\n");
    }
}

static void child_handler(int sig)
{
    enum status status_res;
    int exStat; 
	int info;
	int hasToBeDeleted;
	pid_t pid;
    job* processList = list->next; //gets next process from the list
    while(processList != NULL)
    {
        hasToBeDeleted = 0; // declaring has to be deleted
        pid = waitpid(processList->pgid, &exStat, WNOHANG | WUNTRACED);
        if(pid == processList->pgid)
        {
            status_res = analyze_status(exStat, &info);
        if(status_res != SUSPENDED)
            {
                hasToBeDeleted = 1;
                printf("The process %d has finished successfully.\n", processList->pgid);
            } else
            {
                printf("The process %d is suspended.\n", processList->pgid);
                processList->state = STOPPED;
            }
        }
        if(hasToBeDeleted==1)
        {
            struct job_* jobToDelete = processList;
            processList = processList->next;
            delete_job(list, jobToDelete);
        } else
        {
            processList = processList->next;
        }
  }
}



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
	list = new_list("Jobs");

	 //TERMINAL
		while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("COMMAND->");  //input for command
		fflush(stdout); 	 //stdout is the file into which the kernel writes its output 
		ignore_terminal_signals(); //ignore ctrl c + ctrl \ or ctrl z 
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue;// if empty command just wait
		if(strcmp(args[0],"exit") ==0) exit(1); //if exit typed leave
		if(strcmp(args[0],"cd")==0){
			chdir(args[1]);
		}
		
		
	int pid_fork=fork();  // declaring work
	
	int group = new_process_group(pid_fork);

	// CHILD PROCESS
    if (pid_fork == 0) {
			setpgid(pid_fork,group); // different from parent pid group
			restore_terminal_signals(); // restores signals to terminal
			execvp(args[0],args);
			
			/* this makes sure if args 1 there will be no error
			as args[0] are executed and not args[1]
			*/
			if(args[1]){
			}

			else{
			// If an error occurs, print error and exit
			fprintf (stderr, "unknown command: %s\n", args[0]);
			exit(1);
			}
               
    } else {
		
        // PARENT PROCESS WAITING ON A CHILD PROCESS
		//BACKGROUND
		
		if (background==1) {
			add_job(list, new_job(pid_fork, args[0], BACKGROUND)); //adds job to the background
			printf("\nBackground job running... pid %d,command %s\n\n",pid_fork,args[0],status_strings[status],info);
			unblock_SIGCHLD();
	
	//FOREGROUND
	} else { 
		//assign a group id
			int group = new_process_group(pid_fork);
			int groupID = setpgid(pid_fork,group); // sets the group id
			
			// become owner of terminal
			set_terminal(groupID);
			tcsetpgrp(STDIN_FILENO,groupID);
			
			pid_wait=waitpid(pid_fork, &status, WUNTRACED );
			status= analyze_status(status,&info);	
			
			if(info !=1){
			printf("\nForeground pid: %d, command %s, %s , info: %i\n\n\n",pid_fork,args[0],status_strings[status], info);
			}
			//printf("group id: %i \n", getpgrp()); THIS IS TO TEST GROUP ID FUNCTION AND IF IT WORKS

			// if terminal is suspended the following happens : 
			if (status_res == SUSPENDED) {
                       add_job(list, new_job(pid_fork, args[0], STOPPED));
                       printf("Process (pid: %d, command: %s) has been suspended.\n", getpid(), args[0]);
                       unblock_SIGCHLD();
			}
		}
	}
	}
 }
 







