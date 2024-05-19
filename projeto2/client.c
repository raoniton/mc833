/*
################################################################################
#            MC833 - PROGRAMACAO DE REDES DE COMPUTADORES - 1S_2024            #      
#            PROFº:  EDMUNDO ROBERTO MAURO MADEIRA                             #
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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_addr()

#define MAXSTR 10240
#define STR 1024
#define PORT 9877

// Struct na qual tanto o client como o server tem, sendo que o server envia os packets
// e o client precisa saber o formato para poder utilizar os dados
typedef struct Packet{
    int id;
    char data[MAXSTR];
}Packet;

//Funcao auxiliar, simplesmente para receber o dadao do servidor e tratar o print
void recvAux(int client_socket, char buffer[MAXSTR]){
    recv(client_socket, buffer, MAXSTR,0); 
    printf("%s", buffer);    
    memset(buffer, 0, MAXSTR);
}

//Funcao auxiliar, simplesmente receber a entrada que o client fez no terminal e enviar para o sevidor
void sendAux(int client_socket, char buffer[MAXSTR]){
    fgets(buffer, MAXSTR, stdin);   
    //buffer[strlen(buffer)] = '\n';
    send(client_socket, buffer, strlen(buffer),0);          // envia a opcao escolhida para o servidor
    memset(buffer, 0, MAXSTR);
}

int itHasSubString(char buffer[], char substring[]){
    int bufLen = strlen(buffer), subLen = strlen(substring), i=0, j=0;
    if(subLen > bufLen)
        return 0;
    
    for(i = 0; i < bufLen; i++){
        //printf("Comparando: %c - %c\n", buffer[i], substring[j]);
        if(buffer[i] == substring[j]){
            j++;
            if(j == subLen) return 1;
        }else{
            j=0;
        }
    }
    return 0;
}

/***************************** printEssencialData() **********************************
*  Funcao que recebe e envia dados para o servidor
*  Argumentos:
*   -   client_socket - numero do socket do client que o server fez connect
*/
void clientRequest(int client_socket){
	char buffer[MAXSTR];
	int n;
    //fd_set rfds;
    //struct timeval tv;
    //int retval;
    Packet packet;
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
            //recvAux(client_socket, buffer);
            break;

        }else if(strcmp(buffer, "1\n") == 0){                         
            //recvAux(client_socket, buffer);

        }else if(strcmp(buffer, "2\n") == 0){      // strlen(buffer) - 1, serve para ignorar o \n na comparacao
            
            recvAux(client_socket, buffer);
            sendAux(client_socket, buffer);
            //recvAux(client_socket, buffer);
            
        }else if(strcmp(buffer, "3\n") == 0){                   // strncamp (buffer, comparo com essa entrda, 1), uso 1 para ignorar o \n na comparacao
           
            recvAux(client_socket, buffer);         // recebe a mensagem: "Digite o id que deseja baixar:"
            sendAux(client_socket, buffer);         // envia o id
            
            recv(client_socket, buffer, MAXSTR,0);  // recebe a mensagem de retorno
            
            
            // Verifica se a entrada eh valida
            if(itHasSubString(buffer, "nao encontrado") || itHasSubString(buffer, "Invalido")){
                printf("%s", buffer);
                memset(buffer, 0, MAXSTR);;
                send(client_socket, "teste", strlen("teste"),0);
            
            // Se o If falhar, entao eh uma entrada valida, entao entra no else onde 
            // eh feita a preparacao do diretorio e eh criado um arquivo para receber os dados
            }else{
                printf("\n------------------------\n%s\n------------------------\n", buffer);
                //Prepara a pasta e o caminho para salvar a musica
                system("mkdir -p downloads/");                      // cria a pasta de downloads caso ainda nao exista
                char path[MAXSTR] = "downloads/", songName[STR];    // caminho 
                strcat(buffer, ".mp3");                             // cria o nome do arquivo que desajamos
                strcpy(songName, buffer);                           // salvo o nome do arquivo na variavel songName
                printf("Nome do arquivo: %s\n", buffer);            
                strcat(path, buffer);                               // concatena no caminho o nome do arquivo, downloads/NomeDoArquivo.mp3
                //printf("Caminho do arquivo: %s\n", path);
                FILE *fp = fopen(path,"w");                         // cria o arquivo dentro da pasta downloads/

                //VAMOS RECEBER O ARQUIVO UTILIZANDO SOCKET UDP
                //################## Configuracao Socket UDP ##################
                int udp_socket;
                struct sockaddr_in server_udp_addr, client_udp_addr;
                socklen_t client_udp_addr_len = sizeof(struct sockaddr);
                

                //Socket UDP
                udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
                if(udp_socket == -1){
                    perror("Erro: socket()\n");
                    exit(EXIT_FAILURE);
                }

                //Garantindo que a sctruct estara zerada
                memset(&server_udp_addr, 0, sizeof(server_udp_addr));

                //Preenchendo as infos em server_Addr
                server_udp_addr.sin_family = AF_INET;           //
                server_udp_addr.sin_addr.s_addr = INADDR_ANY;   //
                server_udp_addr.sin_port = htons(PORT);         //
                
                memset(buffer, 0, MAXSTR);
                int bytes_sent = sendto(udp_socket, "Ready", strlen("Ready"), 0, 
                        (struct sockaddr *)&server_udp_addr, client_udp_addr_len);
                if (bytes_sent < 0) {
                    perror("Error sending message");
                    exit(EXIT_FAILURE);
                }
                
                //NESSA PARTE EH RECEBIDA A MUSICA, FAZENDO UMA VERIFICACAO DE ID PARA GARANTIR O RECEBIMENTO DO PACOTE CORRETO
                int check=-1;   // Inicializo o valor com -1 eh como se fosse um 'checksum' simplificado
                while (1) {
                    // Recebe um pacote, que conte o id e o dado
                    client_udp_addr_len = sizeof(struct sockaddr);
                    int bytes_received = recvfrom(udp_socket, (char *)&packet, MAXSTR, 0, (struct sockaddr *)&server_udp_addr, &client_udp_addr_len);
                    
                    // essa eh uma flag criada para indicar que o servidor encerrou o envio
                    if(strstr(packet.data, "EOF_FLAG")){
                        printf("\033[1;32mDownload de %s concluido\033[0m\n", songName);
                        break;
                    }
                    
                    // A soma eh feita de modo a sempre dar 1, na primeira iteracao check == -1, e o primeiro packet.id == 0, (0 - (-1) == 1
                    if( (packet.id - check) == 1 ){
                        //printf("%d - strlen(packet.data):%ld\n", packet.id, strlen(packet.data));
                        memset(buffer, 0, MAXSTR);
                        sprintf(buffer, "%d", packet.id);
                        
                        //ESSE SEND FUNCIONA COMO ACK - o server recebera o id do packet recem enviado, se for igual ao id que o server enviou, entao pode enviar o proximo pacote
                        bytes_sent = sendto(udp_socket, buffer, strlen(buffer), 0, 
                                (struct sockaddr *)&server_udp_addr, client_udp_addr_len);
                        
                        //Devido a checagem, eh garantido que recebemos a posicao correta do pacote e por isso podemos escrever no arquivo
                        fwrite(packet.data, 1, bytes_received, fp);     //escreve no arquivo os dados recebidos no packet
                        check = packet.id;                              // atualizo o valor de check
                        
                    }else{
                        //caso o ACK nao chegue no server, ele vai tentar enviar novamente o mesmo packet, que eh o caso desse if, ai envio ack de "chegou"
                        //mas dessa vez nao fazemos nenhuma escrita no arquivo
                        bytes_sent = sendto(udp_socket, buffer, strlen(buffer), 0, 
                                (struct sockaddr *)&server_udp_addr, client_udp_addr_len);
                                printf("check: %d - ", packet.id);
                        
                    }
                }
                //printf("\nnumero de recebimentos: %d\n", x);
                buffer[0] = '\0';
                songName[0] = '\0';
                path[0] = '\0';
                fclose(fp);
                close(udp_socket);

            }
            
        }else if(strncmp(buffer, "4", 1) == 0){                     //Essa opcao nao aparece no menu, mas serve para limpar o terminal
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
        printf("uso: ./client IP\n");
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
    clientRequest(client_socket);
    
    close(client_socket);
    return 0;
}