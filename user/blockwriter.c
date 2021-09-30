#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "kernel/syscall.h"
#include "kernel/traps.h"
#include "kernel/memlayout.h"

int
main(int argc, char *argv[])
{
	int fd;
	int n =0;
	int br=0;
	char *str;
	int blocks;
	char cakica[512];
	//abcdefghijklmnopqrstuvwxyz
	while(n < 512) {
	for(int i = 97; i < 123 ; i++){
		cakica[n] = i;
		n++;	
	}
	}
	
	if(!(strcmp(argv[1],"-h")) || !(strcmp(argv[1],"--help"))) {

		printf("\nUse this program to create a big file filled with a-z characters.\nDefault filename: long.txt\nDefault blocks: 150\nUsage: blockwriter [OPTION]l...\n\nCommand line options:\n    -h, --help: Show help prompt.\n    -b, --blocks: Number of blocks to write.\n    -o, --output-file: Set output filename.\n");
	
	}else if(!(strcmp(argv[1],"-o")) || !(strcmp(argv[1],"--output-file"))){
			
		str=argv[2];
		printf("%s",str);

	}else if (!(strcmp(argv[1],"-b")) || !(strcmp(argv[1],"--blocks"))) {	

		blocks = atoi(argv[2]);
		printf("Number of blocks: %d\n",blocks);

	} if(!(strcmp(argv[3],"-b")) || !(strcmp(argv[3],"--blocks"))){				
		blocks = atoi(argv[4]);
		printf("Number of blocks: %d\n",blocks);
	
	}else if(!(strcmp(argv[3],"-o")) || !(strcmp(argv[3],"--output-file"))){
		
		str=argv[4];
		printf("%s",str);
	}
		if(str == 0) {
			str = "long.txt";
		}
		fd = open(str, O_CREATE | O_RDWR);
		if(fd < 0){
			printf("create failed\n");
			exit();
		}
		for(int i =0;i<blocks;i++){
			write(fd,cakica,512);
			printf("Creating block number:%d\n",br++);
		}
		
		close(fd);
	exit();
}
