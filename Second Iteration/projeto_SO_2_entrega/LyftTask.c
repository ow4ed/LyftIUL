#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    printf("Execução do LyftTask.\n");
	FILE *f;
	FILE *l;
	f=fopen("viagens.txt","r");//leitura do ficheiro viagenst.txt para saber se existe
	if(f){// se existir o ficheiro viagens.txt
		int n=fork();
		if(n==0){
			execl("/home/a82247/projeto/atualiza_condutores.sh",NULL);//o script atualiza_condutores.sh já remove o ficheiro viagens.txt depois de atualizar o condutores.txt
		}
		else{
			wait(NULL);
			l=fopen("lyftadmin.pid","r");
			if(l){//obtem o pid do processo LyftAdmin e envia-lhe o sinal SIGUSR1
				char str[10];
				int pid;
				fgets(str,10,l);
				pid=atoi(str);
				printf("%d\n",pid);
				kill(pid,SIGUSR1);
			}
			fclose(f);
			fclose(l);
		}
	}
	else{// se nao existe o ficheiro viagens.txt nao vai ser atualizado nada
		printf("Não existe o ficheiro viagens.txt\n");
	}
}
