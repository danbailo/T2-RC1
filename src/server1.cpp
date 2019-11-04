#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define MAX 1500
#define SA struct sockaddr

using namespace std;

//Função para pegar o dia do sistema.
string curr_day(){
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d", &tstruct);
    return buf;
}

//Função para pegar a hora do sistema.
string curr_time(){
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

int new_connection(int port2){
    struct hostent *host = gethostbyname("127.0.0.1");

    if (!host){
        herror("error gethostbyname");
        exit(1);
    }

    struct sockaddr_in paddr = {0};
    socklen_t palen = sizeof(paddr);
    paddr.sin_family = AF_INET;
    paddr.sin_addr.s_addr = *(int32_t *)host->h_addr;
    paddr.sin_port = htons(port2);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0){
        perror("socket error");
        exit(1);
    }
    // else{
    //     printf("\n socket criado com sucesso");
    // }

    int conn = connect(sock, (SA *)&paddr, palen);
    if(conn < 0){
        printf("Por favor, ligue o servidor 2 antes de inicar a execução!\n");
        exit(1);
    }
    return sock;
}

int server(){
    // socket create and verification
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd == -1){
        printf("socket creation failed...\n");
        exit(0);
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
        cerr << "Erro ao criar socket com esta porta! Por favor, tente outra." << endl;
        exit(0);
    }
    return bindStatus;
}

void listener(int serverSd, int clients){
    // Now server is ready to listen and verification
    if ((listen(serverSd, clients)) != 0){
        printf("Listen failed...\n");
        exit(0);
    }
    // printf("Esperando cliente se c");
}

// Function designed for chat between client and server.
int request(int sockfd, int port2){

    char buff[MAX];
    int sock = new_connection(port2);
    int state = 1;

    printf("Aguardando requisição.\n\n");
    // infinite loop for chat
    while(state){
        memset(&buff, 0, sizeof(buff));

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        string op;

        for(int i=0; i < MAX; i++){
            if(buff[i] == '\0' || buff[i] == '\n') break;
            op += buff[i];
        }    

        if(op == "horas"){
            string time_send = "Horário "+ curr_time() +"\n";
            strcpy(buff, time_send.c_str());
            write(sockfd, buff, sizeof(buff));
        }

        else if (op == "dia"){
            string day = "Hoje é dia "+ curr_day() +"\n";
            strcpy(buff,day.c_str());
            write(sockfd, buff, sizeof(buff));
        }
        else if(op == "sair"){
            strcpy(buff, "Obrigado por utilizar nossos serviços!\n");
            write(sockfd, buff, sizeof(buff));
            state = 0;
        }
        else{
            cout << "ENTREI AQUI\n";
            write(sock, buff, sizeof(buff));
            memset(&buff, 0, sizeof(buff));
            read(sock, buff, sizeof(buff));
            write(sockfd, buff, sizeof(buff));            
        }
        printf("Cliente enviou: %s\n", op.c_str());
        printf("Enviado para o cliente: %s\n", buff);
        memset(&buff, 0, sizeof(buff));
    }
    return state;
}

// Driver function
int main(int argc, char *argv[]){

    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 3){
        printf("Por favor, entre com a porta do servidor 1 e servidor 2, respectivamente!\n");
        exit(-1);
    }

	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;    
    pid_t childpid;
    
    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);    
    //Atribui a Porta infomada pelo usuário a esta variável.
    int port2 = atoi(argv[2]);        

    int serverSd = server();

    //Configura o socket;
    int bindStatus = bind(port, serverSd);

    listener(serverSd,10);
    int state = 1;
    while(state){
        newSocket = accept(serverSd, (struct sockaddr*)&newAddr, &addr_size);
        if(newSocket < 0) exit(-1);
        printf("Novo cliente conectado!\n");
        printf("ID:%d\n", ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(serverSd);

			while(state){
                state = request(newSocket, port2);
			}
		}        
    }
    close(newSocket);

}