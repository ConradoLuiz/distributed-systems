/************************************************************************/
/* Nome do Arq : serv-conc.c                                            */
/* Funcao      : servidorconcorrente                                    */
/* Autor       : Jorge Lopes de Souza Leao                              */
/************************************************************************/
/* Data Inicio : 26 abr 01                                              */
/* Ult Modif   : 30 mai 03                                              */
/************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h> // --- DIFERE DA DOCUMENTACAO !!
#include <netdb.h>
#include <pthread.h>

#define EMPTYLINE "\n"
#define PORTA     3457

//------------------------------------------------------------
int sPassivo,sAtivo;
struct sockaddr_in enderServ1;
struct sockaddr_in enderConex1;
int tamEnderConex;
char linha[80];
int i;
char Nome[64];
struct hostent *ptrDadosDoHost;
int result;

int created_thread;

char response[64];

void setupsocket() {
   sPassivo = socket(PF_INET,SOCK_STREAM,0);
   if(sPassivo == -1){
      perror("[Servidor] Erro na criacao do sock 1 do servidor: ");
      exit(0);
   }

   enderServ1.sin_family = AF_INET;
   enderServ1.sin_port   = htons(PORTA);
   bcopy(ptrDadosDoHost->h_addr,
         &enderServ1.sin_addr.s_addr, 
         ptrDadosDoHost->h_length);

   if(bind(sPassivo,(struct sockaddr *)&enderServ1,sizeof(enderServ1))){
      perror("[Servidor] Erro no bind: ");
      exit(0);
   }

   if(listen(sPassivo,10)){
      perror("[Servidor] Erro no listen: ");
      exit(0);
   }
}

void* handle_client(void* arg) {
   long unsigned int tid = pthread_self();
   int *socket = (int *)arg;


   if ( read(*socket, linha, 80) == -1 ) { perror("[Servidor] Erro no read: "); }
   else {
      printf("[Servidor] Recebeu: %s\n",linha);
      sprintf(response, "Pois nao. O servico foi prestado do processo %lu", tid);
      write(*socket, response, strlen(response));
   }

   shutdown(*socket, 2);

   pthread_exit(NULL);
}

//------------------------------------------------------------
int main(){

   char* NOME = "localhost";

   for(i=1;i<5;i++)printf(EMPTYLINE);
   printf("[Servidor] Servidor CON-CON entrando em funcionamento.\n           Usando a porta %d.\n",PORTA);
   printf("[Servidor] Escutando em %s:%d\n", NOME, PORTA);
   
   ptrDadosDoHost = (struct hostent *)gethostbyname(NOME);

   setupsocket();

   while(1){
      
      printf("[Servidor] (Vai esperar pedido de servico...)\n");
      sAtivo = accept(sPassivo,&enderConex1,&tamEnderConex);

      if(sAtivo<0){
         perror("[Servidor] Erro no Accept: ");
         exit(0);
      }

      pthread_t ptid;

      pthread_create(&ptid, NULL, &handle_client, (void *)&sAtivo);

   }
}













