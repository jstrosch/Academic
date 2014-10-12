#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

int host_port = 8080;

void usage()
{
	printf("myhttpd, a simple webserver\n");
	printf("ver 1.0, 2014\n");
	printf("Usage Summary: myhttpd -h -p portno -d\n");
	printf("	-h: display the summary\n");
	printf("	-p: change default port number for example: -p 8080\n\n");
}

void* httpHandler(void* socket)
{
	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;

 	int sock = *(int*)socket;

	FILE* fp = fopen("Log.txt", "a");

  	time_t t = time(0);
    struct tm * now = localtime( & t );

	memset(buffer, 0, buffer_len);
	if ((bytecount = recv(sock, buffer, buffer_len, 0))== -1)
	{
		fprintf(fp, "Error receiving data %d\n", errno);
		return 0;
	}

	fprintf(fp,"[REQUEST] Received at %d-%d-%d\n", (now->tm_mon +1), (now->tm_mday), (now->tm_year + 1900));

	fprintf(fp,"Received bytes %d\nReceived string \"%s\"\n", bytecount, buffer);

	strcpy(buffer, "HTTP/1.1 200 OK\nServer: demo\nContent-Length: 37\nConnection: close\nContent-Type: html\n\n<html><body>Welcome to my first page!</body></html>");

	if ((bytecount = send(sock, buffer, strlen(buffer), 0))== -1)
	{
		fprintf(fp,"Error sending data %d\n", errno);
		return 0;
	}

	fprintf(fp,"Sent bytes %d\n", bytecount);

	//Close socket
	close(sock);

	//Clean-up logging
	fflush(fp);
	fclose(fp);

	return 0;
}


int main(int argc, char *argv[])
{
	struct sockaddr_in my_addr;

	pid_t pid = 0;
	pid_t sid = 0;

	// Create child process
	pid = fork();

	if (pid < 0)
	{
		puts("[ERROR] Forking Failed! Exiting...");
		exit(1);
	}
	// PARENT PROCESS. Need to kill it.
	if (pid > 0)
	{
		printf("[SUCCESS] Process ID of child process is %d. Exiting main process.\n", pid);
		// return success in exit status
		exit(0);
	}

	//set new session
	sid = setsid();
	if(sid < 0)
	{
		exit(1);
	}

	//No longer have a terminal, need to close these pipes
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//Log for debuggin
	FILE* fp = fopen ("Log.txt", "a");

	fprintf(fp, "\n\n[DEBUG] *** SERVER STARTED ***\n[DEBUG] Logging Enabled.\n");
	fflush(fp);
	fclose(fp);

	int hsock;
	int * p_int ;
	int err;
	int socketfd;

	socklen_t addr_size = 0;
	int* csock;
	sockaddr_in sadr;
	pthread_t thread_id=0;

	int opt = 0;
	opt = getopt( argc, argv,"dhl:p:r:t:n:s:" );

	while (opt != -1)
	{
		switch (opt)
		{
			case 'h':
                		usage();
                		exit(0);

			case 'p':
				host_port = atoi(optarg);
                  		break;

			case 'r':
				break;
		}

		opt = getopt( argc, argv, "dhl:p:r:t:n:s:" );
	}

	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if (hsock == -1)
	{
		printf("Error initializing socket %d\n", errno);
		exit(-1);
	}

	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;

	if ((setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		printf("Error setting options %d\n", errno);
		free(p_int);
		exit(-1);
	}
	free(p_int);

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);

	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;

	if (bind(hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
	{
		fprintf(stderr,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
		exit(-1);
	}

	if (listen(hsock, 10) == -1 )
	{
		fprintf(stderr, "Error listening %d\n",errno);
		exit(-1);
	}

	//Now lets do the server stuff
	printf("myhttpd server listening on port %d\n", host_port);

	addr_size = sizeof(sockaddr_in);


	pthread_t tid;

	while (true)
	{
		printf("waiting for a connection\n");
		if ((socketfd = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
		{
			printf("Received connection from %s - %d\n",inet_ntoa(sadr.sin_addr), socketfd);
			//Uncomment this for a sequential program
			//httpHandler(socketfd);

			//This code is for threading
			int error = pthread_create(&tid, NULL, httpHandler, (void*) &socketfd);

			//Debug code for development
			if(error)
				puts("ERROR: Problem creating thread.");
			else
				puts("SUCCESS: Thread created");
		}
		else
		{
			fprintf(stderr, "Error accepting %d\n", errno);
		}
	}

	close(socketfd);
}


