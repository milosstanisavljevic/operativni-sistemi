#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int fd;
	int sys;

	if(!(strcmp(argv[1],"-h")) || !(strcmp(argv[1],"-help"))) {

		printf("\nUse this program to decrypt files written on the disk.\nUsage: decr [OPTION]... [FILE]...\n\nCommand line options:\n\    -h, --help: Show help prompt.\n    -a, --decrypt-all: Decrypt all files in CWD with current key.\n");
	}else{
		if((fd = open(argv[1], 0))!=0){
		sys = decr(fd);

		close(fd);

			if(sys==-1){
				printf("Kljuc nije postavljen.\n");
			}else if(sys==-2){
				printf("datoteka je T_DEV tipa.\n");
			}else if(sys==-3){
				printf("datoteka nije enkriptovana.\n");
			}else if(sys==0){
				printf("sistemski poziv se uspesno zavrsio.\n");
			}
		}else{
			printf("greska");
		}
	}
	exit();
}
