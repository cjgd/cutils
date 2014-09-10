/* singals.c -- probe for system signals */
/* $Id$ */
/* Carlos Duarte <cgd@teleweb.pt>, 990328 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define SIG_RET_TYPE void

#define e(signame, desc) { signame, #signame, desc }
struct siglist {
	int nr; 
	char *name; 
	char *desc; 
} sigl[] = {
#ifdef SIGHUP
e(SIGHUP, "hangup detected, or death of, controlling terminal"), 
#endif

#ifdef SIGINT
e(SIGINT, "interrupt from tty (usually ^C)"), 
#endif

#ifdef SIGQUIT
e(SIGQUIT, "quit from tty (usually ^\\)"), 
#endif

#ifdef SIGILL
e(SIGILL, "illegal instruction (not reset when caught)"), 
#endif

#ifdef SIGTRAP
e(SIGTRAP, "trace/breakpoint trap (not reset when caught)"), 
#endif

#ifdef SIGABRT
e(SIGABRT, "abort signal from abort(3)"), 
#endif

#ifdef SIGIOT
e(SIGIOT, "IOT trap, synonym for SIGABRT"), 
#endif

#ifdef SIGEMT
e(SIGEMT, "EMT trap"), 
#endif

#ifdef SIGFPE
e(SIGFPE, "floating point exception"), 
#endif

#ifdef SIGKILL
e(SIGKILL, "kill (cannot be caught or ignored)"), 
#endif

#ifdef SIGBUS
e(SIGBUS, "bus error"), 
#endif

#ifdef SIGSEGV
e(SIGSEGV, "segmentation violation: invalid mem reference"), 
#endif

#ifdef SIGSYS
e(SIGSYS, "bad argument to system call"), 
#endif

#ifdef SIGPIPE
e(SIGPIPE, "broken pipe: write to one, with no readers"), 
#endif

#ifdef SIGALRM
e(SIGALRM, "alarm clock from alarm(1)"), 
#endif

#ifdef SIGTERM
e(SIGTERM, "software termination signal from kill"), 
#endif

#ifdef SIGCLD
e(SIGCLD, "synonym for SIGCHLD."), 
#endif

#ifdef SIGPOLL
e(SIGPOLL, "synonym for SIGIO"), 
#endif

#ifdef SIGURG
e(SIGURG, "urgent condition on IO channel"), 
#endif

#ifdef SIGSTOP
e(SIGSTOP, "stop process, via kill"), 
#endif

#ifdef SIGTSTP
e(SIGTSTP, "stop process, via tty"), 
#endif

#ifdef SIGCONT
e(SIGCONT, "continue if stopped"), 
#endif

#ifdef SIGCHLD
e(SIGCHLD, "delivered to parent, on child stop or exit"), 
#endif

#ifdef SIGTTIN
e(SIGTTIN, "tty input, for background processes"), 
#endif

#ifdef SIGTTOU
e(SIGTTOU, "tty output, for background processes"), 
#endif

#ifdef SIGIO
e(SIGIO, "input/output possible signal"), 
#endif

#ifdef SIGXCPU
e(SIGXCPU, "exceeded CPU time limit"), 
#endif

#ifdef SIGXFSZ
e(SIGXFSZ, "exceeded file size limit"), 
#endif

#ifdef SIGVTALRM
e(SIGVTALRM, "virtual time alarm"), 
#endif

#ifdef SIGPROF
e(SIGPROF, "profiling time alarm"), 
#endif

#ifdef SIGWINCH
e(SIGWINCH, "window changed"), 
#endif

#ifdef SIGLOST
e(SIGLOST, "resource lost (eg, file lock lost)"), 
#endif

#ifdef SIGUSR1
e(SIGUSR1, "user defined signal 1"), 
#endif

#ifdef SIGUSR2
e(SIGUSR2, "user defined signal 2"), 
#endif

#ifdef SIGMSG
e(SIGMSG, "HFT input data pending"), 
#endif

#ifdef SIGPWR
e(SIGPWR, "power failure imminent (save your data)"), 
#endif

#ifdef SIGDANGER
e(SIGDANGER, "system crash imminent"), 
#endif

#ifdef SIGMIGRATE
e(SIGMIGRATE, "migrate process to another CPU"), 
#endif

#ifdef SIGPRE
e(SIGPRE, "programming error"), 
#endif

#ifdef SIGGRANT
e(SIGGRANT, "HFT monitor mode granted"), 
#endif

#ifdef SIGRETRACT
e(SIGRETRACT, "HFT monitor mode retracted"), 
#endif

#ifdef SIGSOUND
e(SIGSOUND, "HFT sound sequence has completed"), 
#endif

#ifdef SIGSTKFLT
e(SIGSTKFLT, "stack fault on coprocessor"), 
#endif

#ifdef SIGINFO
e(SIGINFO, "synonym for SIGPWR"), 
#endif

{0, 0, 0}
};

int
cmpf(const void *a, const void *b) {
	const struct siglist *aa = a; 
	const struct siglist *bb = b; 

	return aa->nr - bb->nr; 
}

typedef SIG_RET_TYPE (*handler)(); 
SIG_RET_TYPE dum_catcher() { }

int
main() {
	handler old, cur; 
	int i; 

	qsort(sigl, sizeof(sigl)/sizeof(*sigl)-1, sizeof(*sigl), cmpf); 

	printf("%-15s%-15s%s\n", "   SIGNAL", "CURR CATCHER", "DESCRIPTION"); 
	printf("%-15s%-15s%s\n", "   ------", "------------", "-----------"); 
	for (i=0; sigl[i].name; i++) {
		int l = printf("%2d %-12s", sigl[i].nr, sigl[i].name); 
		old = signal(sigl[i].nr, dum_catcher); 
		cur = signal(sigl[i].nr, dum_catcher); 
		if (cur == dum_catcher)
			l += printf("CTCH"); 
		else
			l += printf("UNCTCH"); 
		if (old == SIG_IGN) l += printf(",IGN"); 
		if (old == SIG_DFL) l += printf(",DFL"); 
		while (l<30) l += printf(" "); 
		printf("%s\n", sigl[i].desc); 
	}
	return 0; 
}
