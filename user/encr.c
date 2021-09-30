#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int fd;
	int sys;


	if(!(strcmp(argv[1],"-h")) || !(strcmp(argv[1],"-help"))) {

		printf("\nUse this program to encrypt files written on the disk.\nUsage: encr [OPTION]... [FILE]...\n\nCommand line options:\n\    -h, --help: Show help prompt.\n    -a, --encrypt-all: Entcrypt all files in CWD with current key.\n");
	}else{
		if((fd = open(argv[1], 0))!=0){
		sys = encr(fd);

		close(fd);

			if(sys==-1){
				printf("Kljuc nije postavljen.\n");
			}else if(sys==-2){
				printf("datoteka je T_DEV tipa.\n");
			}else if(sys==-3){
				printf("datoteka je vec enkriptovana.\n");
			}else if(sys==0){
				printf("sistemski poziv se uspešno zavrsio.\n");
			}
		}else{
			printf("open error");
		}
	}
		
	exit();
}
