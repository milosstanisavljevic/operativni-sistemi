#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int key;
	int s;
	int test; 
	key = atoi(argv[1]);
	char sNiz[50];
	
	if(!(strcmp(argv[1],"-h")) || !(strcmp(argv[1],"--help"))) {

		printf("\nUse this program to set the current active key.\nAfter setting the key, you can use encr and decr with that key.\nUsage: setkey [OPTION]... [KEY]\n\nCommand line options:\n\    -h, --help: Show help prompt.\n    -s, --secret: Enter the key via STDIN. Hide hey when entering it.\n");
	}else if(!(strcmp(argv[1],"-s")) || !(strcmp(argv[1],"--secret"))) {
		
		setecho(ECHO_OFF);
		printf("Enter key: ");			

		read(0, sNiz, 50);
		s = atoi(sNiz);

		setecho(ECHO_ON);
		test = setkey(s);
		
		printf("%d" ,test);
			
	}else if(argv[1]!=0){
		test = setkey(key);
		
		printf("%d" ,test);
	}
	exit();
}
