/* cmos.c -- cmos save/restore  */
/* Carlos Duarte <cgd@teleweb.pt>, 960603 */

/*

-r, 		restore cmos
-s, 		save cmos
-f file,	specify file (def file is cmos.dat)

default behavior is save to cmos.dat, if -f file, then save to file

*/

#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
typedef char * rd_wr_buf_t;
#else
#include <io.h>
#include <sys\stat.h>
typedef void * rd_wr_buf_t;
#endif

#include <fcntl.h>

void restore_cmos(char *file_name);
void save_cmos(char *file_name);
void usage(char *reason);

#define DEF_FILENAME 	"cmos.dat"

#ifdef __linux__
#define PORT_FILENAME	"/dev/port"
#endif

typedef unsigned char cmos_t;

char *prog_name;

int main(int argc, char **argv)
{
	char c;
	char *s;
	char *filename = NULL;
	int restore_flag = 0;

	prog_name = argv[0];

	while(--argc > 0 && (s = *++argv, *s) == '-')
		while((c = *++s) != '\0')
			switch(c)
			{
				case 'r':
					restore_flag++; break;
				case 's':
					if(restore_flag)
						usage("can't use -s with -r");
					break;
				case 'f':
					if(*++s != '\0')
						filename = s;
					else if(--argc > 0)
						filename = *++argv;
					else
						usage(NULL);
					while(*++s); s--; /* break inner while */
					break;
				default:
					usage("wrong flags");
					break;
			}
	if(argc != 0)
		usage("extra args");

	(restore_flag ? restore_cmos : save_cmos)(filename ?
						filename : DEF_FILENAME);
	return 0;
}

void restore_cmos(char *file)
{
	cmos_t tab[0x40];
	cmos_t *tabp = tab;
	int fd = -1;
	int i;
#ifdef __linux__
	 char c[2];
#endif

	if((fd = open(file, O_RDONLY
#ifndef __linux
					| O_BINARY
#endif
							)) == -1)
	{
		perror(file); goto exit_err;
	}
	if(read(fd, (rd_wr_buf_t)tab, sizeof tab) != sizeof tab)
	{
		perror("read"); goto exit_err;
	}
	close(fd);

#ifdef __linux__
	if((fd = open(PORT_FILENAME, O_RDONLY)) == -1)
	{
		perror(PORT_FILENAME); goto exit_err;
	}
	if(lseek(fd, 0x70, SEEK_SET) == -1)
	{
		perror("lseek"); goto exit_err;
	}
#endif
	for(i = 0; i < 0x40; i++)
	{
#ifdef __linux__
		c[0] = i;
		c[1] = *tabp++;
		if(write(fd, c, 2) != 2)
		{
			perror("port write"); goto exit_err;
		}
		if(lseek(fd, -2, SEEK_CUR) == -1)
		{
			perror("lseek"); goto exit_err;
		}
#else
		int val = *tabp++;
		asm mov ax, i
		asm out 0x70, al
		asm mov ax, val
		asm out 0x71, al
#endif
	}
#ifdef __linux__
	close(fd);
#endif
	return;

exit_err:
	if(fd != -1) close(fd);
	exit(1);
}

void save_cmos(char *file)
{
	cmos_t tab[0x40];
	cmos_t *tabp = tab;
	int fd = -1;
	int i; char c;

#ifdef __linux__
	if((fd = open(PORT_FILENAME, O_RDWR)) == -1)
	{
		perror(PORT_FILENAME); goto exit_err;
	}
	if(lseek(fd, 0x70, SEEK_SET) != 0x70)
	{
		perror("lseek"); goto exit_err;
	}
#endif
	for(i = 0; i < 0x40; i++)
	{
		c = i;
#ifdef __linux__
		if(write(fd, &c, 1) != 1)
		{
			perror("write to port"); goto exit_err;
		}
		if(read(fd, &c, 1) != 1)
		{
			perror("read from port"); goto exit_err;
		}
		if(lseek(fd, -2, SEEK_CUR) == -1)
		{
			perror("lseek"); goto exit_err;
		}
#else
		asm mov ax, i
		asm out 0x70, al
		asm in al, 0x71
		asm mov c, al
#endif
		*tabp++ = c;
	}
#ifdef __linux__
	close(fd);
#endif
	if((fd = open(file,
#ifdef __linux__
			O_CREAT | O_WRONLY, O_TRUNC,
#else
			O_CREAT | O_WRONLY | O_TRUNC | O_BINARY,
#endif
#ifdef __linux__
			0644
#else
			S_IREAD | S_IWRITE
#endif
				)) == -1)
	{
		perror(file); goto exit_err;
	}
	if(write(fd, (rd_wr_buf_t)tab, sizeof tab) != sizeof tab)
	{
		perror("write"); goto exit_err;
	}
	close(fd);
	return;

exit_err:
	if(fd != 1) close(fd);
	exit(1);
}

void usage(char *reason)
{
	if(reason)
		fprintf(stderr, "%s\n", reason);

	fprintf(stderr, "usage: %s [-r|-s] [-f file]\n", prog_name);
	exit(1);
}
