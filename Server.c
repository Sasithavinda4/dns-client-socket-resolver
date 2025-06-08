#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

void*handle_client(void*arg){
    int newsockfd = *((int*)arg);
    char buffer[255];
    int n;

    while(1){
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if(n<0)
            error("Error on reading.");
        printf("Client : %s\n",buffer);

        bzero(buffer, 255);
        printf("Server : ");
        fgets(buffer, 255, stdin);
        n = write(newsockfd, buffer, strlen(buffer));
        if(n<0)
            printf("Error is on writing.");

        int i = strncmp("Bye", buffer, 3);

        if(i == 0){
            break;
        }
    }
    close(newsockfd);
    pthread_exit(NULL);
}

int main(int argc, char*argv[]){
    if(argc<2){
        fprintf(stderr,"Port number is not provided. Program is terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    pthread_t tid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        error("Error opening the socket.\n");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding failed");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while(1){
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        if(newsockfd < 0){
            error("Error is on accept.");
        }

        if(pthread_create(&tid, NULL, handle_client, (void*)&newsockfd) != 0){
            error("Thread creation is failed.");
        }
    }
    close(sockfd);
    return 0;
}

