/*
################################################################################
#            MC833 - PROGRAMACAO DE REDES DE COMPUTADORES - 1S_2024            #      
#            PROFº:  EDMUNDO ROBERTO MAUTO MADEIRA                             #
#            NOME:   RAONITON ADRIANO DA SILVA                                 #
#            RA:     186291                                                    #
################################################################################
*/

#include "configAndCRUD.h"

int server_socket, client_socket, reuse=1;
    struct sockaddr_in server_Addr, client_Addr;
    socklen_t client_addr_len;

void request(Data *data, int client_socket, int udp_socket, struct sockaddr_in  client_addr, pid_t pid, char verificacao[STR]){
    char buffer[MAXSTR];
    char msg[MAXSTR] = "\nESCOLHA UMA OPCAO:\n[1] - Listar todas as musicas e informacoes.\n[2] - Listar musicas por estilo musical\n[3] - Fazer download\nDigite a opcao: ";
    char aux1[MAXSTR]; 
    char songInfo[6][MAXSTR];
    char songInfoMsg[6][MAXSTR] = {{"titulo da musica:"},{"nome do autor/cantor da musica:"},{"idioma da musica:"},{"estilo da musica:"},{"ano de lancamento:"},{"refrao da musica:"}};
    int bytes_received;
    int n;
    bool super = false;
    Packet packet;
    

    // Receive and send messages
    //while(1){
        memset(buffer, 0, MAXSTR);
        strcpy(buffer,msg);
        //sprintf(buffer, "ESCOLHA UMA OPCAO:\n[1] - Buscar musica por id\n[2] - Listar todas as musicas e informacoes.\n[3] - Listar musicas por ano de lancamento\n[4] - Listar musicas por idioma e ano de lancamento\n[5] - Listar musicas por estilo musical\n[0] - Sair\n");
        send(client_socket, buffer, strlen(buffer),0); 
        memset(buffer, 0, MAXSTR);
        bytes_received = recv(client_socket, buffer, MAXSTR, 0);
        if (bytes_received <= 0) {
        //    break;
        }
        printf("Client pid %d - opt: %s", getpid(), buffer);        //Se buffer == 0 -> Cliente no client side pediu para fechar a conexao
        //printf("strlen: %ld\n", strlen(buffer));

        
        if(strncmp(buffer, "1", strlen(buffer)-1) == 0){                    //BUSCA TODAS AS MUSICAS E INFO
            memset(buffer, 0, MAXSTR);
            printAllDataInfo(data, client_socket);
        
        }else if(strncmp(buffer, "2", strlen(buffer)-1 ) == 0){             //BUSCA MUSICAS POR ESTILO MUSICAL
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o estilo da musica:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            
            printf("Client pid %d - enviou ano: %s", getpid(), buffer);
            
            //printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style)
            printEssencialData(data, client_socket, 3, 0, 0, NULL, buffer); 
            memset(buffer, 0, MAXSTR);

        //DOWNLOAD
        }else if(strncmp(buffer, "3", strlen(buffer)-1 ) == 0){             //BUSCA MUSICAS POR ESTILO MUSICAL
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o id que deseja baixar: ");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            printf("Client pid %d - enviou id: %s", getpid(), buffer);
            
            //VERIFICA SE A ENTRADA EH UM ID(numero) - caso seja entrado qualquer string diferente de uma string numerica, mostra a mensagem de erro
            if(validaInput(buffer) == 1){
                int x = findToDownload(data, client_socket, atoi(buffer)); // quando converto usando o atoi, caso a pessoa entre 'a' o atoi converte para 0 e isso pode ocasionar o download de uma musica que nao eh a que o client queria, por isso a funcao verifica a entrada
                printf("posicao no vetor data: %d - nome da musica: %s\n", x, data[x].title);
                int len = sizeof(client_addr);
                memset(&buffer, 0, sizeof(buffer));
                printf("\nMessage from UDP client: ");
                n = recvfrom(udp_socket, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&client_addr, &len);
                //puts(buffer);
                char path[STR] = "songs/";
                strcat(path, data[x].title);
                strcat(path,".mp3");
                printf("%s\n", path);
                FILE *fp = fopen(path, "r");
                    
                
                int bytes_read;
                char check[5];
                int y=0;
                packet.id = y;
                while((bytes_read = fread(packet.data, 1, MAXSTR, fp)) > 0) {
                    //printf("bytes received: %d - ", bytes_read);
                    do{    
                        int bytes_sent = sendto(udp_socket, &packet, sizeof(Packet), 0,
                            (struct sockaddr *)&client_addr, sizeof(client_addr));
                        
                        memset(buffer, 0, MAXSTR);
                        recvfrom(udp_socket, buffer, sizeof(buffer), 0,
                                (struct sockaddr *)&client_addr, &len);
                        sprintf(check, "%d", packet.id);
                        printf("check: %s - ", check);
                    
                    
                    }while(strcmp(buffer, check) != 0);
                    
                    y++;
                    memset(packet.data, 0 , MAXSTR);
                    packet.id = y;
                }
                strcpy(packet.data, "EOF_FLAG");
                sendto(udp_socket, &packet, sizeof(Packet), 0,
                            (struct sockaddr *)&client_addr, sizeof(client_addr));
                printf("\nIIII numero de envios: %d\n", y);

                printf("IIIII Transferencia concluida.\n");   
                fclose(fp);
            }else{
                strcpy(buffer, "\033[1;31mId Invalido!\033[0m\n");
                send(client_socket, buffer, strlen(buffer), 0);
            }
        
        }else{
            //printf("This is red text\n");
            strcpy(buffer, "\033[1;31mOpcao Invalida!\033[0m\n");
            send(client_socket, buffer, strlen(buffer), 0);
            
        }
        memset(buffer, 0, MAXSTR);
    //}

    
        // //Caso todos os dados sejam recebidos n==0, caso contrario, continua recebendo
        // while ((n = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        //     buffer[n] = '\0';       //isso faz o buffer iniciar da posicao inicial novamente. 
        //     printf("%s", buffer);   //a saida para o terminal do client fica normal como se 
        //                             //o buffer tivesse conseguido armazenar toda a string
        // }
        // //printf("Mensagem do Cliente: %s  - tam: %ld\n", buffer, strlen(buffer));
        

    
    close(client_socket);
    exit(EXIT_SUCCESS);
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

/************************* main() do server ******************************
*  Funcao que realizara os passos para o servidor aceitar conexoes
*   -   cria o socket
*   -   especifica o ip, a familia e a porta que sera usada
*   -   faz o bind
*   -   faz o listen e seta o numero de clients que o server podera ouvir simultaneamente
*   -   faz os accepts que fica aceitando conexoes de clients num loop
*/
int main(){
    //VARIAVEIS DA CONFIGURACAO DO SERVER
    int server_socket, client_socket, udp_socket,  reuse=1, nready, maxfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    pid_t pid;
    fd_set rset;
    ssize_t n;
    char verificacao[STR];
    char buffer[MAXSTR];
    int bytes_recvd;

    //SETUP E VARIAVEIS DE CONTROLE DOS DADOS E ARQUIVOS CSV
    createCSVs();
    Data *data = loadSongsData();

    
    
 //################## Configuracao Socket TCP ##################
    //Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Erro: socket()\n");
        exit(EXIT_FAILURE);
    }else
        printf("Socket tcp criado com sucesso.\n");

    //Garantindo que a sctruct estara zerada
    memset(&server_addr, 0, sizeof(server_addr));
    
    //Preenchendo as infos em server_Addr
    server_addr.sin_family = AF_INET;           //
    server_addr.sin_addr.s_addr = INADDR_ANY;   //
    server_addr.sin_port = htons(TCP_PORT);         //

    //Reuseaddr - permite que use novamente uma porta que foi usada poucos instantes antes
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("Erro: setsockopt(SO_REUSEADDR)\n");
        exit(EXIT_FAILURE);
    }

    //Bind
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("Erro: bind().\n");
        exit(EXIT_FAILURE);
    }else
        printf("Bind realizado com sucesso.\n");

    //Listen
    if(listen(server_socket, LISTENQUEUE) == -1){
        perror("Erro: listen().\n");
        exit(EXIT_FAILURE);
    }else
        printf("Server tcp is listening --- Port: %d\n", TCP_PORT);

 //################## Configuracao Socket UDP ##################
 
    //Socket UDP
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket == -1){
        perror("Erro: socket()\n");
        exit(EXIT_FAILURE);
    }else
        printf("Socket udp criado com sucesso.\n");

    if(bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("Erro: bind().\n");
        exit(EXIT_FAILURE);
    }else
        printf("Bind realizado com sucesso.\n");

    // // limpa o conjunto de descritores
    // FD_ZERO(&rset);

    // //maximo entre os file descriptor
    // maxfd = max(server_socket, udp_socket) + 1;
    
    while(1){
        // set listenfd and udpfd in readset
        // FD_SET(server_socket, &rset);
        // FD_SET(udp_socket, &rset);
        
        // //seleciona o socket que esta pronto
        // nready = select(maxfd, &rset, NULL, NULL, NULL); 

        // // if tcp socket is readable then handle
        // // it by accepting the connection
        // if(FD_ISSET(server_socket, &rset)){
             len = sizeof(client_addr);
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
            if(client_socket == -1){
                perror("Erro: accept().\n");    //Em caso de erro ao aceitar um client, nao eh necessario matar o programa.
                continue;                    //ou seja, o while pode seguir esperando por novos accepts
            }else
                printf("Client Accept realizado.\n");
            
            pid = fork();
            if(pid == -1){
                perror("Erro: fork()");
                close(client_socket);
                continue;
            }else if(pid == 0){
                close(server_socket);               //fecha o server_socket do fork
                request(data, client_socket, udp_socket, client_addr, pid, verificacao);
            }else
                close(client_socket);
        
            
        //}

        // if udp socket is readable receive the message.
        // if(FD_ISSET(udp_socket, &rset)){
        //     len = sizeof(client_addr);
        //     memset(&buffer, 0, sizeof(buffer));
        //     printf("\nMessage from UDP client: ");
        //     n = recvfrom(udp_socket, buffer, sizeof(buffer), 0,
        //                  (struct sockaddr *)&client_addr, &len);
        //     puts(buffer);
        //     char path[STR] = "songs/";
        //     strcat(path,buffer);
        //     FILE *fp = fopen(path, "r");
        //     printf("%s\n", path);
                
                
        //     printf("Arquivo realmente existeeee haha\n");   
        //     int x=0;
        //     while((bytes_recvd = fread(buffer, 1, MAXSTR, fp)) > 0) {
             
        //         int bytes_sent = sendto(udp_socket, (const char *)buffer, bytes_recvd, 0,
        //             (struct sockaddr *)&client_addr, sizeof(client_addr));
               
        //         x++;
        //     }
        //     printf("\nnumero de envios: %d\n", x);
        
        //     printf("Transferencia concluida.\n");   
        //     fclose(fp);
        // }
        
    }
    
    free(data);
    close(server_socket);
    return 0;
}