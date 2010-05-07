
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// Send CA to stdout, and return the exit code
int send_ca(void){
    char buf[4048];
    int nbytes;
    int fd = open("ca-cert.pem", O_RDONLY);
    if (fd < 0) {
	fprintf(stderr, "Config error: can't find ca-cert.pem");
	return 1;
    }

    while(0 < (nbytes = read(fd, buf, sizeof(buf)))){
	if(nbytes != write(1, buf, nbytes)){
	    fprintf(stderr, "Write error: can't send ca-cert.pem");
	    close(fd);
	    return 1;
	}
    }
    close(fd);

    return 0;
}


int main (int argc, char * argv[], char * envp[]){

    int ch;

    char cadir[1024];
    char * procname = strrchr(cadir, '/');
    if(procname){
        procname[0] = '\0';
    }

    ssize_t err = readlink("/proc/self/exe", cadir, sizeof(cadir)-1);
    if(err<0){
        fprintf(stderr, "Couldn't find executable path: %m\n. exiting");
        exit(1);
    } else {
	/* terminate the string */
	cadir[err] = '\0';
    }

    if(chdir(cadir) != 0){
	fprintf(stderr, "Unable to chdir(%s)\n", cadir);
	exit(1);
    }

    /* options descriptor */
    static struct option longopts[] = {
	{ "get-ca",      no_argument,            NULL,           'c' },
	{ NULL,         0,                      NULL,           0 }
    };

    while ((ch = getopt_long(argc, argv, "c", longopts, NULL)) != -1){
	switch (ch) {
	    case 'c':
		exit(send_ca());
	}
    }

    
    /*
    printf("Real UID\t= %d\n", getuid());
    printf("Effective UID\t= %d\n", geteuid());
    printf("Real GID\t= %d\n", getgid());
    printf("Effective GID\t= %d\n", getegid());
	*/

    setreuid(geteuid(), geteuid());

    char * const new_argv[] = {
    	"/usr/bin/openssl", 
	"ca", 
	"-batch",
	"-config",
	"ca-ssl.conf",
	"-in", "/dev/stdin", "-out", "/dev/stdout", NULL};

    execve("/usr/bin/openssl", new_argv, envp);

    // If we get here, return error
    fprintf(stderr, "Error execing openssl\n");
    exit(1);
}
