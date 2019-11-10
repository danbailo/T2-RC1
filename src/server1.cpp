// https://stackoverflow.com/questions/1790750/what-is-the-difference-between-read-and-recv-and-between-send-and-write

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

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
    //Configura o socket e as ferramentas de conexão.
    const char* serverIp = "127.0.0.1";
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port2);
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);

    //Tenta conectar
    int status = connect(serverSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0) return 0;
    return serverSd;
}

int server(){
    // Cria o socket.
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
    // Server está pronto para ouvir os clientes.
    if ((listen(serverSd, clients)) != 0){
        printf("Falha ao inicializar o módulo de ouvir os clientes!\n");
        exit(0);
    }
}

//Chat entre cliente e servidor
int request(int sockfd, int port2, int id_client){
    int sock = new_connection(port2);
    char buffer[1500];
    int state = 1;

    printf("Aguardando requisição.\n\n");

    while(state){
        memset(&buffer, 0, sizeof(buffer));

        //Lê a mensagem do cliente e copia ela no buffer
        read(sockfd, buffer, sizeof(buffer));
        string op;
        for(int i=0; i < 1500; i++){
            if(buffer[i] == '\0' || buffer[i] == '\n' || buffer[i] == ' ') break;
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
            char buffer_aux[] = "sair";
            write(sock, buffer_aux, sizeof(buffer_aux));
            read(sock, buffer_aux, sizeof(buffer_aux));            
        }
        else{
            if(op == ""){
                memset(&buffer, ' ', sizeof(buffer));
                write(sock, buffer, sizeof(buffer));
                read(sock, buffer, sizeof(buffer));
                write(sockfd, buffer, sizeof(buffer));          
            } 
            else{
                write(sock, buffer, sizeof(buffer));
                memset(&buffer, 0, sizeof(buffer));
                read(sock, buffer, sizeof(buffer));
                write(sockfd, buffer, sizeof(buffer));          
            }          
        }
        printf("Cliente %d solicitou: %s\n", id_client, op.c_str());
        printf("Enviado como resposta para o cliente: %s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
    }
    return state;
}

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