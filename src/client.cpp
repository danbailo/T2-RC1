#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

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
    string op;
	char buffer[1500]; 
    int state = 1;
    while(state){ 
		op = "";
        int i = 0; 
        memset(&buffer, 0, sizeof(buffer));
        printf("\nEntre com a opção desejada\n"); 
        printf("[integrantes] Quais são os integrantes deste grupo?\n");
        printf("[dia] Qual é o dia de hoje?\n");
        printf("[mes] Qual é o dia de hoje?\n");
        printf("[horas] Que horas são?\n");
        printf("[sair] Sair.\n");

        while((buffer[i++] = getchar()) != '\n');

        for(int i=0; i < 1500; i++){
            if(buffer[i] == '\0' || buffer[i] == '\n' || buffer[i] == ' ') break;
            op += buffer[i];
        }

        if((strncmp("sair", buffer, 4)) == 0) state = 0;

		if(op == "integrantes" || op == "dia" || op == "mes" || op == "horas"){
			write(sockfd, buffer, sizeof(buffer)); 
			read(sockfd, buffer, sizeof(buffer));
			printf("Resposta: %s", buffer); 
			memset(&buffer, 0, sizeof(buffer));			
		}
		else{
			cout << "Resposta: Não entendi sua solicitação!\n";
			memset(&buffer, 0, sizeof(buffer));	
		}
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

    request(clientSd); 

    close(clientSd);
    printf("Conexão fechada!\n"); 
} 