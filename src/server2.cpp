#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <map>

using namespace std;

//Função para pegar a data do sistema.
string curr_month(){
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%m", &tstruct);
    return buf;
}

int server(){
    // Cria o socket.
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd == -1){
        printf("Erro ao criar o socket!\n");
        exit(-1);
    }
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
    if (listen(serverSd, clients) != 0){
        printf("Falha ao inicializar o módulo de ouvir os clientes!\n");
        exit(-1);
    }
}

int request(int sockfd, int id_client){
    char buffer[1500];
    int state = 1;
    map<int, string> months;
    months.insert(pair<int, string>(1, "janeiro"));
    months.insert(pair<int, string>(2, "fevereiro"));
    months.insert(pair<int, string>(3, "março"));
    months.insert(pair<int, string>(4, "abril"));
    months.insert(pair<int, string>(5, "maio"));
    months.insert(pair<int, string>(6, "junho"));
    months.insert(pair<int, string>(7, "julho"));
    months.insert(pair<int, string>(8, "agosto"));
    months.insert(pair<int, string>(9, "setembro"));
    months.insert(pair<int, string>(10, "outubro"));
    months.insert(pair<int, string>(11, "novembro"));
    months.insert(pair<int, string>(12, "dezembro"));
    
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

        if (op == "integrantes"){
            strcpy(buffer, "O grupo é formado por Daniel, Josué e Beatriz.\n");
            write(sockfd, buffer, sizeof(buffer));
        }
        else if (op == "mes"){
            int month_number = stoi(curr_month());
            string month = "Atualmente estamos no mês de "+ months.at(month_number) +".\n";
            strcpy(buffer, month.c_str());
            write(sockfd, buffer, sizeof(buffer));
        }
        else if(op == "sair"){
            strcpy(buffer, "Obrigado por utilizar nossos serviços!\n");
            write(sockfd, buffer, sizeof(buffer));
            state = 0;
        }     
        else{
            strcpy(buffer, "Não entendi sua solicitação!\n");
            write(sockfd, buffer, sizeof(buffer));
        }
        printf("Cliente %d solicitou: %s\n",id_client, op.c_str());
        printf("Enviado como resposta para o cliente: %s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
    }
    return state;
}

int main(int argc, char *argv[]){
    //Nessa parte iremos forçar o usuário entrar com a porta do servidor.
    if(argc != 2){
        printf("Por favor, entre somente com uma porta, onde esta deve ser a mesma que foi digitada na execução do servidor 1!\n");
        exit(-1);
    }    
	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;    
    pid_t childpid;
    int state = 1;

    //Atribui a Porta infomada pelo usuário a esta variável.
    int port = atoi(argv[1]);          
    
    int serverSd = server();
    
    int bindStatus = bind(port, serverSd);
    
    listener(serverSd,10);

    printf("Servidor ligado com sucesso!\n");
    printf("Esperando cliente se conectar\n\n");    

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
                state = request(newSocket, id_client);
		}
    }
    close(newSocket);
}