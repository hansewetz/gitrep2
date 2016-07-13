// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

/*
Test program showing how to wait for a signal (in this case SIGCHLD) with a timeout
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
 
static pid_t fork_child (void)
{
	int p = fork ();
	if (p == -1) {
		perror ("fork");
		exit (1);
	}
	if (p == 0) {
		puts ("child: sleeping...");
		sleep (2);
		puts ("child: exiting");
		exit (0);
	}
	return p;
}
// test program
int main (int argc, char *argv[])
{
	pid_t pid;
	pid = fork_child ();

	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL );
        signal(SIGCHLD,[](int){}); // need dummy signal handler or sigtimedwait() won't work

	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;
	do {
		if (sigtimedwait(&mask, NULL, &timeout) < 0) {
			if (errno == EINTR) {
                                printf("EINTR\n");
				continue;
			}
			else if (errno == EAGAIN) {
				printf ("Timeout, killing child\n");
			}
			else {
				perror ("sigtimedwait");
				return 1;
			}
		}else{
                  printf("got signal ...\n");
                }
		break;
	} while (1);

	if (waitpid(pid, NULL, 0) < 0) {
		perror ("waitpid");
		return 1;
	}
	return 0;
}
