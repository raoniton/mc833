#include "headerIncludes.h"

/******************************* createCSVs() ************************************
*  Funcao responsavel por criar os arquivos .csv que serao usados para armazenar os dados das musicas.
*  Os arquivos sao criados no diretorio do executavel 
*   -   songsData.csv:  armazena todas as informacoes de uma musica
*   -   nSongs.csv:     armazena o numero de musicas cadastradas
*   -   songId.csv:     armazena um numero que vai incrementando cada vez que uma musica eh cadastrada
*/
void createCSVs(){
    char csv_ColsNames[STR], csv_Value[STR];
    FILE *fp;
    //Cria o 1o arquivo que usaremos como banco de dados
    if(access("songsData.csv", F_OK) == -1){
        strcpy(csv_ColsNames, "Id,Titulo,Interprete,Idioma,Tipo,Refrao,Ano");
        fp = fopen("songsData.csv", "w+");
        fprintf(fp, "%s\n", csv_ColsNames);
        fclose(fp);
    }
    
    //Cria o 2o arquivo que usaremos como banco que mantem a quantidade de dados
    if(access("nSongs.csv", F_OK) == -1){
        strcpy(csv_ColsNames, "Quantidade de dados");
        fp = fopen("nSongs.csv", "w+");
        fprintf(fp, "%s\n%s\n", csv_ColsNames, "0");
        fclose(fp);
    }

    //Cria o 3o arquivo que usaremos com banco que mantem um valor de id
    //esse id sera usado quando criarmos uma nova musica, o valor de id sera unico
    if(access("songId.csv", F_OK) == -1){
        strcpy(csv_ColsNames, "SongId");
        fp = fopen("songId.csv", "w+");
        fprintf(fp, "%s\n%s\n", csv_ColsNames, "0");
        fclose(fp);
    }

}

/******************************* writeSong() *************************************
*  Funcao responsavel por escrever os dados de uma musica que esta sendo cadatrada, a musica eh escrita no arquivo Songs.csv e carregada no 
*   o vetor de struct *data que armazena os dados na memoria do programa enquanto esta rodando.
*  ----> Somente Client super consegue cadastrar novas musicas!
*  Argumentos: 
*   -   *data - ponteiro de struct data
*   -   client_socket - numero do socket do client que o server fez accept
*   -   title, auth, lang, year, style, chorus - valores que sao passados para a funcao realizar a escrita e load para a memoria do programa.
*/
Data*   writeSong(Data *data, int client_socket, char title[STR], char auth[STR], char lang[STR], char year[STR], char style[STR], char chorus[MAXSTR]){
    int n = nSongsFromCSV(), newN;
    char buffer[MAXSTR];
    FILE *fp = fopen("songsData.csv", "a+");
    
    updateNSongsToCSV(1);               //atualizo a quantidade de musicas em + uma unidade no arquivo nSongs

    if(n == 0){
        data = (Data *)malloc(1* sizeof(Data));
    }else if(n >= 1){
        newN = n;
        data = (Data *)realloc(data, (newN+1) * sizeof(Data));
    }
    
    //gera um numero que sera usado como id da musica
    int id = generateId();
    char s_id[STR];
    sprintf(s_id, "%d", id);
    
    //escrevo no arquivo csv
    fprintf(fp, "%s,%s,%s,%s,%s,%s,%s\n", s_id, title, auth, lang, style, chorus, year);

    //escrevo na nova posicao alocada do vetor de struct data
    data[n].id = id;
    data[n].year = atoi(year);
    strcpy(data[n].title, title);
    strcpy(data[n].auth,  auth);
    strcpy(data[n].lang,  lang);
    strcpy(data[n].style, style);
    strcpy(data[n].chorus, chorus);
    fclose(fp);

    //envia uma mensagem para o cliente
    strcpy(buffer, "\033[1;32mA musica foi cadastrada*\033[0m\n\n");
    send(client_socket, buffer, strlen(buffer),0); 
    memset(buffer, 0, MAXSTR);
    return data;

}

/******************************* deleteSong() *************************************
*  Funcao responsavel por remover uma musica do "banco de dados", com base na chave que eh o id da musica
*  ----> Somente Client super consegue deletar musicas!
*  Argumentos: 
*   -   *data - ponteiro de struct data
*   -   client_socket - numero do socket do client que o server fez accept
*   -   k - id que o client deseja remover
*/
Data* deleteSong(Data *data, int client_socket, int k){
    char buffer[MAXSTR], aux[STR];
    int pos=-1, i, nSongs = nSongsFromCSV();
    buffer[0] = '\n';

    sprintf(buffer, "\nTentativa de exclusao\n");
    strcat(buffer, "--------------------------------------\n");
    //se o numero de musicas > 0 busca o id deseja, caso contrario retorna
    if(nSongs > 0){
        for(i=0; i < nSongs; i++){
            if(data[i].id == k)
                pos = i;
        }
        if(pos == -1){
            sprintf(aux, "id '%d' nao encontrado!\n", k);
            strcat(buffer, aux);

        }else{
            //remove do vetor sobrescrevendo os dados
            for(i=pos; i < nSongs-1; i++){
                data[i] = data[i+1];
            }
            //atualiza o numero de musicas do arquivo csv
            updateNSongsToCSV(-1);
            data = (Data*)realloc(data, (nSongs-1) *sizeof(Data));
            //atualiza o arquivo csv que armazena as informacoes das musicas
            updateSongsDataCSV(data);
            sprintf(aux, "\033[1;32mA musica com o id '%d' foi removida!\033[0m\n\n", k);
            strcat(buffer, aux);

        }
    }else
        strcat(buffer, "Nao ha elementos!\n--------------------------------------\n\n");
    
    send(client_socket, buffer, strlen(buffer), 0);
    return data;
}

/**************************** updateSongDataCSV() **********************************
*  Funcao responsavel por sobrescrever o csv songsData.csv com os arquivos que estao em memoria
*  com isso mantem-se o arquivo csv e os dados em memoria sincronizados 
*  Argumentos: 
*   -   *data - ponteiro de struct data
*/
void updateSongsDataCSV(Data* data){
    char s_id[STR], s_year[STR];
    int i, nSongs = nSongsFromCSV();
    FILE *fp = fopen("songsData.csv", "w+");
    fprintf(fp, "%s\n", "Id,Titulo,Interprete,Idioma,Tipo,Refrao,Ano");
    for(i=0; i < nSongs; i++){
        sprintf(s_id, "%d", data[i].id);        //converto os itens que sao int para string
        sprintf(s_year, "%d", data[i].year);    //
        
        //escrevo no arquivo csv
        fprintf(fp, "%s,%s,%s,%s,%s,%s,%s\n", s_id, data[i].title, data[i].auth, data[i].lang, data[i].style, data[i].chorus, s_year);
    }
    fclose(fp);

}

/******************************* countData() *************************************
*  Funcao responsavel por contar quantas linhas tem no arquivo csv, a cada '\n' encontrado qtd = qtd + 1 
*  Retorno:
*   qtd - 1, pois assim desconsidera o cabecalho do arquivo csv
*/
int countSongsData(){
    FILE *fp = fopen("songsData.csv", "r");
    int qtd=0, c;

    if(fp != NULL){
        fseek(fp, 0, SEEK_SET);     //reseta o ponteiro do arquivo para a posicao inicial
        while ((c = getc(fp)) != EOF){
            if(c == '\n')
                qtd++;
        }
    }else{
        perror("Erro ao abrir arquivo.");
        exit(-1);
    }
    fclose(fp);
    return qtd-1;
}

/******************************* countData() *************************************
*  Funcao responsavel por verificar o valor que esta no arquivo nSongs.csv
*  Retorno:
*   qtd - 1, pois assim desconsidera o cabecalho do arquivo csv
*/
int nSongsFromCSV(){
    FILE *fp = fopen("nSongs.csv", "r");
    char row[STR];
    // conta os caracteres da primeira linha(cabecalho csv) para posicionar o ponteiro na posicao correta
    int len = lenFirstRowFromCSV("nSongs.csv");     
    fseek(fp, len, SEEK_SET);
    fgets(row, STR, fp);    //pega o conteudo da linha seguinte ao cabecalho, esse o numero de musicas cadastradas
    fclose(fp);
    return atoi(row);       //como o arquivo resgata uma string, usamos a funcao atoi() para converter um array em inteiro
    
}

/*************************** updateNSongDataTOCSV() *******************************
*  Funcao responsavel por sobrescrever o csv nSongs.csv com um novo valor, apos cadastrar uma nova musica ou remover uma musica
*  chamamos esta funcao para atualizar, eh passado o valor 1 ou -1, com isso regatamos o numero de musicas, incrementamos e escrevemos de volta
*  Argumentos: 
*   -   n - inteiro que eh +1 ou -1, vindo das funcoes writeSong ou deleteSong
*/
void updateNSongsToCSV(int n){
    char row[STR];
    int nSongs = nSongsFromCSV(), i;
    FILE *fp = fopen("nSongs.csv", "w+");
    
    nSongs = nSongs + n;
    
    sprintf(row, "%d", nSongs);
    fprintf(fp, "%s\n%s\n", "Quantidade de dados", row);
    fclose(fp);
}

/******************************* generateId() *************************************
*  Funcao responsavel gerar um numero que sera usado como id de uma musica
*  abrimos o arquivo e lemos o valor que esta cadastrado em songId.csv, esse valor eh salve em newId. Apos isso abrimos novamente o arquivo para atualizar
*  o valor do csv que eh incrementado em +1. Isso garante que nenhuma musica tera o mesmo id
*  Retorno: 
*   -   newId - inteiro que sera usado como o id de uma musica que esta sendo cadastrada
*/
int generateId(){
    char row[STR];
    int newId, i;
    FILE *fp = fopen("songId.csv", "r");
    
    for(i=0; i < 2; i++){
        fgets(row, STR, fp);
    }
    fclose(fp);
    newId = atoi(row);
    
    fp = fopen("songId.csv", "w+");
    sprintf(row, "%d", newId+1);
    fprintf(fp, "%s\n%s\n", "SongId", row);
    
    fclose(fp);
    return newId;
}


/**************************** lenFirstRowFromCSV() **********************************
*  Funcao que conta quantos caracteres tem na primeira linha, que representa o titulo do arquivo .csv e 
*   retornar a posicao da segunda linha do arquivo, que eh a partir de onde os dados estao
*  Argumentos:
*   -   char *s: eh o nome do arquivo que vc deve passar, ex: lenFirstRow("songsData.csv");
*/
int lenFirstRowFromCSV(char *s){
    FILE *fp = fopen(s, "r");
    if(fp != NULL){
        int c, len=0;
        while ((c = getc(fp)) != '\n') len++;
        fclose(fp);
        return len+1;
    }else{
        perror("Erro ao abrir arquivo/Arquivo nao existe.");
        exit(-1);
    }
    
}

/********************************* loadData() ***************************************
*  Funcao que carrega os dados do csv para a memoria do programa em uma struct Data . Isso eh feito toda vez que rodamos o prgrama. 
*   Abre o arquivo songsData.csv e vai quebrando a string em tokens, usando a virgula como separador, similar a uma funcao split de outras linguagens
*   Cada token gerado eh salvo no vetor de struct data no campo correto
*  Retorno: 
*   *data - ponteiro de struct data
*/
Data* loadSongsData(){
    Data *data;
    int len = lenFirstRowFromCSV("songsData.csv");
    int i=0, opt=0, n = nSongsFromCSV();
    char row[MAXSTR];
    char *token;
    if(countSongsData() == 0){
        data = (Data *)malloc(1 * sizeof(Data));
        
    }else{
        FILE *fp = fopen("songsData.csv", "a+");
        data = (Data *)malloc( (n+1) * sizeof(Data));

        fseek(fp, len, SEEK_SET);     //reseta o ponteiro do arquivo para a posicao apos o titulo das colunas
        
        while(fgets(row, MAXSTR, fp) != NULL){  // recebe uma linha enquanto não encontra um '\n'
            token = strtok(row, ",");           // separa os tokens sempre que encontra uma ','
            opt = 0;

            //esse while funciona como uma funcao a funcao split de outras linguagens fazendo o split por ',' em cada linha, 
            //como o arquivo que salva os dados eh um csv, eh importante realizar esse parse
            while(token != NULL){               // salva cada token na posicao correta da struct data.
                
                if(opt == 0){
                    data[i].id = atoi(token);
                }else if(opt == 1){
                    strcpy(data[i].title, token);
                }else if(opt == 2){
                    strcpy(data[i].auth, token);
                }else if(opt == 3){
                    strcpy(data[i].lang, token);
                }else if(opt == 4){
                    strcpy(data[i].style, token);
                }else if(opt == 5){
                    strcpy(data[i].chorus, token);
                }else if(opt == 6){
                    data[i].year = atoi(token);
                } 
                token = strtok(NULL, ",");
                opt++;                          // a cada iteracao eu salvo do laco interno salvo o token num campo diferente da struct
            }
            i++;
            
        }
        
    }
    return data;

}


/***************************** printEssencialData() **********************************
*  Funcao que ao ser chamada printa "id, Noma da Musica, Autor" de acordo com alguma condicao
*  Argumentos:
*   -   *data - ponteiro de struct data
*   -   client_socket - numero do socket do client que o server fez accept
*   - flags:
*       flag == 0: condicao: id            | flag == 1: condicao dado um ano
*       flag == 2: condicao: ano && idioma | flag == 3: condicao estilo
*   - de acordo com a flag eh pedido para o client informar alguns dos argumentos: id, year, lang e style
*/
void printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style){
    int i, qtd=0, n = nSongsFromCSV(), tamAtual=MAXSTR;
    char aux[MAXSTR];
    char *buffer = (char*)malloc(MAXSTR * sizeof(char)); 

    //Prepara o cabecalho que sera retornado ao client
    sprintf(buffer, "\n%-3s %-20s %-15s\n", "id","Musica","Autor");
    sprintf(aux, "---+--------------------+---------------\n");
    strcat(buffer, aux);    
    
    if(n > 0){
    
        if(flag == 0){                          // se flag == 0, estamos interessados em buscar uma musica pelo id
            for(i=0; i < n; i++)
                if(data[i].id == id){
                    sprintf(aux, "%-3d %-20s %-15.20s\n", data[i].id, data[i].title, data[i].auth);     
                    if(strlen(aux) + strlen(buffer) >= tamAtual){
                        tamAtual = tamAtual*2;
                        buffer = (char*)realloc(buffer, tamAtual*sizeof(char));
                    }
                    strcat(buffer, aux);        //concatena as linhas encontradas no buffer
                    qtd++;                      //incrementa a quantidade de encontrados
                }
        
        }else if(flag == 1){                    // se flag == 1, estamos interessados em buscar musicas pelo ano de lancamento
            for(i=0; i < n; i++)
                if(data[i].year == year ){
                    sprintf(aux, "%-3d %-20s %-15.20s\n", data[i].id, data[i].title, data[i].auth);     
                    if(strlen(aux) + strlen(buffer) >= tamAtual){
                        tamAtual = tamAtual*2;
                        buffer = (char*)realloc(buffer, tamAtual*sizeof(char));
                    }
                    strcat(buffer, aux);        //concatena as linhas encontradas no buffer
                    qtd++;                      //incrementa a quantidade de encontrados
                }

        }else if(flag == 2){                    // se flag == 0, estamos interessados em buscar musicas pelo idioma e ano de lancamento
            for(i=0; i < n; i++){
                if(data[i].year == year && strncmp(lang, data[i].lang, strlen(lang)-1) == 0){
                    sprintf(aux, "%-3d %-20s %-15.20s\n", data[i].id, data[i].title, data[i].auth);     
                    if(strlen(aux) + strlen(buffer) >= tamAtual){
                        tamAtual = tamAtual*2;
                        buffer = (char*)realloc(buffer, tamAtual*sizeof(char));
                    }
                    strcat(buffer, aux);        //concatena as linhas encontradas no buffer
                    qtd++;                      //incrementa a quantidade de encontrados
                }
            }

        }else if(flag == 3){                    // se flag == 0, estamos interessados em buscar musicas pelo estilo musical
            for(i=0; i < n; i++){
                if( strncmp(style, data[i].style, strlen(style)-1) == 0 ){
                    sprintf(aux, "%-3d %-20s %-15.20s\n", data[i].id, data[i].title, data[i].auth);     
                    if(strlen(aux) + strlen(buffer) >= tamAtual){
                        tamAtual = tamAtual*2;
                        buffer = (char*)realloc(buffer, tamAtual*sizeof(char));
                    }
                    strcat(buffer, aux);        //concatena as linhas encontradas no buffer
                    qtd++;                      //incrementa a quantidade de encontrados
                }
            }
        
        }
    }      
    if(qtd == 0)
        strcat(buffer, "Nenhum dado encontrado!\n");
    
    
    strcat(buffer, "---+--------------------+---------------\n\n");
    //Envia para o client o que foi armazenado no buffer
    send(client_socket, buffer, strlen(buffer), 0);
    free(buffer);

}

/******************************* printAllDataInfo ***********************************
*  Funcao que ao ser chamada printa "id, Noma da Musica, Autor, Idioma, Estilo, Refrao, Ano" 
*  Argumentos:
*   -   *data - ponteiro de struct data
*   -   client_socket - numero do socket do client que o server fez accept
*  Existe uma formatacao para a saida ficar padronizada, entao existe um espaco reservado para aquela informacao, caso 
*  o espaco destinado nao seja ocupado, espaco sao colocados, mas caso a informacao seja maior, o print nao exibira por completo, mas
*  a informacao ainda esta armazenada no "banco de dados"
*/
void printAllDataInfo(Data *data,  int client_socket){
    FILE *fp;
    char aux[MAXSTR];
    char *buffer = (char*)malloc(MAXSTR * sizeof(char)); 
    int i, n = nSongsFromCSV(), qtd=0, tamAtual=MAXSTR;
    
    buffer[0] = '\n';
    memset(buffer, 0, MAXSTR);
    
    sprintf(buffer, "\n%-3s %-18s %-15s %-8s %-9s %-35s %-4s\n", "id","Musica","Autor", "Idioma", "Estilo", "Refrao", "Ano");
    sprintf(aux,"---+------------------+---------------+---------+--------+-----------------------------------+-----\n");
    strcat(buffer, aux);    
    
    
    if(n > 0){
        for(i=0; i < n; i++){
            //printf(          "%-3d %-18.18s %-15.15s %-9.9s %-8.8s %-32.32s... %-4d\n", data[i].id, data[i].title, data[i].auth, data[i].lang, data[i].style, data[i].chorus, data[i].year);     
            sprintf(aux, "%-3d %-18.18s %-15.15s %-9.9s %-8.8s %-32.32s... %-4d\n", data[i].id, data[i].title, data[i].auth, data[i].lang, data[i].style, data[i].chorus, data[i].year);
            if(strlen(aux) + strlen(buffer) >= tamAtual){
                tamAtual = tamAtual*2;
                buffer = (char*)realloc(buffer, tamAtual*sizeof(char));
            }
            strcat(buffer, aux);   
            qtd++;
        }
    }
    
    if(qtd == 0)
        strcat(buffer, "Nenhum dado encontrado!\n");
        

    strcat(buffer,"---+------------------+---------------+---------+--------+-----------------------------------+-----\n\n");
    send(client_socket, buffer, strlen(buffer), 0);
    free(buffer);
}

/******************************* findToDownload ***********************************
*  Funcao que ao ser chamada verifica se o id passado esta presente no banco de dados
*  Argumentos:
*   -   *data - ponteiro de struct data
*   -   client_socket - numero do socket do client que o server fez accept
*   -   id - numero que o cliente deseja baixar
*/
int findToDownload(Data *data, int client_socket, int id){
    int i, n = nSongsFromCSV(), qtd=0, tamAtual=MAXSTR;
    char buffer[MAXSTR], aux[MAXSTR];
    
    if(n > 0){
        for(i=0; i < n; i++){
            if(data[i].id == id){
                sprintf(aux, "%s", data[i].title);     
                strcat(buffer, aux);        //concatena as linhas encontradas no buffer
                qtd++;                      //incrementa a quantidade de encontrados
                break;
            }
        }
    }
    
    if(qtd == 0){
        i = -1;                             //retorna -1 se nada foi encontrado
    }else{
        //strcat(buffer,"---+--------------------+---------------\n\n");
//        printf("NOMEEE: %s\n", buffer);
        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, MAXSTR);
    }

    return i;                               //retorna i que eh a posicao que o dado esta no vetor
}

/******************************* validaInput ***********************************
*  Funcao que ao ser chamada verifica se a entrada contem apenas digitos numericos
*  Argumentos:
*   -   *str - uma string
*/
int validaInput(char *str){
    for(int i=0; i < strlen(str) - 1; i++){ // strlen - 1 para desconsiderar o \0 da string 
        if(!isdigit(str[i]))
            return -1;
    }
    return 1;
}
