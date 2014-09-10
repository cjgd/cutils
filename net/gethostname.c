/* gethostname.c -- a poor's man nslookup */
/* Carlos Duarte <cgd@teleweb.pt>, 000425 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int i; 
	if (argc == 1) {
		fprintf(stderr, "\
usage: %s hostname1 hostname2 ...\n\
\n\
converts given hostnames into their IP addresses\n", argv[0]); 
		exit(1);
	}

	for (i=1; i<argc; i++) {
		struct hostent *h; 
		char *host = argv[i]; 
		char **z; 

		h = gethostbyname(host); 
		if (!h) {
			fprintf(stderr, "couldn't resolv %s\n", host); 
			continue;
		}
		printf("Name: %s\n", h->h_name); 
		z = h->h_aliases; 
		if (*z) {
			printf("Aliases: "); 
			for (; *z; z++)
				printf("%s%s", *z, z[1] ? ", " : "\n"); 
		}
		z = h->h_addr_list; 
		if (*z) {
			printf("Adresses: "); 
			for (; *z; z++) {
				char *name;
				struct in_addr in; 
				memcpy(&in, *z, sizeof in); 
			       	name = inet_ntoa(in); 
				printf("%s%s", name, z[1] ? ", " : "\n"); 
			}
		}
	}
	return 0; 
}

