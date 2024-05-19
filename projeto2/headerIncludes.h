#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <ctype.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_addr()

#define MAXSTR 10240
#define STR 1024
#define col 7 //qtd de informacoes

#define PORT 9877
#define TCP_PORT 9877
#define UDP_PORT 9988
#define LISTENQUEUE 1024
#define TIME_OUT 5  //tempo em segundos

typedef struct Data{
    int  id;
    char title[STR];
    char auth[STR];
    char lang[STR];
    char style[STR];
    char chorus[MAXSTR];
    int year;
}Data;

// Struct na qual tanto o client como o server tem, sendo que o server envia os packets
// e o client precisa saber o formato para poder utilizar os dados
typedef struct Packet{
    int id;
    char data[MAXSTR];
}Packet;

//************************************ PROTOTIPOS ************************************
//UTILIZADAS NO PROJETO 2
Data*   loadSongsData();                //carrega as informacoes das musicas para uma struct
int     nSongsFromCSV();                //carrega a quantidade/numero de musicas
int     lenFirstRowFromCSV(char *s);    //conta o numero de caracteres do cabecalho csv     
void    printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style); //FOI ALTERADA NO PROJETO 2
void    printAllDataInfo(Data *data, int client_socket);   //FOI ALTERADA NO PROJETO 2

//ADICIONADAS PARA O PROJETO 2
int     findToDownload(Data *data, int client_socket, int id);
int     validaInput(char *str);


//NAO UTILIZADA NO PROJETO 2
Data*   writeSong(Data *data, int client_socket, char title[STR], char auth[STR], char lang[STR], char year[STR], char style[STR], char chorus[MAXSTR]);       //NAO UTILIZADA NO PROJETO 2
Data*   deleteSong(Data *data, int client_socket, int k);       //NAO UTILIZADA NO PROJETO 2
void    updateSongsDataCSV(Data* data);                         //NAO UTILIZADA NO PROJETO 2
int     countSongsData();               //conta o numero de musicas do csv  //NAO UTILIZADA NO PROJETO 2
void    createCSVs();
void    updateNSongsToCSV();                                    //NAO UTILIZADA NO PROJETO 2
int     generateId();                   //                      //NAO UTILIZADA NO PROJETO 2




