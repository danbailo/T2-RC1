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
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

using namespace std;

int sock;

struct tm now(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return tm;
}

// Function designed for chat between client and server.
void func(int sockfd, int port2){

    char buff[MAX];
    int n;
    char name[80] = {0};
    struct tm time = now();
    int day = time.tm_mday;

    string time_client = to_string(time.tm_hour) + ":" + to_string(time.tm_min) + ":" + to_string(time.tm_sec);

    strncpy(name, "127.0.0.1", 63);

    struct hostent *pp = gethostbyname(name);

    if (!pp){
        herror("error gethostbyname");
        exit(1);
    }

    struct sockaddr_in paddr = {0};
    socklen_t palen = sizeof(paddr);
    paddr.sin_family = AF_INET;
    paddr.sin_addr.s_addr = *(int32_t *)pp->h_addr;
    paddr.sin_port = htons(port2);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0){
        perror("socket error\n");
        exit(1);
    }
    // else{
    //     printf("Socket criado com sucesso!\n");
    // }

    int connect_value = connect(sock, (SA *)&paddr, palen);
    if (connect_value < 0){
        perror("Erro ao estabelecer conexão!\n");
        exit(1);
    }
    else{
        printf("Conexão estabelecida com sucesso!\n");
    }

    // infinite loop for chat
    while(1){
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        if (strncmp("horas", buff, 5) == 0){            
            // sprintf(buff, "%d:%d:%d\n",hours,minutes,seconds);
            // write(sockfd, buff, sizeof(buff));

            string time_send = "Horário "+ time_client +"\n";
            write(sockfd, time_send.c_str(), sizeof(time_send.c_str()));

        }

        else if (strncmp("dia", buff, 3) == 0){
            sprintf(buff, "%d\n",day);
            write(sockfd, buff, sizeof(buff));
        }
        else if (strncmp("sair", buff, 4) == 0){
            
            char exit_client[] = "Obrigado por utilizar nossos serviços!";

            write(sockfd, exit_client, sizeof(exit_client));
            // close(sockfd);
            // close(sock);
            // exit(1);
        }
        else{
            write(sock, buff, sizeof(buff));
            bzero(buff, MAX);
            read(sock, buff, sizeof(buff));

            write(sockfd, buff, sizeof(buff));

            // if msg contains "Exit" then server exit and chat ended.
            if (strncmp("exit", buff, 4) == 0){
                printf("Server Exit...\n");
                break;
            }
        }
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
    }
}

// Driver function
int main(int argc, char *argv[]){
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 3){
        perror("Informe uma porta para executar\n");
        exit(-1);
    }

    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);    

    //Atribui a Porta infomada pelo usuário a esta variável.
    int port2 = atoi(argv[2]);        

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
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    while (1){
        func(connfd, port2);
    }
    // After chatting close the socket
    close(sockfd);
}