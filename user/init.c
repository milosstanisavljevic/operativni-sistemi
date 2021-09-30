// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };
char terminalname[] = "/dev/ttyN";

void
terminal(int n)
{
       int pid;

       terminalname[strlen(terminalname)-1] = n + '0';
       pid = fork();
       if(pid < 0){
               printf("init: fork failed\n");
               exit();
       }
       if(pid == 0){
               close(0);
               close(1);
               close(2);

               if(open(terminalname, O_RDWR) < 0){
                       mknod(terminalname, 1, n);
                       open(terminalname, O_RDWR);
               }
               dup(0);
               dup(0);

               printf("starting sh on %s!\n", terminalname);
               exec("/bin/sh", argv);
               printf("init: exec sh failed\n");
               exit();
       }
}

int
main(void)
{
	int wpid;

	if(getpid() != 1){
		fprintf(2, "init: already running\n");
		exit();
	}

	if(open("/dev/console", O_RDWR)){
		mknod("/dev/console", 1, 1);
		open("/dev/console", O_RDWR);
	}
	dup(0);  // stdout
	dup(0);  // stderr

	for(int i = 1 ; i <= 6 ; i++){
		terminal(i);
	}
	while(1){
	while((wpid=wait()) >= 0)
		printf("zombie!\n");
	}
	
}
