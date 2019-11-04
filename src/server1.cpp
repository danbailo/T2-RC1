// https://stackoverflow.com/questions/1790750/what-is-the-difference-between-read-and-recv-and-between-send-and-write
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX 1500
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */


using namespace std;

//Função para pegar o dia do sistema.
string curr_day(){
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d", &tstruct);
    return buf;
}

//Função para pegar a hora do sistema.
string curr_time(){
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

int new_connection(int port2){
    struct hostent *host = gethostbyname("127.0.0.1");
    if (!host){
        printf("Erro ao pegar o host!\n");
        exit(-1);
    }
    struct sockaddr_in paddr = {0};
    socklen_t palen = sizeof(paddr);
    paddr.sin_family = AF_INET;
    paddr.sin_addr.s_addr = *(int32_t *)host->h_addr;
    paddr.sin_port = htons(port2);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0){
        printf("Erro ao estabeler o socket!\n");
        exit(-1);
    }
    int conn = connect(sock, (SA *)&paddr, palen);
    if(conn < 0){
        printf("Por favor, inicie o servidor 2 com a mesma porta que foi passado na linha de comando antes de inicar a execução!\n");
        printf("Por favor, aperte CTRL+C para encerrar o programa!\n");
        exit(-1);
    }
    return sock;
}

int server(){
    // socket create and verification
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd == -1){
        printf("Erro ao criar o socket!\n");
        exit(-1);
    }
    printf("Servidor ligado com sucesso!\n");
    printf("Esperando cliente se conectar\n\n");    
    return serverSd;
}

int bind(int port, int serverSd){
    //Configura o socket e as ferramentas de conexão.
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //Vincula o socket com o endereço local.
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if(bindStatus < 0){
        printf("Erro ao criar socket com esta porta! Por favor, tente outra.\n");
        exit(-1);
    }
    return bindStatus;
}

void listener(int serverSd, int clients){
    // Now server is ready to listen and verification
    if ((listen(serverSd, clients)) != 0){
        printf("Falha ao inicializar o módulo de ouvir os clientes!\n");
        exit(0);
    }
}

// Function designed for chat between client and server.
int request(int sockfd, int port2, int id_client){
    int sock = new_connection(port2);
    char buffer[MAX];
    int state = 1;

    printf("Aguardando requisição.\n\n");
    // infinite loop for chat
    while(state){
        memset(&buffer, 0, sizeof(buffer));
        // read the message from client and copy it in buffer
        read(sockfd, buffer, sizeof(buffer));
        string op = "";
        for(int i=0; i < MAX; i++){
            if(buffer[i] == '\0' || buffer[i] == '\n') break;
            op += buffer[i];
        }    
        if(op == "horas"){
            string time_send = "Horário "+ curr_time() +".\n";
            strcpy(buffer, time_send.c_str());
            write(sockfd, buffer, sizeof(buffer));
        }
        else if (op == "dia"){
            string day = "Hoje é dia "+ curr_day() +".\n";
            strcpy(buffer,day.c_str());
            write(sockfd, buffer, sizeof(buffer));
        }
        else if(op == "sair"){
            strcpy(buffer, "Obrigado por utilizar nossos serviços!\n");
            write(sockfd, buffer, sizeof(buffer));
            state = 0;
        }
        else{
            write(sock, buffer, sizeof(buffer));
            memset(&buffer, 0, sizeof(buffer));
            read(sock, buffer, sizeof(buffer));
            write(sockfd, buffer, sizeof(buffer));          
        }
        printf("Cliente %d solicitou: %s\n", id_client, op.c_str());
        printf("Enviado como resposta para o cliente: %s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
    }
    return state;
}

// Driver function
int main(int argc, char *argv[]){

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 3){
        printf("Por favor, entre somente com a porta do servidor 1 e servidor 2, respectivamente!\n");
        exit(-1);
    }
	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;    
    pid_t childpid;
    int state = 1;
    
    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);    
    //Atribui a Porta infomada pelo usuário a esta variável.
    int port2 = atoi(argv[2]);
           
    int serverSd = server();

    //Configura o socket;
    int bindStatus = bind(port, serverSd);

    listener(serverSd,10);

    int id_client = 0;
    while(state){
        newSocket = accept(serverSd, (struct sockaddr*)&newAddr, &addr_size);
        id_client++;
        if(newSocket < 0) exit(-1);
        printf("Novo cliente conectado!\n");
        printf("ID:%d\n", id_client);
		if((childpid = fork()) == 0){
			close(serverSd);
			while(state) 
                state = request(newSocket, port2, id_client);
		}        
    }
    close(newSocket);
}