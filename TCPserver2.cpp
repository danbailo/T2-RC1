#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/wait.h>
#include <time.h>
#include <iostream>

#define MAX 1500
#define DEMO_PORT 54000
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

using namespace std;

int sockfd;

struct tm now(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return tm;
}

// Function designed for chat between client and server.
void func(int sockfd){

    char buff[MAX];
    int n;
    struct tm time = now();
    // int month_client = time.tm_mon + 1;
    string month_client = to_string(time.tm_mon + 1);


    // infinite loop for chat
    while(1){
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        if (strncmp("integrantes", buff, 11) == 0){
            char group[] = "O grupo é formado por Daniel, Josué e Beatriz.\n";
            strcpy(buff, "O grupo é formado por Daniel, Josué e Beatriz.\n");

            write(sockfd, group, sizeof(group));
        }

        if (strncmp("mes", buff, 3) == 0){
            // sprintf(buff, "%d\n",month_client);
            string month = "Atualmente estamos no mês "+ month_client +" do ano.\n";
            strcpy(buff, month.c_str());
            write(sockfd, buff, sizeof(buff));
        }

        else if(strncmp("sair", buff, 4) == 0){
            char exit_client[] = "Obrigado por utilizar nossos serviços!";
            write(sockfd, exit_client, sizeof(exit_client));
        }

        else{
            write(sockfd, "Não encontrado\n", sizeof("Não encontrado\n"));
        }   

        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);

        bzero(buff, MAX);
        n = 0;

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0){
            printf("Server Exit...\n");
            close(sockfd);
        }
    }
}

// Driver function
int main(int argc, char *argv[]){
    int connfd, len;
    struct sockaddr_in servaddr, cli;

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 2){
        perror("Informe uma porta para executar\n");
        exit(-1);
    }    

    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);          

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0){
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0){
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, (socklen_t *)&len);
    if (connfd < 0){
        printf("server acccept failed...\n");
        exit(0);
    }
    else{
        printf("server acccept the client...\n");
        printf("Conexão estabelecida com sucesso!\n");

    }

    // Function for chatting between client and server
    while (1){
        func(connfd);
    }
    
    // After chatting close the socket
    close(sockfd);
}