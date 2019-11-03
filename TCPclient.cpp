#include <stdlib.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <iostream>
 
#define MAX 1500 
// #define PORT 1234 
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

using namespace std;

int sockfd;

void func(int sockfd) { 
    char buff[MAX]; 
    int state = 0;
    int n; 
    while(1){ 
        bzero(buff, sizeof(buff)); 
        // system("clear");
        printf("\nEntre com a opção desejada"); 
        n = 0;

        printf("\n[integrantes] Quais são os integrantes deste grupo?\n");
        printf("[dia] Qual é o dia de hoje?\n");
        printf("[mes] Qual é o dia de hoje?\n");
        printf("[horas] Que horas são?\n");
        printf("[sair] Sair.\n");

        while((buff[n++] = getchar()) != '\n');

        if((strncmp("sair", buff, 4)) == 0){
            state = 1;
        }

        write(sockfd, buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff));
        printf("Resposta: %s\n", buff); 
        if(state) { 
            printf("Client Exit...\n"); 
            exit(0);
        }
        bzero(buff, sizeof(buff)); 

    } 
} 
  
int main(int argc, char *argv[]) { 
    
    int connfd; 
    struct sockaddr_in servaddr, cli; 

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 2){
        perror("Informe uma porta para executar\n");
        exit(-1);
    }

    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);        

    bzero(&servaddr, sizeof(servaddr)); 
    char buffer[MAX] = {0};
    strncpy(buffer, "127.0.0.1", 63);
    struct hostent *pp = gethostbyname(buffer);
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = *(int32_t*)pp->h_addr; 
    servaddr.sin_port = htons(port); 

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    // socket create and varification  
    if (sockfd < 0) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) { 
        printf("connection with the server failed...\n"); 
        exit(1); 
    } 
    else
        printf("connected to the server..\n"); 

    // function for chat 
    func(sockfd); 
  
    // close the socket 
    // close(sockfd); 
} 