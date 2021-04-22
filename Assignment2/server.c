// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    struct passwd* pwd;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";
    char *user = "nobody";

    printf("execve=0x%p\n", execve);
    if(strcmp(argv[0],"child") ==0){
        int duplicate = atoi(argv[1]);
        valread = read(duplicate,buffer,102);
        send(duplicate,hello,strlen(hello),0);
        printf("Hello Message Sent\n");
        exit(0);
    }
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    pid_t child = fork();
    if(child == -1)
    {
        perror("Fork failure");
        exit(EXIT_FAILURE);
    }
    else if(child ==0)
    {
        pwd = getpwnam(user);
        int dup_sock = dup(new_socket);
        if(dup_sock == -1)
        {
            perror("Duplication failed");
        }

        if(pwd == NULL){
            perror("Unable to find user:nobody");
            exit(EXIT_FAILURE);
        }
        setuid(pwd->pw_uid);
        char intToSt[10];
        sprintf(intToSt,"%d",dup_sock);
        char *args[] = {"child",intToSt,NULL};
        execvp(argv[0],args);
    }
    wait(NULL);
    return 0;
}
