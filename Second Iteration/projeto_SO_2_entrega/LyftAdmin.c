#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "mystring.h"//biblioteca que apenas tem a funcao myfgets
#define ALARMTIMER 60 //espaço de tempo em que cada SIGALARM e enviado

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

Tcondutor *listacondutores;//lista dos condutores
Tpassageiro *listapassageiros;//lista dos passageiros

int readNrLinhasFicheiro (char ficheiro[] ){//esta funcao le o num de linhas que um ficheiro tem
    FILE *f = fopen(ficheiro, "r");

    if(!f){
        perror("Erro na leitura do ficheiro");
        exit(1);
    }

    char input[512];
    int i=0;
    while(fgets(input,512,f) != NULL){
        i++;
    }
    fclose(f);
    return i;
}

void inicializarListas(){//esta funcao inicializa as duas listas 
    int nrLinhasFicheroCondutores = readNrLinhasFicheiro("condutores.txt");//num de linhas no ficheiro condutores.txt
    int nrLinhasFicheioPassageiros = readNrLinhasFicheiro("passageiros.txt");//num de linhas no ficheiro passageiros.txt

    listacondutores = (Tcondutor*)malloc(sizeof(Tcondutor)*nrLinhasFicheroCondutores);//alocacao do espaço em memoria para a lista dos condutores dependendo do numero de linhas do ficheiro condutores.txt
    if(NULL== listacondutores){//caso ocorra erro ao alocar espaço
       perror("ERRO NO MALLOC");
       exit(1);
    }
    listapassageiros = (Tpassageiro*)malloc(sizeof(Tpassageiro)*nrLinhasFicheioPassageiros);//alocacao do espaço em memoria para a lista dos passageiros dependendo do numero de linhas do ficheiro passageiros.txt
    if(NULL== listapassageiros){
       perror("ERRO NO MALLOC");//caso ocorra erro ao alocar espaço
       exit(1);

   }
}

void obter_substring(char linha[], char resultado[], char separador, int indice) {// funcao que serve para obter um bocado de uma string dado um separador 
	int i;
	int j=0;
	int ind=0;
	for (i=0; linha[i] != '\0'; i++) {
		if ( linha[i] == separador ) {
			ind++;
		} else if (ind == indice) {
			resultado[j++]=linha[i];
		}
	}
	resultado[j]='\0';
}

void cria_lyftadmin_pid(){// sempre que o programa LyftAdmin é executado esta funcao ira colocar o seu pid no ficheiro lyftadmin.pid para ser utilizador pelo LyftTask
	int pid = getpid();
	FILE *f;
    f=fopen("lyftadmin.pid","w");
	if(f!=NULL){
		fprintf(f,"%d",pid);
	}
    fclose(f);
}

void ler_condutores(){//esta funcao le o ficheiro condutores.txt e armazena cada condutor em memoria
	FILE *f = fopen("condutores.txt","r");

	if(!f){// se houver erro na aberturar do ficheiro condutores.txt
        perror("Erro na leitura do ficheiro condutores.txt");
        exit(1);
    }

    char input[512];
	char tmp[512];
	int contador=0;

    while(fgets(input,512,f) !=NULL){//para cada linha do ficheiro condutores.txt	
		Tcondutor t;
		obter_substring(input,tmp,':',0);
		t.numero= atoi(tmp);
		obter_substring(input,t.nome,':',1);
		obter_substring(input,t.turma,':',2);
		obter_substring(input,t.telemovel,':',3);
		obter_substring(input,t.email,':',4);
		obter_substring(input,t.tipo,':',5);
		obter_substring(input,t.marca,':',6);
		obter_substring(input,t.matricula,':',7);
		obter_substring(input,tmp,':',8);
		t.viagens=atoi(tmp);
		obter_substring(input,tmp,':',9);
		t.pontos=atoi(tmp);
		obter_substring(input,tmp,':',10);
		t.saldo=atof(tmp);
		t.activo=0;
		t.disponivel_desde=0;
		t.PID=0;
		listacondutores[contador++]=t;
    }
    fclose(f);
}

void ler_passageiros(){//esta funcao le o ficheiro passageiros.txt e armazena cada passageiro em memoria
	FILE *f = fopen("passageiros.txt","r");
	
	if(!f){//se houver um erro ao abrir o ficheiro passageiros.txt
		perror("Erro na leitura do ficheiro passageiros.txt");
		exit(1);
	}
    
	char input[512];
    char tmp[512];
	int contador=0;
	
	while(fgets(input,512,f) != NULL){//para cada linha do ficheiro passageiros.txt
		Tpassageiro t;
		obter_substring(input,tmp,':',0);
		t.numero=atoi(tmp);
		obter_substring(input,t.nome,':',1);
		obter_substring(input,t.turma,':',2);
		obter_substring(input,t.telemovel,':',3);
		obter_substring(input,t.email,':',4);
		strcpy(t.c_credito,"\0");
		listapassageiros[contador++]=t;
	}
	fclose(f);
}

void mostralistacondutores(){//funcao que serve para listar todos os condutores 
	int i;
	printf("Condutores:\n");
	int nl=readNrLinhasFicheiro("condutores.txt");
	for (i=0;listacondutores[i].numero!=0 && i<nl; i++ ){
	printf ("condutor: %3d %-5d %-20s %-4s %-9s %-20s %-10s %-20s %-15s %-3d %-5d %-.1f %-d %-d %-d\n", i, listacondutores[i].numero, listacondutores[i].nome,listacondutores[i].turma,listacondutores[i].telemovel,listacondutores[i].email,listacondutores[i].tipo,listacondutores[i].marca,listacondutores[i].matricula,listacondutores[i].viagens,listacondutores[i].pontos,listacondutores[i].saldo,listacondutores[i].activo,listacondutores[i].disponivel_desde,listacondutores[i].PID );//os valores diferentes em %3d,%5d,%20d,etc sao simplesmente para que na listagem dos condutores o texto resultado fique formatado
	}
	printf("Existem %d condutores.\n",i);
}

void mostralistapassageiros(){//funcao que serve para listar todos os passageiros
	int i;
	printf("Passageiros:\n");
	int nl=readNrLinhasFicheiro("passageiros.txt");
	for(i=0;listapassageiros[i].numero !=0 && i<nl;i++){
	printf("passageiro: %3d %5d %-20s %-5s %-9s %-20s %-20s\n",i,listapassageiros[i].numero,listapassageiros[i].nome,listapassageiros[i].turma,listapassageiros[i].telemovel,listapassageiros[i].email,listapassageiros[i].c_credito);//os valores diferentes %3d,%5d,%-20s,etc sao apenas para formatar a listagem dos passageiros
	}
	printf("Existem %d passageiros.\n",i);
}

void escrever_passageiros(){//esta funcao apaga o ficheiro passageiros.txt e cria-o de novo a partir da informacao guardada em memoria
	int n=readNrLinhasFicheiro("passageiros.txt");
    remove("/home/a82247/projeto/passageiros.txt");//remove o ficheiro passageiros.txt
    FILE *f = fopen("/home/a82247/projeto/passageiros.txt","w");//cria de novo o ficheiro passageiros.txt
    if(NULL==f){
        perror("Erro na criacao de passageiros.txt\n");
    }
    printf("Vou escrever nos passageiros.txt\n");
    int k;
    for(k=0;listapassageiros[k].numero!=0 && k<n;k++){//este ciclo percorre os passageiros em memoria e escreve-os no ficheiro passageiros.txt
         fprintf(f,"%d:%s:%s:%s:%s:%s\n",listapassageiros[k].numero,listapassageiros[k].nome,listapassageiros[k].turma,listapassageiros[k].telemovel,listapassageiros[k].email,listapassageiros[k].c_credito);
    }
    fclose(f);
}

void escrever_condutores(){//esta funcao apaga o ficheiro condutores.txt e cria-o de novo a partir da informacao guardada em memoria
	int n=readNrLinhasFicheiro("condutores.txt");
    remove("/home/a82247/projeto/condutores.txt");//remove o ficheiro condutores.txt
    FILE *f = fopen("/home/a82247/projeto/condutores.txt","w");// cria de novo o ficheiro condutores.txt
    if(NULL==f){
        perror("Erro na criacao de condutores.txt\n");
    }
    printf("Vou escrever nos condutores.txt\n");
    int i;
    for(i=0;listacondutores[i].numero!=0 && i<n;i++){//este ciclo percorre os condutores guardados em memoria e escreve-os no ficheiro condutores.txt
        fprintf (f,"%d:%s:%s:%s:%s:%s:%s:%s:%d:%d:%.1f:%d:%d:%d\n", listacondutores[i].numero, listacondutores[i].nome,listacondutores[i].turma,listacondutores[i].telemovel,listacondutores[i].email,listacondutores[i].tipo,listacondutores[i].marca,listacondutores[i].matricula,listacondutores[i].viagens,listacondutores[i].pontos,listacondutores[i].saldo,listacondutores[i].activo,listacondutores[i].disponivel_desde,listacondutores[i].PID );
    }
    fclose(f);
}


void trata_sinal(int signal){//esta funcao trata os diferentes sinais que o processo LyftAdmin recebe
	if ( signal == SIGUSR1 ){// no caso do sinal ser o SIGUSR1 vao ser atualizadas as informacoes nas estruturas em memoria
		ler_condutores();//vai ser atualizada a informacao guardada em memoria
	}
	if ( signal == SIGTERM ){//no caso do sinal ser o SIGTERM os condutores e passageiros, que estao gravados em memoria, serao escritos nos ficheiros condutores.txt e passageiros.txt,respetivamente
		escrever_condutores();
		escrever_passageiros();
		exit(0);//apos essa escrita o programa LyftAdmin termina
	}
	if( signal == SIGALRM){//ao receber o sinal SIGALARM é executado o LyftTask
       printf("Tratamento do SIGALARM\n");
       int t=fork();
       if( t==0 ){
          execl("/home/a82247/projeto/LyftTask", NULL);
          printf("Executei o lifttask\n");
       }
       else{
         wait(NULL);
         alarm(ALARMTIMER);//o alarm volta a ser armado para daqui a 60s 
         printf("Enviado novo alarm\n");
      }
    }
}



int exists(int num,int pc){// esta funcao procura na listapassageiros ou na listacondutores consoante o valor de pc é 0 ou 1, respetivamente, e devolve o numero da linha em que esta o numero passado como argumento.Se nao existir o numero na lista desejada é devolvido o valor -1
	int encontrou=-1;
	int i;
	if(pc==0){//se o valor de pc for 0, o num vai ser procurado na lista dos passageiros
		for(i=0;listapassageiros[i].numero!=0 && encontrou!=0;i++){
			if(listapassageiros[i].numero==num){//caso encontre esse numero na lista, é devolvida a posicao em que esta o passageiro com esse numero
				encontrou=0;
				return i;
			}
		}
		return -1;//se nao encontrar o num na lista dos passageiros é devolvido -1
	}
	if(pc==1){//se o valor de pc for 1, o num vai ser procurado na lista dos condutores
		for(i=0;listacondutores[i].numero!=0 && encontrou!=0;i++){
			if(listacondutores[i].numero==num){//caso encontre esse numero na lista, é devolvida a posicao em que esta o condutor com esse numero
				encontrou=0;
				return i;
			}
		}
		return -1;//se nao encontrar o num na lista dos condutores é devolvido -1
	}
}

void menu(){//funcao que apresenta o menu de opcoes repetitivamente ate ser escolhido o 0
	int res=-1;
	int num;
	int pos;
	char tmp[50];
	char str[50];
	while(res!=0){
		printf("1) Imprimir memória\n");
		printf("2) Alterar passageiro\n");
		printf("3) Alterar condutor\n");
		printf("0) SAIR\n");
		fgets(tmp,100,stdin);
		res=atoi(tmp);
		if(res==1){
			mostralistacondutores();
			mostralistapassageiros();
			printf("\n");
        }
		if(res==2){
			int naorespondido=0;//este contador verifica se o utilizador fez de facto alguma alteracao no passageiro, caso o utilizador entrar para alterar um passageiro mas nao fizer nenhuma mudanca nos campos nao vai ser preciso atualizar o ficheiro passageiros.txt
			printf("Numero estudante passageiro?\n");
			fgets(tmp,100,stdin);
			num=atoi(tmp);
			pos=exists(num,0);
			if(pos!=-1){
					printf("Se desejar alterar um campo deve inserir o novo valor,senão deixar o espaço em branco para nao alterar\n");
					printf("Nome?\n");
					myfgets(str,50,stdin);		
					if(strcmp(str,"")!=0){//alterar o nome
						strcpy(listapassageiros[pos].nome,str);
						printf("Foi alterado o nome\n");
					}
					else{ 
						printf("O nome nao foi alterado\n");
						naorespondido++;
					}
					printf("Turma?\n");
					myfgets(str,10,stdin);
					if(strcmp(str,"")!=0){//alterar a turma
						strcpy(listapassageiros[pos].turma,str);
						printf("Foi alterada a turma\n");
					}
					else{
						printf("Nao foi alterada a turma\n");
						naorespondido++;
					}
					printf("Telemovel?\n");
					myfgets(str,15,stdin);
                    if(strcmp(str,"")!=0){//alterar o num telemovel
						strcpy(listapassageiros[pos].telemovel,str);
						printf("Foi alterado o num telemovel\n");
					}
					else{
						printf("Nao foi alterado o num telemovel\n");
						naorespondido++;
					}
					printf("email?\n");
					myfgets(str,40,stdin);
                    if(strcmp(str,"")!=0){//alterar o email
						strcpy(listapassageiros[pos].email,str);
						printf("Foi alterado o email\n");
					}
					else{ 
						printf("Nao foi alterado o email\n");
						naorespondido++;
					}
					printf("cartao_credito?\n");
					myfgets(str,20,stdin);
                    if(strcmp(str,"")!=0){//alterar o num c_credito
						strcpy(listapassageiros[pos].c_credito,str);
						printf("Foi alterado o num de c_credito");
					}		
					else{
						printf("Nao foi alterado o num de c_credito\n");
						naorespondido++;
					}
					if(naorespondido==5){//caso nao tenha alterado nada
						printf("Nada foi alterado...\n");
					}else{
						escrever_passageiros();//para que as alteracoes que foram feitas em memoria sejam feitas tambem no ficheiro passageiros.txt
						printf("Foram gravadas as alteracoes\n");
					}
			}	
			else{//caso nao seja encontrado um passageiro com este numero
				printf("Não existe um passageiro com esse numero\n");
			}
				
		}
		 if(res==3){
			int naorespondidas=0;
			printf("Numero estudante condutor?\n");
			fgets(tmp,100,stdin);
			num=atoi(tmp);
			pos=exists(num,1);
			if(pos!=-1){
				printf("Se desejar alterar um campo deve inserir o novo valor, senão deixar o espaço em branco para nao alterar\n");
				printf("Nome?\n");
				myfgets(str,50,stdin);
                if(strcmp(str,"")!=0){//alterar o nome	
					strcpy(listacondutores[pos].nome,str);
					printf("Foi alterado o nome\n");
				}
				else{
					printf("Nao foi alterado o nome\n");
					naorespondidas++;
				}
				printf("Turma?\n");
				myfgets(str,10,stdin);
                if(strcmp(str,"")!=0){//alterar a turma
					strcpy(listacondutores[pos].turma,str);
					printf("Foi alterada a turma\n");
				}
				else{
					printf("Nao foi alterada a turma\n");
					naorespondidas++;
				}
				printf("Num telemovel?\n");
				myfgets(str,15,stdin);
                if(strcmp(str,"")!=0){//alterar o num telemovel
					strcpy(listacondutores[pos].telemovel,str);
					printf("Foi alterado o num telemovel\n");	
				}
				else{
					printf("Nao foi alterado o num telemovel\n");
					naorespondidas++;
				}
				printf("Email?\n");
				myfgets(str,40,stdin);
                if(strcmp(str,"")!=0){//alterar email
					strcpy(listacondutores[pos].email,str);
					printf("Foi alterado o email\n");
				}
				else{
					printf("Nao foi alterado o email\n");
                    naorespondidas++;
				}
				printf("Tipo?\n");
				myfgets(str,20,stdin);
                if(strcmp(str,"")!=0){//alterar o tipo de veiculo
					strcpy(listacondutores[pos].tipo,str);	
					printf("Foi alterado o tipo\n");
				}
				else{
					printf("Nao foi alterado o tipo\n");
                    naorespondidas++;
				}
				printf("Marca?\n");
				myfgets(str,20,stdin);
                if(strcmp(str,"")!=0){//alterar a marca
					strcpy(listacondutores[pos].marca,str);
					printf("Foi alterada a marca\n");
				}
				else{
					printf("Nao foi alterada a marca\n");
                    naorespondidas++;
				}
				printf("Matricula?\n");
				myfgets(str,15,stdin);
                if(strcmp(str,"")!=0){//alterar a matricula
					strcpy(listacondutores[pos].matricula,str);
					printf("Foi alterada a matricula\n");	
				}
				else{
					printf("Nao foi alterada a matricula\n");
                    naorespondidas++;
				}
				printf("Activo?\n");
				myfgets(tmp,40,stdin);
                int activo=atoi(tmp);
                if(strcmp(tmp,"")!=0){//alterar ativo 
                    listacondutores[pos].activo=activo;
					printf("Foi alterado o ativo\n");
                }
                else{
					printf("Nao foi alterado o ativo\n");
                    naorespondidas++;
                }
				printf("Disponivel_desde?\n");
				myfgets(tmp,40,stdin);
                long disponivel_desde=atol(tmp);
                if(strcmp(tmp,"")!=0){//alterar o disponivel desde
                    listacondutores[pos].disponivel_desde=disponivel_desde;
					printf("Foi alterado o disponivel_desde\n");
                }
                else{
					printf("Nao foi alterado o disponivel_desde\n");
                    naorespondidas++;
                }
				printf("PID?\n");
				myfgets(tmp,40,stdin);
                int PID=atoi(tmp);
                if(strcmp(tmp,"")!=0){//alterar o pid
                    listacondutores[pos].PID=PID;
					printf("Foi alterado o PID\n");
                }
                else{
					printf("Nao foi alterado o PID\n");
                    naorespondidas++;
                }
				if(naorespondidas==10){//se nada for alterado nao vale a pena estar a atualizar o ficheiro condutores.txt
					printf("Nada foi alterado...\n");
				}
				else{
					escrever_condutores();//para atualizar as alteracoes feitas em memoria no ficheiro condutores.txt
					printf("Foram gravadas as alteracoes\n");
				}
			}
			else{//caso nao for encontrado um condutore com este numero
				printf("Não existe um condutor com esse numero\n");
			}
		}
    }
	exit(0);
}

int main(){
    printf("%d\n",getpid());
	
	inicializarListas();//inicializar as listas em memoria, o programa fecha se esta funcao nao for bem sucedida
	
    ler_condutores();//passar para a lista de condutores em memoria os condutores do ficheiro condutores.txt

    ler_passageiros();//passar para a lista de passageiros em memoria os passageiros do ficheiro passageiros.txt

    //cria_lyftadmin_pid();//guardar o pid do processo LyftAdmin no ficheiro lyftadmin.pid para ser utilizado pelo LyftTask

    signal(SIGUSR1, trata_sinal);//tratamento do sinal SIGUSR1
    signal(SIGTERM, trata_sinal);//tratamento do sinal SIGTERM
    signal(SIGALRM, trata_sinal);//tratamento do sinal SIGALARM
	
	cria_lyftadmin_pid();

    printf("vou mandar o alarm do main em %d sec\n",ALARMTIMER);
    alarm(ALARMTIMER);//primeiro sinal de alarm a ser enviado, os proximos seram enviados na funcao trata_sinal
    printf("mandei o alarme do main de %d sec\n",ALARMTIMER);
    menu();// abertura do menu
}

