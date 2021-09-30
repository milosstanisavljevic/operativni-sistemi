#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int boja;
	if(!(strcmp(argv[1],"-h")) || !(strcmp(argv[1],"--help"))) {

		printf("\nUse this program to change colour of terminal.\nCommand line options:\n    -h, --help: Show help prompt.\n    -bg, --background: Change background colour.\n    -fg, --foreground: Change foreground colour.\n");
		exit();
	
	}else if(!(strcmp(argv[1],"-fg")) || !(strcmp(argv[1],"--foreground"))){
		if(!(strcmp(argv[2],"black"))){
			boja = 0x0000;
		}else if(!(strcmp(argv[2],"blue"))){
			boja = 0x0100;
		}else if(!(strcmp(argv[2],"green"))){
			boja = 0x0200;
		}else if(!(strcmp(argv[2],"aqua"))){
			boja = 0x0300;
		}else if(!(strcmp(argv[2],"red"))){
			boja = 0x0400;
		}else if(!(strcmp(argv[2],"purple"))){
			boja = 0x0500;
		}else if(!(strcmp(argv[2],"yellow"))){
			boja = 0x0600;
		}else if(!(strcmp(argv[2],"white"))){
			boja = 0x0700;
		}else if(!(strcmp(argv[2],"Lblack"))){
			boja = 0x0800;
		}else if(!(strcmp(argv[2],"Lblue"))){
			boja = 0x0900;
		}else if(!(strcmp(argv[2],"Lgreen"))){
			boja = 0x0A00;
		}else if(!(strcmp(argv[2],"Laqua"))){
			boja = 0x0B00;
		}else if(!(strcmp(argv[2],"Lred"))){
			boja = 0x0C00;
		}else if(!(strcmp(argv[2],"Lpurple"))){
			boja = 0x0D00;
		}else if(!(strcmp(argv[2],"Lyellow"))){
			boja = 0x0E00;
		}else if(!(strcmp(argv[2],"Lwhite"))){
			boja = 0x0F00;
		}
		
		colour(boja);
		exit();

	}else if (!(strcmp(argv[1],"-bg")) || !(strcmp(argv[1],"--background"))) {	
		if(!(strcmp(argv[2],"black"))){
			boja = 0x0000;
		}else if(!(strcmp(argv[2],"blue"))){
			boja = 0x1000;
		}else if(!(strcmp(argv[2],"green"))){
			boja = 0x2000;
		}else if(!(strcmp(argv[2],"aqua"))){
			boja = 0x3000;
		}else if(!(strcmp(argv[2],"red"))){
			boja = 0x4000;
		}else if(!(strcmp(argv[2],"purple"))){
			boja = 0x5000;
		}else if(!(strcmp(argv[2],"yellow"))){
			boja = 0x6000;
		}else if(!(strcmp(argv[2],"white"))){
			boja = 0x7000;
		}else if(!(strcmp(argv[2],"Lblack"))){
			boja = 0x8000;
		}else if(!(strcmp(argv[2],"Lblue"))){
			boja = 0x9000;
		}else if(!(strcmp(argv[2],"Lgreen"))){
			boja = 0xA000;
		}else if(!(strcmp(argv[2],"Laqua"))){
			boja = 0xB000;
		}else if(!(strcmp(argv[2],"Lred"))){
			boja = 0xC000;
		}else if(!(strcmp(argv[2],"Lpurple"))){
			boja = 0xD000;
		}else if(!(strcmp(argv[2],"Lyellow"))){
			boja = 0xE000;
		}else if(!(strcmp(argv[2],"Lwhite"))){
			boja = 0xF000;
		}
		colour2(boja);
		exit();

	}/*else {
		boja = atoi(argv[1]);
		colour(boja);
		exit();
	}*/



	

	exit();
}
