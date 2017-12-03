#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <time.h>   

#define FLT_MAX 3.402823466e+38F /* max value */

typedef struct Musica{
	float *array;
	char *genero;
} Musica;

float menor_distancia = FLT_MAX;
struct Musica melhor_musica;
void debugArq(char line[], int qdeElem){
	printf("Line: %s \n", line);
    printf("%d elementos \n",qdeElem);
}
void print_array(float *array, int length){
	printf("Printando array \n");
    for (int i = 0; i < length; i++) {
    	printf("array[%d] => %f; \n",i,array[i]);
    }
}
/*https://stackoverflow.com/questions/4235519/counting-number-of-occurrences-of-a-char-in-a-string-in-c*/
int countChars( char* s, char c ){
    return *s == '\0'? 0: countChars( s + 1, c ) + (*s == c);
}
/*https://stackoverflow.com/questions/1515195/how-to-remove-n-or-t-from-a-given-string-in-c*/
void strip(char *s) {
    char *p2 = s;while(*s != '\0') {if(*s != '\t' && *s != '\n') {*p2++ = *s++;} else {++s;}}*p2 = '\0';
}

float calculaEucliciana(float el1,float el2){
	return pow((el1-el2), 2);
}

void leituraArquivo(char dir[],struct Musica **musicas_return,int *qdeElem,int *qdeMusicas){
	size_t len = 0;
	FILE *file;
	char *line = NULL,
	     *split,
		 read,
		 eof[100]="#EOF#";

	*qdeElem=0;
	float *array;
	file = fopen(dir, "r");
	int ind_array=0;
	*qdeMusicas=0;

	struct Musica *musicas = (struct Musica *)malloc(sizeof(struct Musica));
	file = fopen(dir, "r");
	if(file != NULL){
		while (1) {
			read = getline(&line, &len, file);
			if(strcmp(line,eof)==0){
       			break;
       		}
			ind_array=0;
    		if(*qdeElem==0){
    			*qdeElem = countChars(line,',');
    		}
    		musicas = (struct Musica *)realloc(musicas,(*qdeMusicas+1)*sizeof(Musica));
    		musicas[*qdeMusicas].array = (float *)malloc(*qdeElem*sizeof(float));

    		while((split = strsep(&line,",")) != NULL ){
    			if(ind_array!=*qdeElem){
	    			musicas[*qdeMusicas].array[ind_array] = strtof(split, NULL);
	       		}else{
	       			musicas[*qdeMusicas].genero = strtok(split, "");
	       			strip(musicas[*qdeMusicas].genero);
	       		}
       			ind_array++;
    		}    	
    		//treino(musicas[qdeMusicas],qdeElem);
    		*qdeMusicas+=1;
		}
		*musicas_return = musicas;

		fclose(file);
    }else{
    	printf("Erro, nao foi possivel abrir o arquivo %s \n",dir);
	}
    
}

float distanciaMusica(struct Musica caso_teste,struct Musica caso_treino,int qdeElem){
	float distancia=0;
	for(int i=0;i<qdeElem;i++){
		distancia+=calculaEuclidiana(caso_teste.array[i],caso_treino.array[i]);
	}	
	distancia = sqrt(distancia);
	return distancia;
}
void jajajampi(){
	int id, size,i,val,max,min, local_val;
	MPI_Init(&argc,&argv);
	MPI_Status st;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	srand(time(NULL)+id);//gerar um valor aleatório baseado no tempo e no id;
	local_val = rand()%1000;
	float distancia;

	if(id == 0){ 
		max = min = local_val;
		printf("valor local %d = %d\n",id,local_val);
		for(i=1;i<size;i++){//para todos os processos da aplicação
			MPI_Recv(&val,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);//recebe valor
			printf("recebido %d do processador %d\n",val,i);
			if(val<min){//verifica se valor recebido é menor que o mínimo atual
				min = val;
			}else if(val > max){//verifica se valor recebido é maior que o máximo atual
				max = val;
			}
		}
		for(i=1;i<size;i++){//envia mínimo e máximo para todos processos
			MPI_Send(&max,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
			MPI_Send(&min,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
		}
	}else{
		// ele apenas envia seu valor e recebe o mínimo e o máximo
		distancia = distanciaMusica(caso_teste,casos_treino[i],qdeElem);
		MPI_Send(&distancia,1,MPI_INT,0,TAG,MPI_COMM_WORLD);
		MPI_Recv(&max,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);
	}
}
void calcula_menor_distancia(Musica caso_teste, 
							 Musica *casos_treino,
							 int qdeElem,
							 int size_treino,
							 int id_mpi,
							 int num_mpi){
	float distancia;
	for(int i=id_mpi;i<size_treino;i+=num_mpi){
		distancia = distanciaMusica(caso_teste,casos_treino[i],qdeElem);
		if(distancia < menor_distancia){
			menor_distancia = distancia;
			melhor_musica = casos_treino[i];
		}
	}
}

int main(int argc, char *argv[]){		
	char *line = NULL,
	     *split,
		 read,
		 dir_test[100]="bases/test_",
		 dir_treino[100]="bases/train_";

	int qdeElem=0,
	    ind_array=0,
	    qdeMusicas=0,
	    size_teste,
	    size_treino,
	    erros=0,
	    acertos=0,
	    nth=1,
	    caso=0,
	    detalhes=0;
	strcat(dir_test, argv[1]);
	strcat(dir_test,".data");
	strcat(dir_treino, argv[1]);
	strcat(dir_treino,".data");
	nth = atoi(argv[2]);
	detalhes = atoi(argv[3]);

	size_t len = 0;
	float MENOR_DISTANCIA;
	struct Musica MELHOR_MUSICA;
	struct Musica *casos_teste = (struct Musica *)malloc(sizeof(struct Musica));
	struct Musica *casos_treino = (struct Musica *)malloc(sizeof(struct Musica));

	/*-----var def-----*/
	printf("::Iniciando::\n");
	printf("Teste: %s\n Treino: %s\n",dir_test,dir_treino);
	printf("Num threads: %d\n ",nth);
	leituraArquivo(dir_test,&casos_teste,&qdeElem,&size_teste);
	printf("Quantidade de testes: %d \n",size_teste);
	printf("Quantidade de elementos: %d \n",qdeElem);
	leituraArquivo(dir_treino,&casos_treino,&qdeElem,&size_treino);
	printf("Quantidade de treinos: %d \n",size_treino);
	printf("Quantidade de elementos: %d \n",qdeElem);

	for(int i=0;i<size_teste;i++){
		menor_distancia = FLT_MAX;
		memset(&melhor_musica, 0, sizeof(melhor_musica));
		calcula_menor_distancia(casos_teste[i],casos_treino,qdeElem,size_treino,nth);
		if(detalhes!=0){
			printf("Genero: [%s => %s]; Menor distancia %.10f \n",casos_teste[i].genero,melhor_musica.genero,menor_distancia);
		}
		if(strcmp(casos_teste[i].genero,melhor_musica.genero) == 0){
			acertos++;
		}else{
			erros++;
		}
	}
	printf("Quantidade de acertos: %d\nQuantidade de erros: %d\nTotal: %d\n",acertos,erros,(erros+acertos));
	printf("\n");
	///*
    /*print_array(casos_teste[0].array,qdeElem);
	print_array(casos_treino[0].array,qdeElem);*/
	//*/
	return 0;
}
