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

typedef struct Packet{
    int id;
    char data[MAXSTR];
}Packet;

//************************************ PROTOTIPOS ************************************
void    createCSVs();
Data*   writeSong(Data *data, int client_socket, char title[STR], char auth[STR], char lang[STR], char year[STR], char style[STR], char chorus[MAXSTR]);
Data*   deleteSong(Data *data, int client_socket, int k);
void    updateSongsDataCSV(Data* data);
int     countSongsData();               //conta o numero de musicas do csv
int     nSongsFromCSV();                //carrega a quantidade/numero de musicas
void    updateNSongsToCSV();
int     generateId();                   //
int     lenFirstRowFromCSV(char *s);    //conta o numero de caracteres do cabecalho csv
Data*   loadSongsData();                //carrega as informacoes das musicas para uma struct
void    printEssencialData(Data *data, int client_socket, int flag, int id, int year, char *lang, char *style);
void    printAllDataInfo(Data *data, int client_socket);   //



