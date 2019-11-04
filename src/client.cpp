#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#define MAX 1500 
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

using namespace std;

int client(int port){
    //Configura o socket e as ferramentas de conexão.
    const char* serverIp = "127.0.0.1";
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    //Tenta conectar
    int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0) return 0;
    return clientSd;
}

void request(int sockfd) { 
    char buff[MAX]; 
    int state = 1;
    while(state){ 
        int i = 0; 
        memset(&buff, 0, sizeof(buff));
        printf("\nEntre com a opção desejada\n"); 
        printf("[integrantes] Quais são os integrantes deste grupo?\n");
        printf("[dia] Qual é o dia de hoje?\n");
        printf("[mes] Qual é o dia de hoje?\n");
        printf("[horas] Que horas são?\n");
        printf("[sair] Sair.\n");

        while((buff[i++] = getchar()) != '\n');
        if((strncmp("sair", buff, 4)) == 0) state = 0;
        write(sockfd, buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff));
        printf("Resposta: %s", buff); 
        memset(&buff, 0, sizeof(buff));
    } 
} 
  
int main(int argc, char *argv[]) { 
    
    int connfd; 
    struct sockaddr_in servaddr, cli; 

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 2){
        printf("Por favor, entre somente com a porta do servidor!\n");
        exit(-1); 
    } 
    
    //Atribui o IP e a Porta as seguintes variáveis.
    int port = atoi(argv[1]); 

    int clientSd = client(port);
    if (!clientSd){
        cout << "Erro ao conectar com o socket!" << endl;
        cout << "Verifique se o servidor está ligado!" << endl;
        cout << "Verifique se o IP e a porta do servidor estão corretos!" << endl;    
        exit(-1);    
    }
    cout << "Conectado com o servidor!" << endl;    

    // function for chat 
    request(clientSd); 
  
    // close the socket 
    close(clientSd);
    printf("Conexão fechada!\n"); 
} 