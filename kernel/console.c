// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "colors.h"

static void consputc(int);

static int panicked = 0;

static struct {
	struct spinlock lock;
	int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
	static char digits[] = "0123456789abcdef";
	char buf[16];
	int i;
	uint x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);

	if(sign)
		buf[i++] = '-';

	while(--i >= 0)
		consputc(buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
	int i, c, locking;
	uint *argp;
	char *s;

	locking = cons.locking;
	if(locking)
		acquire(&cons.lock);

	if (fmt == 0)
		panic("null fmt");

	argp = (uint*)(void*)(&fmt + 1);
	for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
		if(c != '%'){
			consputc(c);
			continue;
		}
		c = fmt[++i] & 0xff;
		if(c == 0)
			break;
		switch(c){
		case 'd':
			printint(*argp++, 10, 1);
			break;
		case 'x':
		case 'p':
			printint(*argp++, 16, 0);
			break;
		case 's':
			if((s = (char*)*argp++) == 0)
				s = "(null)";
			for(; *s; s++)
				consputc(*s);
			break;
		case '%':
			consputc('%');
			break;
		default:
			// Print unknown % sequence to draw attention.
			consputc('%');
			consputc(c);
			break;
		}
	}

	if(locking)
		release(&cons.lock);
}

void
panic(char *s)
{
	int i;
	uint pcs[10];

	cli();
	cons.locking = 0;
	// use lapiccpunum so that we can call panic from mycpu()
	cprintf("lapicid %d: panic: ", lapicid());
	cprintf(s);
	cprintf("\n");
	getcallerpcs(&s, pcs);
	for(i=0; i<10; i++)
		cprintf(" %p", pcs[i]);
	panicked = 1; // freeze other CPU
	for(;;)
		;
}

#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

int trenutnaBoja=0x0700;

static void
cgaputc(int c)
{
	int pos;

	// Cursor position: col + 80*row.
	outb(CRTPORT, 14);
	pos = inb(CRTPORT+1) << 8;
	outb(CRTPORT, 15);
	pos |= inb(CRTPORT+1);

	if(c == '\n')
		pos += 80 - pos%80;
	else if(c == BACKSPACE){
		if(pos > 0) --pos;
	}
		 else
		crt[pos++] = (c&0xff) | trenutnaBoja;  // black on white

	if(pos < 0 || pos > 25*80)
		panic("pos under/overflow");

	if((pos/80) >= 24){  // Scroll up.
		memmove(crt, crt+80, sizeof(crt[0])*23*80);
		pos -= 80;
		memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
	}

	outb(CRTPORT, 14);
	outb(CRTPORT+1, pos>>8);
	outb(CRTPORT, 15);
	outb(CRTPORT+1, pos);
	crt[pos] = ' ' | trenutnaBoja;
}

void
consputc(int c)
{
	if(panicked){
		cli();
		for(;;)
			;
	}

	if(c == BACKSPACE){
		uartputc('\b'); uartputc(' '); uartputc('\b');
	} else
		uartputc(c);
	cgaputc(c);
}

#define INPUT_BUF 128
struct {
	char buf[INPUT_BUF];
	uint r;  // Read index
	uint w;  // Write index
	uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x
// alt mapa
char niz1[10]= {'B','l','a','c','k','\0'};
char pocetak[25]= {'/','-','-','-','<','F','G','>','-','-','-',' ','-','-','-','<','B','G','>','-','-','-','\\','\0'};
char niz3[10]= {'G','r','e','e','n','\0'};
char niz2[10] = {'B','l','u','e','\0'};
char niz4[10] = {'A','q','u','a','\0'};
char niz5[10] = {'R','e','d','\0'};
char niz6[10] = {'P','u','r','p','l','e','\0'};
char niz7[10] = {'Y','e','l','l','o','w','\0'};
char niz8[10] = {'W','h','i','t','e','\0'};
char kraj[25] ={'\\','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','/','\0'};
int p=0,k=0,m=0,n=0,red=0,kolona=0, isTableOn=0;
char nizCons[801];
char console[2000];


void tabela()
{
	for(int i=57;i<777; i++){
		if(i % 80 >57){
			crt[i] = ' ';
		}
	}
	for(int i = 57; i < 80 ; i++){
		crt[i] = 0x0f00 | pocetak[i-57];
		crt[i+9*80] = 0x0f00 | kraj[i-57];
					
	}

	for(int i=137; i<137+8*80; i+=80){	
		crt[i] = 0x0f00 | '|' ;
		crt[i+22] = 0x0f00 | '|' ;
		crt[i+11] = 0x0f00 | '|';
	}
	for(int i = 0; i<9;i++){
		crt[i+138] = 0x0f00 | niz1[i];
		crt[i+138+80] = 0x0f00 | niz2[i];
		crt[i+138+2*80] = 0x0f00 | niz3[i];
		crt[i+138+3*80] = 0x0f00 | niz4[i];
		crt[i+138+4*80] = 0x0f00 | niz5[i];
		crt[i+138+5*80] = 0x0f00 | niz6[i];
		crt[i+138+6*80] = 0x0f00 | niz7[i];
		crt[i+138+7*80] = 0x0f00 | niz8[i];
	}
	for(int i = 0; i < 8 ; i ++){
		crt[i+149] = 0x0f00 | niz1[i];
		crt[i+149+80] = 0x0f00 | niz2[i];
		crt[i+149+2*80] = 0x0f00 | niz3[i];
		crt[i+149+3*80] = 0x0f00 | niz4[i];
		crt[i+149+4*80] = 0x0f00 | niz5[i];
		crt[i+149+5*80] = 0x0f00 | niz6[i];
		crt[i+149+6*80] = 0x0f00 | niz7[i];
		crt[i+149+7*80] = 0x0f00 | niz8[i];
	}
}
void
consoleintr(int (*getc)(void))
{
	int c, doprocdump = 0;

	acquire(&cons.lock);
	while((c = getc()) >= 0){
		switch(c){
		case C('P'):  // Process listing.
			// procdump() locks cons.lock indirectly; invoke later
			doprocdump = 1;
			break;
		case C('U'):  // Kill line.
			while(input.e != input.w &&
			      input.buf[(input.e-1) % INPUT_BUF] != '\n'){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case C('H'): case '\x7f':  // Backspace
			if(input.e != input.w){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case C('C'):
			m++;
			break;
		case C('O'):
			if(m>0){
			n++;
			};
			break;
		case C('L'): 
				if(n%2==1){
					isTableOn=1;
					for(int i=0; i < 800; i++){
						nizCons[i]=crt[i];
					}
					tabela();
				
			}else if(isTableOn==1)	{
				isTableOn=0;
                                for(int i=0; i < 800; i++){
					crt[i] = trenutnaBoja | nizCons[i];
				}
			}
			break;
		
		case 's':
			if(isTableOn==1){
				red++;									
				for(int i=0; i<10; i++){
				if(kolona%2==0){
					if(red%8==1){
						crt[i+138] = 0xf000 | niz1[i];
						crt[i+138+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+138] = 0x0f00 | niz1[i];						
						crt[i+138+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+138+80] = 0x0f00 | niz2[i];						
						crt[i+138+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+138+80*2] = 0x0f00 | niz3[i];						
						crt[i+138+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+138+80*3] = 0x0f00 | niz4[i];						
						crt[i+138+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+138+80*4] = 0x0f00 | niz5[i];						
						crt[i+138+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+138+80*5] = 0x0f00 | niz6[i];						
						crt[i+138+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+138+80*6] = 0x0f00 | niz7[i];						
						crt[i+138+80*7] = 0xf000 | niz8[i];					
					}
				}else if(kolona%2==1){
					if(red%8==1){
						crt[i+149] = 0xf000 | niz1[i];
						crt[i+149+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+149] = 0x0f00 | niz1[i];						
						crt[i+149+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+149+80] = 0x0f00 | niz2[i];						
						crt[i+149+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+149+80*2] = 0x0f00 | niz3[i];						
						crt[i+149+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+149+80*3] = 0x0f00 | niz4[i];						
						crt[i+149+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+149+80*4] = 0x0f00 | niz5[i];						
						crt[i+149+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+149+80*5] = 0x0f00 | niz6[i];						
						crt[i+149+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+149+80*6] = 0x0f00 | niz7[i];						
						crt[i+149+80*7] = 0xf000 | niz8[i];					
					}
				}
			}
		}else{
			consputc('s');
		}
			break;

		case 'w':
				if(isTableOn==1){
				red--;
				for(int i=0 ; i<10 ; i++){
					if(kolona%2==0){		
					if(red%8==1){
						crt[i+138] = 0xf000 | niz1[i];
						crt[i+138+80] = 0x0f00 | niz2[i];
						crt[i+138+80*7] = 0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+138] = 0x0f00 | niz1[i];						
						crt[i+138+80] = 0xf000 | niz2[i];
						crt[i+138+80*2] = 0x0f00 | niz3[i];
					}else if(red%8==3){
						crt[i+138+80] = 0x0f00 | niz2[i];						
						crt[i+138+80*2] = 0xf000 | niz3[i];
						crt[i+138+80*3] = 0x0f00 | niz4[i];					
					}else if(red%8==4){
						crt[i+138+80*4] = 0x0f00 | niz5[i];						
						crt[i+138+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+138+80*5] = 0x0f00 | niz4[i];						
						crt[i+138+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+138+80*6] = 0x0f00 | niz7[i];						
						crt[i+138+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+138+80*7] = 0x0f00 | niz8[i];						
						crt[i+138+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+138] = 0x0f00 | niz1[i];						
						crt[i+138+80*7] = 0xf000 | niz8[i];					
					}
					}else if(kolona%2==1){
						if(red%8==1){
						crt[i+149] = 0xf000 | niz1[i];
						crt[i+149+80] = 0x0f00 | niz2[i];
						crt[i+149+80*7] = 0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+149] = 0x0f00 | niz1[i];						
						crt[i+149+80] = 0xf000 | niz2[i];
						crt[i+149+80*2] = 0x0f00 | niz3[i];
					}else if(red%8==3){
						crt[i+149+80] = 0x0f00 | niz2[i];						
						crt[i+149+80*2] = 0xf000 | niz3[i];
						crt[i+149+80*3] = 0x0f00 | niz4[i];					
					}else if(red%8==4){
						crt[i+149+80*4] = 0x0f00 | niz5[i];						
						crt[i+149+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+149+80*5] = 0x0f00 | niz4[i];						
						crt[i+149+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+149+80*6] = 0x0f00 | niz7[i];						
						crt[i+149+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+149+80*7] = 0x0f00 | niz8[i];						
						crt[i+149+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+149] = 0x0f00 | niz1[i];						
						crt[i+149+80*7] = 0xf000 | niz8[i];					
					}	
				}
				}
			}else{
				consputc('w');
			}
			break;
		
		case 'a': case 'd' :
				if(isTableOn==1){				
				kolona++;
				if(kolona%2==1){
					for(int i=0; i<10; i++){
						if(red%8==1){
							crt[i+138] = 0x0f00 | niz1[i];
							crt[i+149] = 0xf000 | niz1[i];
						}else if(red%8==2){					
							crt[i+138+80] = 0x0f00 | niz2[i];
							crt[i+149+80] = 0xf000 | niz2[i];
						}else if(red%8==3){						
							crt[i+138+80*2] = 0x0f00 | niz3[i];
							crt[i+149+80*2] = 0xf000 | niz3[i];					
						}else if(red%8==4){
							crt[i+138+80*3] = 0x0f00 | niz4[i];						
							crt[i+149+80*3] = 0xf000 | niz4[i];					
						}else if(red%8==5){
							crt[i+138+80*4] = 0x0f00 | niz5[i];						
							crt[i+149+80*4] = 0xf000 | niz5[i];					
						}else if(red%8==6){
							crt[i+138+80*5] = 0x0f00 | niz6[i];						
							crt[i+149+80*5] = 0xf000 | niz6[i];					
						}else if(red%8==7){
							crt[i+138+80*6] = 0x0f00 | niz7[i];						
							crt[i+149+80*6] = 0xf000 | niz7[i];					
						}else if(red%8==0){
							crt[i+138+80*7] = 0x0f00 | niz8[i];						
							crt[i+149+80*7] = 0xf000 | niz8[i];					
						}	
					}			
				}else if(kolona%2==0){
					for(int i=0; i<10; i++){
						if(red%8==1){
							crt[i+138] = 0xf000 | niz1[i];
							crt[i+149] = 0x0f00 | niz1[i];
						}else if(red%8==2){					
							crt[i+138+80] = 0xf000 | niz2[i];
							crt[i+149+80] = 0x0f00 | niz2[i];
						}else if(red%8==3){						
							crt[i+138+80*2] = 0xf000 | niz3[i];
							crt[i+149+80*2] = 0x0f00 | niz3[i];					
						}else if(red%8==4){
							crt[i+138+80*3] = 0xf000 | niz4[i];						
							crt[i+149+80*3] = 0x0f00 | niz4[i];					
						}else if(red%8==5){
							crt[i+138+80*4] = 0xf000 | niz5[i];						
							crt[i+149+80*4] = 0x0f00 | niz5[i];					
						}else if(red%8==6){
							crt[i+138+80*5] = 0xf000 | niz6[i];						
							crt[i+149+80*5] = 0x0f00 | niz6[i];					
						}else if(red%8==7){
							crt[i+138+80*6] = 0xf000 | niz7[i];						
							crt[i+149+80*6] = 0x0f00 | niz7[i];					
						}else if(red%8==0){
							crt[i+138+80*7] = 0xf000 | niz8[i];						
							crt[i+149+80*7] = 0x0f00 | niz8[i];					
						}
					}
					}
				}else{
					consputc(c);
				}
			break;
		case 'e' :
			if(isTableOn==1){		
				if(kolona%2==0){
				//crt[pos++] = (c&0xff) | trenutnaBoja;  // black on white
				trenutnaBoja = trenutnaBoja & 0xf000;
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja & 0x0f00;
				}
				for(int i =0; i<2000;i++){
					console[i] = crt[i];
				}if(red%8 != 0){
				for(int i = 0 ; i<2000; i++){
					if(kolona%2==0){
						crt[i] = console[i] | ( trenutnaBoja | osnovneBoje1[(red%8)-1] );
						
					}else if(kolona%2==1){
						crt[i] = console[i] | ( trenutnaBoja | osnovneBoje2[(red%8)-1] );
					}
				}
				}else if(red%8 == 0) {
				for(int i = 0 ; i<2000; i++){
					if(kolona%2==0){
						crt[i] = console[i] | ( trenutnaBoja | osnovneBoje1[(red%8+7)] );
						
					}else if(kolona%2==1){
						crt[i] = console[i] | ( trenutnaBoja | osnovneBoje2[ (red%8+7)] );
					}
				}
				}
				if(red%8 != 0){
				if(kolona%2==0){				
					trenutnaBoja = trenutnaBoja | osnovneBoje1[(red%8)-1];
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja | osnovneBoje2[(red%8)-1];
				}
				}else if(red%8 == 0){
				if(kolona%2==0){				
					trenutnaBoja = trenutnaBoja | osnovneBoje1[(red%8)+7];
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja | osnovneBoje2[(red%8)+7];
				}
				}
				tabela();
				
				for(int i=0; i<10; i++){
					if(kolona%2==0){
					if(red%8==1){
						crt[i+138] = 0xf000 | niz1[i];
						crt[i+138+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+138] = 0x0f00 | niz1[i];						
						crt[i+138+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+138+80] = 0x0f00 | niz2[i];						
						crt[i+138+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+138+80*2] = 0x0f00 | niz3[i];						
						crt[i+138+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+138+80*3] = 0x0f00 | niz4[i];						
						crt[i+138+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+138+80*4] = 0x0f00 | niz5[i];						
						crt[i+138+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+138+80*5] = 0x0f00 | niz6[i];						
						crt[i+138+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+138+80*6] = 0x0f00 | niz7[i];						
						crt[i+138+80*7] = 0xf000 | niz8[i];					
					}
				}else if(kolona%2==1){
					if(red%8==1){
						crt[i+149] = 0xf000 | niz1[i];
						crt[i+149+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+149] = 0x0f00 | niz1[i];						
						crt[i+149+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+149+80] = 0x0f00 | niz2[i];						
						crt[i+149+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+149+80*2] = 0x0f00 | niz3[i];						
						crt[i+149+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+149+80*3] = 0x0f00 | niz4[i];						
						crt[i+149+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+149+80*4] = 0x0f00 | niz5[i];						
						crt[i+149+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+149+80*5] = 0x0f00 | niz6[i];						
						crt[i+149+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+149+80*6] = 0x0f00 | niz7[i];						
						crt[i+149+80*7] = 0xf000 | niz8[i];					
					}
				}
			
				}
			}else{
					consputc('e');
				}
			break;
		case 'r' : 
			if(isTableOn==1){
				if(kolona%2==0){
				//crt[pos++] = (c&0xff) | trenutnaBoja;  // black on white
					trenutnaBoja = trenutnaBoja & 0xf000;
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja & 0x0f00;
				}		
				for(int i =0; i<2000;i++){
					console[i] = crt[i];
				}
				if(red%8 != 0){
				for(int i = 0 ; i<2000; i++){
					if(kolona%2==0){
						crt[i] = console[i] | (trenutnaBoja | svetlijeBoje1[(red%8)-1]);
					}else if(kolona%2==1){
						crt[i] = console[i] | (trenutnaBoja | svetlijeBoje2[(red%8)-1]);
					}
				}
				}else if(red%8 == 0) {
				for(int i = 0 ; i<2000; i++){
					if(kolona%2==0){
						crt[i] = console[i] | ( trenutnaBoja | svetlijeBoje1[(red%8+7)] );
						
					}else if(kolona%2==1){
						crt[i] = console[i] | ( trenutnaBoja | svetlijeBoje2[ (red%8+7)] );
					}
				}
				}
				if(red%8 != 0){
				if(kolona%2==0){				
					trenutnaBoja = trenutnaBoja | svetlijeBoje1[(red%8)-1];
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja | svetlijeBoje2[(red%8)-1];
				}
				}else if(red%8 == 0){
				if(kolona%2==0){				
					trenutnaBoja = trenutnaBoja | svetlijeBoje1[(red%8)+7];
				}else if(kolona%2==1){
					trenutnaBoja = trenutnaBoja | svetlijeBoje2[(red%8)+7];
				}
				}
				tabela();
				
				for(int i=0; i<10; i++){
					if(kolona%2==0){
					if(red%8==1){
						crt[i+138] = 0xf000 | niz1[i];
						crt[i+138+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+138] = 0x0f00 | niz1[i];						
						crt[i+138+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+138+80] = 0x0f00 | niz2[i];						
						crt[i+138+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+138+80*2] = 0x0f00 | niz3[i];						
						crt[i+138+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+138+80*3] = 0x0f00 | niz4[i];						
						crt[i+138+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+138+80*4] = 0x0f00 | niz5[i];						
						crt[i+138+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+138+80*5] = 0x0f00 | niz6[i];						
						crt[i+138+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+138+80*6] = 0x0f00 | niz7[i];						
						crt[i+138+80*7] = 0xf000 | niz8[i];					
					}
				}else if(kolona%2==1){
					if(red%8==1){
						crt[i+149] = 0xf000 | niz1[i];
						crt[i+149+80*7] =0x0f00 | niz8[i];
					}else if(red%8==2){
						crt[i+149] = 0x0f00 | niz1[i];						
						crt[i+149+80] = 0xf000 | niz2[i];
					}else if(red%8==3){
						crt[i+149+80] = 0x0f00 | niz2[i];						
						crt[i+149+80*2] = 0xf000 | niz3[i];					
					}else if(red%8==4){
						crt[i+149+80*2] = 0x0f00 | niz3[i];						
						crt[i+149+80*3] = 0xf000 | niz4[i];					
					}else if(red%8==5){
						crt[i+149+80*3] = 0x0f00 | niz4[i];						
						crt[i+149+80*4] = 0xf000 | niz5[i];					
					}else if(red%8==6){
						crt[i+149+80*4] = 0x0f00 | niz5[i];						
						crt[i+149+80*5] = 0xf000 | niz6[i];					
					}else if(red%8==7){
						crt[i+149+80*5] = 0x0f00 | niz6[i];						
						crt[i+149+80*6] = 0xf000 | niz7[i];					
					}else if(red%8==0){
						crt[i+149+80*6] = 0x0f00 | niz7[i];						
						crt[i+149+80*7] = 0xf000 | niz8[i];					
					}
				}
			
				}
			}else{
					consputc('r');
				}
			break;

		case 'q': case 't': case 'y': case 'u': case 'i': case 'o': case 'p': case '[': case ']': case 'f': case 'g': case 'h':	
		case 'k': case 'j': case 'l': case ';': case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm': case ',':
		case '.': case '/': case '`': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		case '0': case '-': case '=': case '<': 
			if(isTableOn==1){
				break;	
			}else{
				cgaputc(c);
			}
			break;


		default:
						
			if(c != 0 && input.e-input.r < INPUT_BUF){
				c = (c == '\r') ? '\n' : c;
				input.buf[input.e++ % INPUT_BUF] = c;
				consputc(c);
				if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
					input.w = input.e;
					wakeup(&input.r);
				}
			}
			break;
		}
	}
	release(&cons.lock);
	if(doprocdump) {
		procdump();  // now call procdump() wo. cons.lock held
	}
}

int
consoleread(struct inode *ip, char *dst, int n)
{
	uint target;
	int c;

	iunlock(ip);
	target = n;
	acquire(&cons.lock);
	while(n > 0){
		while(input.r == input.w){
			if(myproc()->killed){
				release(&cons.lock);
				ilock(ip);
				return -1;
			}
			sleep(&input.r, &cons.lock);
		}
		c = input.buf[input.r++ % INPUT_BUF];
		if(c == C('D')){  // EOF
			if(n < target){
				// Save ^D for next time, to make sure
				// caller gets a 0-byte result.
				input.r--;
			}
			break;
		}
		*dst++ = c;
		--n;
		if(c == '\n')
			break;
	}
	release(&cons.lock);
	ilock(ip);

	return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
	int i;

	iunlock(ip);
	acquire(&cons.lock);
	for(i = 0; i < n; i++)
		consputc(buf[i] & 0xff);
	release(&cons.lock);
	ilock(ip);

	return n;
}

void
consoleinit(void)
{
	initlock(&cons.lock, "console");

	devsw[CONSOLE].write = consolewrite;
	devsw[CONSOLE].read = consoleread;
	cons.locking = 1;

	ioapicenable(IRQ_KBD, 0);
}

