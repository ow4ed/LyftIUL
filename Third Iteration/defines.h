#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sem.h>
#include "mystring.h"//biblioteca que apenas tem a funcao myfgets
#define ALARMTIMER 60 //espaço de tempo em que cada SIGALARM e enviado
#define exit_on_error(a,s) if (a<0){perror(s); exit(1);}
#define exit_on_null(a,s) if (a==NULL){perror(s); exit(1);} 

struct sembuf DOWN = { 0, -1, 0};
struct sembuf UP = { 0, 1, 0};

typedef struct {
    int numero;
    char nome[50];
    char turma[10];
    char telemovel[15];
    char email[40];
    char tipo[20];
    char marca[20];
    char matricula[15];
    int viagens;
    int pontos;
    float saldo;
    int activo;
    long disponivel_desde;
    int PID;
} Tcondutor;//estrutura de um condutor

typedef struct {
    int numero;
    char nome[50];
    char turma[10];
    char telemovel[15];
    char email[40];
    char c_credito[20];
} Tpassageiro;//estrutura de um passageiro

typedef struct {
	long tipo;
	struct {
		int pid_passageiro;
		int pid_condutor;
		char local_encontro[100];
		long data;
		float pontos;
		float valor;
	} dados;
} MsgViagem;

typedef struct {
	int numCondutores;
	int numPassageiros;
	void * inicio;
}ShMem;

