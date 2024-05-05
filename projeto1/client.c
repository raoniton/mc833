/*
################################################################################
#            MC833 - PROGRAMACAO DE REDES DE COMPUTADORES - 1S_2024            #      
#            PROFº:  EDMUNDO ROBERTO MAUTO MADEIRA                             #
#            NOME:   RAONITON ADRIANO DA SILVA                                 #
#            RA:     186291                                                    #
################################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_addr()

#define MAXSTR 10240
#define STR 1024
#define PORT 9877

//Funcao auxiliar, simplesmente para receber o dadao do servidor e tratar o print
void recvAux(int client_socket, char buffer[MAXSTR]){
    recv(client_socket, buffer, MAXSTR,0); 
    printf("%s ", buffer);    
    memset(buffer, 0, MAXSTR);
}

//Funcao auxiliar, simplesmente receber a entrada que o client fez no terminal e enviar para o sevidor
void sendAux(int client_socket, char buffer[MAXSTR]){
    fgets(buffer, MAXSTR, stdin);   
    //buffer[strlen(buffer)] = '\n';
    send(client_socket, buffer, strlen(buffer),0);          // envia a opcao escolhida para o servidor
    memset(buffer, 0, MAXSTR);
}


/***************************** printEssencialData() **********************************
*  Funcao que recebe e envia dados para o servidor
*  Argumentos:
*   -   client_socket - numero do socket do client que o server fez connect
*   -   verificacao - eh um vetor, que usamos para avaliar o nivel de privilegio do client, se ele eh super ou common
*/
void clientRequest(int client_socket, char *verificacao){
	char buffer[MAXSTR];
	int n;
    bool super = false;
    //envio de verificacao
    send(client_socket, verificacao, strlen(verificacao),0);
    if(strstr(verificacao, "super") != NULL)
        super = true;

	while(1){
		memset(buffer, 0, MAXSTR);                              // limpa o buffer
		recv(client_socket, buffer, MAXSTR,0);                  // recebe o menu no buffer
        printf("%s", buffer);                                   // print o menu no terminal do client
		memset(buffer, 0, MAXSTR);                              // limpa o buffer apos o uso
        
        
		// copio a mensagem do servidor para o buffer
        n = 0; 
		while ((buffer[n++] = getchar()) != '\n'); 
        send(client_socket, buffer, strlen(buffer),0);          // envia a opcao escolhida para o servidor
        
        //caso a entrada for um zero, encerra a conexao do lado do cliente
        if(strcmp(buffer, "0\n") == 0){                         
            break;

        }else if(strcmp(buffer, "2\n") == 0 || strcmp(buffer, "3\n") == 0 || strcmp(buffer, "5") == 0){      // strlen(buffer) - 1, serve para ignorar o \n na comparacao
            
            recvAux(client_socket, buffer);
            sendAux(client_socket, buffer);
            
        }else if(strncmp(buffer, "4", 1) == 0){                   // strncamp (buffer, comparo com essa entrda, 1), uso 1 para ignorar o \n na comparacao
            recvAux(client_socket, buffer);
            sendAux(client_socket, buffer);
            
            recvAux(client_socket, buffer);
            sendAux(client_socket, buffer);
            
        }else if(super && strncmp(buffer, "6", strlen(buffer)-1 ) == 0){ 
            for(int i=0; i < 6; i++){
                recvAux(client_socket, buffer);                     //recebe a mensagem
                memset(buffer, 0, MAXSTR);                          // limpa o buffer apos o uso
                
                fgets(buffer, MAXSTR, stdin);                       // le o que client escreve no terminal e envia
                send(client_socket, buffer, strlen(buffer),0);      // envia as infos da musica, sendo elas: title, auth, lang, style, chorus e year;
                memset(buffer, 0, MAXSTR);
            }
            
        }else if(super && strncmp(buffer, "7", 1) == 0){ 
            recvAux(client_socket, buffer);
            sendAux(client_socket, buffer);
        }else if(strncmp(buffer, "8", 1) == 0){                     //Essa opcao nao aparece no menu, mas serve para limpar o terminal
            system("clear");
        }

        memset(buffer, 0, MAXSTR);
	}
    close(client_socket);
}

/************************* main() do client ******************************
*  Funcao que realizara os passos para se conectar ao servidor
*   -   cria o socket
*   -   especifica o ip, a familia e a porta que sera usada
*   -   faz o connect
*   -   e chama a funcao que tratara os send e recv
*  Argumentos:
*   -   argc - numero de strings digitadas na linha de comando
*   -   argv - vetor de char que armazena as strings entradas na linha de comando
*/
int main(int argc, char *argv[]){
    int client_socket, n;
    struct sockaddr_in server_Addr;
    char buffer[MAXSTR], ip[STR], verificacao[STR];

    //Verifico se foi digitado o IP na linha de comando
    if(argv[1]==NULL){
        printf("uso para cliente comum: ./client IP\n");
        printf("uso para cliente super: ./client IP super\n");
        printf("substitua IP pelo IP da maquina onde ./server esta rodando\n");
        printf("ou por 127.0.0.1 caso esteja rodando na mesma maquina\n");
        exit(0);
    }else{
        strcpy(ip, argv[1]);
    }
    
    
    //Socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1){
        perror("Erro: socket()\n");
        exit(1);
    }

    
    //Insere as infos nos campos da struct sockaddr_in server_Addr
    server_Addr.sin_addr.s_addr = inet_addr(ip); 
    server_Addr.sin_family = AF_INET;
    server_Addr.sin_port = htons(PORT);         

    //Connect
    if(connect(client_socket, (struct sockaddr*)&server_Addr, sizeof(server_Addr)) == -1){
        perror("Erro: connect(): ");
        exit(1);
    }
    
    printf("Cliente conectado --- Port: %d\n", PORT);
    
    //Funcao que fara o cliente exibir as respostas do server
    if(argv[2]!=NULL){
        //Para rodar como super client, ex: ./client 000.000.000.000 super
        //essa verificacao server para o servidor fazer uma simples identificacao do tipo de client
        if(strcmp(argv[2], "super") == 0){
            printf("Nivel de privilegio: %s\n", argv[2]);
            clientRequest(client_socket, "super");
        }
    }else{
        printf("Nivel de privilegio: COMUM\n");
        clientRequest(client_socket, "common");
    }
        
    
    close(client_socket);
    return 0;
}