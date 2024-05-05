/*
################################################################################
#            MC833 - PROGRAMACAO DE REDES DE COMPUTADORES - 1S_2024            #      
#            PROFº:  EDMUNDO ROBERTO MAUTO MADEIRA                             #
#            NOME:   RAONITON ADRIANO DA SILVA                                 #
#            RA:     186291                                                    #
################################################################################
*/

#include "configAndCRUD.h"

void request(Data *data, int client_socket, pid_t pid, char verificacao[STR]){
    char buffer[MAXSTR];
    char msg[MAXSTR] = "\nESCOLHA UMA OPCAO:\n[1] - Listar todas as musicas e informacoes.\n[2] - Buscar musica por id.\n[3] - Listar musicas por ano de lancamento.\n[4] - Listar musicas por idioma e ano de lancamento.\n[5] - Listar musicas por estilo musical\n[0] - Sair.\n\nDigite a opcao: ";
    char aux1[MAXSTR]; 
    char songInfo[6][MAXSTR];
    char songInfoMsg[6][MAXSTR] = {{"titulo da musica:"},{"nome do autor/cantor da musica:"},{"idioma da musica:"},{"estilo da musica:"},{"ano de lancamento:"},{"refrao da musica:"}};
    int bytes_received;
    int n;
    bool super = false;
    
    //VERIFICACAO DE CLIENT SUPER - se for um client com privilegios o menu exibe mais acoes que o menu para client sem privilegios
    recv(client_socket, buffer, MAXSTR, 0);
    if(strcmp(buffer, "super") == 0){
        super = true;
        strcpy(msg, "\nESCOLHA UMA OPCAO:\n[1] - Listar todas as musicas e informacoes.\n[2] - Buscar musica por id.\n[3] - Listar musicas por ano de lancamento.\n[4] - Listar musicas por idioma e ano de lancamento.\n[5] - Listar musicas por estilo musical\n[6] - Cadastrar nova musica\n[7] - Remover musica pelo id\n[0] - Sair.\nDigite a opcao: ");
    }

    // Receive and send messages
    while(1){
        memset(buffer, 0, MAXSTR);
        strcpy(buffer,msg);
        //sprintf(buffer, "ESCOLHA UMA OPCAO:\n[1] - Buscar musica por id\n[2] - Listar todas as musicas e informacoes.\n[3] - Listar musicas por ano de lancamento\n[4] - Listar musicas por idioma e ano de lancamento\n[5] - Listar musicas por estilo musical\n[0] - Sair\n");
        send(client_socket, buffer, strlen(buffer),0); 
        memset(buffer, 0, MAXSTR);
        bytes_received = recv(client_socket, buffer, MAXSTR, 0);
        if (bytes_received <= 0) {
            break;
        }
        printf("Client pid %d - opt: %s", getpid(), buffer);        //Se buffer == 0 -> Cliente no client side pediu para fechar a conexao
        //printf("strlen: %ld\n", strlen(buffer));

        
        if(strncmp(buffer, "1", strlen(buffer)-1) == 0){                    //BUSCA TODAS AS MUSICAS E INFO
            memset(buffer, 0, MAXSTR);
            printAllDataInfo(data, client_socket);
        
        }else if(strncmp(buffer, "2", strlen(buffer)-1 ) == 0 ){            //BUSCA MUSICA POR UM ID INFORMADO
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o id:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            
            printf("Client pid %d - enviou id: %s", getpid(), buffer);
            
            //printEssencialData(data, int client_socket, int flag, int id, int year, char *lang, char *style)
            printEssencialData(data, client_socket,        0,        atoi(buffer),    0, NULL, NULL); 
            memset(buffer, 0, MAXSTR);
            
            
        }else if(strncmp(buffer, "3", strlen(buffer)-1 ) == 0 ){            //BUSCA MUSICAS POR ANO DE LANCAMENTO
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o ano de lancamento:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            
            printf("Client pid %d - enviou ano: %s\n", getpid(), buffer);
            
            //printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style)
            printEssencialData(data, client_socket, 1, 0, atoi(buffer), NULL, NULL); 
            memset(buffer, 0, MAXSTR);
            
            
        }else if(strncmp(buffer, "4", strlen(buffer)-1 ) == 0){             //BUSCA MUSICAS POR IDIOMA E ANO DE LANCAMENTO
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o idioma:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            strcpy(aux1, buffer);
            printf("Client pid %d - enviou idioma: %s", getpid(), buffer);

            strcpy(buffer, "Digite o ano de lancamento:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            
            printf("Client pid %d - enviou ano: %s", getpid(), buffer);
            
            //printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style)
            printEssencialData(data, client_socket, 2, 0, atoi(buffer), aux1, NULL); 
            memset(buffer, 0, MAXSTR);
            
        }else if(strncmp(buffer, "5", strlen(buffer)-1 ) == 0){             //BUSCA MUSICAS POR ESTILO MUSICAL
            memset(buffer, 0, MAXSTR);

            strcpy(buffer, "Digite o estilo da musica:");
            send(client_socket, buffer, strlen(buffer),0); 
            memset(buffer, 0, MAXSTR);

            recv(client_socket, buffer, MAXSTR, 0);
            
            printf("Client pid %d - enviou ano: %s", getpid(), buffer);
            
            //printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style)
            printEssencialData(data, client_socket, 3, 0, 0, NULL, buffer); 
            memset(buffer, 0, MAXSTR);

        //ESPECIAL OPTIONS
        }else if(super && strncmp(buffer, "6", strlen(buffer)-1 ) == 0){    //CADASTRA UM NOVA MUSICA
            
            int i=0, y;
            memset(buffer, 0, MAXSTR);
            for(i=0; i < 6; i++){
                if(i==0){
                    strcpy(buffer, "\033[1;34m*O id sera gerado automaticamente*\033[0m\nDigite o ");
                    strcat(buffer,songInfoMsg[i]);
                }else{
                    strcpy(buffer, "Digite o ");
                    strcat(buffer,songInfoMsg[i]);
                }

                send(client_socket, buffer, strlen(buffer),0);      // envia a mensagem para o cliente
                
                memset(buffer, 0, MAXSTR);
                recv(client_socket, buffer, MAXSTR, 0);             // recebe a resposta e salva no vetor

                //ESCAPO AS VIRGULAS QUE O USUARIO PODE TER ENTRADO
                y=0;
                while(y < strlen(buffer)){
                    if(buffer[y] == ',')
                        buffer[y] = ' ';
                    y++;
                }

                strncpy(songInfo[i], buffer, strlen(buffer)-1);
                printf("Client pid %d - enviou %s: %s", getpid(), songInfoMsg[i], buffer);
                memset(buffer, 0, MAXSTR);
            }
            //ordem da solicitacoes:        title      , auth       , lang       , style      , year       , chorus - a ordem que o client tem que responder muda um pouco da ordem de exibicao
            //writeSong(data, client_socket,title      , auth       , lang       , year       , style      , chorus)
            data = writeSong(data, client_socket, songInfo[0], songInfo[1], songInfo[2], songInfo[4], songInfo[3], songInfo[5]);

            

            
        }else if(super && strncmp(buffer, "7", strlen(buffer)-1 ) == 0){    //REMOVE UMA MUSICA
            memset(buffer, 0, MAXSTR);
            strcpy(buffer, "Digite o id da musica a ser removida:");
            send(client_socket, buffer, strlen(buffer),0); 

            memset(buffer, 0, MAXSTR);
            recv(client_socket, buffer, MAXSTR, 0);
            printf("Client pid %d - enviou id: %s", getpid(), buffer);
            
            deleteSong(data, client_socket, atoi(buffer));

        }else if(strncmp(buffer, "0", strlen(buffer)-1 ) == 0){             //SAI DO LOOP
            break;
            //memset(buffer, 0, MAXSTR);
            
        }else if(strncmp(buffer, "8", 1) == 0){                             //ESSA OPCAO NAO APARECE NO MENU, MAS SERVE PARA LIMPAR O TERMINAL
            system("clear");
        }else{
            //printf("This is red text\n");
            strcpy(buffer, "\033[1;31mOpcao Invalida ou Nao permitida para esse nivel de privilegio!\033[0m\n");
            send(client_socket, buffer, strlen(buffer), 0);
            
        }
        memset(buffer, 0, MAXSTR);
    }

    
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
    int server_socket, client_socket, reuse=1;
    socklen_t client_addr_len;
    struct sockaddr_in server_Addr, client_Addr;
    char verificacao[STR];
    //SETUP E VARIAVEIS DE CONTROLE DOS DADOS E ARQUIVOS CSV
    createCSVs();
    Data *data = loadSongsData();

    recv(client_socket, verificacao, strlen(verificacao), 0);
    

    //Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Erro: socket()\n");
        exit(EXIT_FAILURE);
    }else
        printf("Socket criado com sucesso.\n");

    //Garantindo que a sctruct estara zerada
    memset(&server_Addr, 0, sizeof(server_Addr));
    
    //Preenchendo as infos em server_Addr
    server_Addr.sin_family = AF_INET;           //
    server_Addr.sin_addr.s_addr = INADDR_ANY;   //
    server_Addr.sin_port = htons(PORT);         //

    //Reuseaddr - permite que use novamente uma porta que foi usada poucos instantes antes
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("Erro: setsockopt(SO_REUSEADDR)\n");
        exit(EXIT_FAILURE);
    }

    //Bind
    if(bind(server_socket, (struct sockaddr*)&server_Addr, sizeof(server_Addr)) == -1){
        perror("Erro: bind().\n");
        exit(EXIT_FAILURE);
    }else
        printf("Bind realizado com sucesso.\n");

    //Listen
    if(listen(server_socket, LISTENQUEUE) == -1){
        perror("Erro: listen().\n");
        exit(EXIT_FAILURE);
    }else
        printf("Server is listening --- Port: %d\n", PORT);

    //Accept
    
    while(1){
        client_addr_len = sizeof(client_Addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_Addr, &client_addr_len);
        if(client_socket == -1){
            perror("Erro: accept().\n");    //Em caso de erro ao aceitar um client, nao eh necessario matar o programa.
            continue;                    //ou seja, o while pode seguir esperando por novos accepts
        }else
            printf("Client Accept realizado.\n");
        
        pid_t pid = fork();
        if(pid == -1){
            perror("Erro: fork()");
            close(client_socket);
            continue;
        }else if(pid == 0){
            close(server_socket);               //fecha o server_socket do fork
            request(data, client_socket, pid, verificacao);
        }else
            close(client_socket);
        
        //printf("Entre qualquer caracter para sair");
        
    }
    
    free(data);
    close(server_socket);
    return 0;
}