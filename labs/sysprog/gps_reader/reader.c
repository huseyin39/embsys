#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>

#include <util.h>

//-----------------------------------------------------------------------------
int main(int argc, char *argv [])
{
    char * port1 = NULL;
    char * port2 = NULL;

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s", asctime (timeinfo) );

    pid_t pid_father = getppid();
    printf("Parent PID : %ld \n" , (long) pid_father) ;

    // parse comand line
    if (argc != 5)
    {
        fprintf(stderr, "Invalid usage: reader -p port_name1 -q port_name2\n");
        exit(EXIT_FAILURE);
    }

    char * options = "p:q:";
    int option;
    while((option = getopt(argc, argv, options)) != -1)
    {
        switch(option)
        {
            case 'p':
                port1 = optarg;
                break;
	    case 'q':
		port2 = optarg;
                break;
            case '?':
                fprintf(stderr, "Invalid option %c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }

    // open serial ports
    int fd1 = open(port1, O_RDWR | O_NOCTTY);
    int fd2 = open(port2, O_RDWR | O_NOCTTY);

    if (fd1 == -1 || fd2 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    tcflush(fd1, TCIOFLUSH);
    tcflush(fd2, TCIOFLUSH);


    // read port
    char buff1[50];
    fd_set fdset1;


    while(1)
    {
        bzero(buff1, sizeof(buff1));

        FD_ZERO(&fdset1);
        FD_SET(fd1, &fdset1);


        select(fd1+1, &fdset1, NULL, NULL, NULL);


        //read port1
        if (FD_ISSET(fd1, &fdset1))
        {
            int bytes = read (fd1, buff1, sizeof(buff1));

            if (bytes > 0)
            {
                printf("Port 1: %s\n", buff1);
                fflush(stdout);
            }
        }
        bzero(buff1, sizeof(buff1));

        FD_ZERO(&fdset1);
        FD_SET(fd2, &fdset1);

        select(fd2+1, &fdset1, NULL, NULL, NULL);
	//read port2
        if (FD_ISSET(fd2, &fdset1))
        {
            int bytes = read (fd2, buff1, sizeof(buff1));

            if (bytes > 0)
            {
                printf("Port 2: %s\n", buff1);
                fflush(stdout);
            }
        }
    }

    // close serial ports
    close(fd1);
    close(fd2);

    exit(EXIT_SUCCESS);
}

	


