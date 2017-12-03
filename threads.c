#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FLT_MAX 3.402823466e+38F /* max value */

pthread_mutex_t lock;

typedef struct Musica{
	float *array;
	char *genero;
} Musica;

typedef struct StructParamThread{
	Musica caso_teste;
	Musica *casos_treino;
	int qdeElem;
	int size_treino;
	int nth;
	int index;
	int caso;
} StructParamThread;

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
float calculaEuclidiana(float el1,float el2){
	return pow((el1-el2), 2);	
}
float distanciaMusica(struct Musica caso_teste,struct Musica caso_treino,int qdeElem){
	float distancia=0;
	for(int i=0;i<qdeElem;i++){
		distancia+=calculaEuclidiana(caso_teste.array[i],caso_treino.array[i]);
	}	
	distancia = sqrt(distancia);
	return distancia;
}
void *thread_musica(void *arg){

    StructParamThread *param = (StructParamThread*) arg;
	float distancia;
	for(int i=(*param).index;i<(*param).size_treino;i+=(*param).nth){
		distancia = distanciaMusica((*param).caso_teste,(*param).casos_treino[i],(*param).qdeElem);
		pthread_mutex_lock(&lock);
		if(distancia < menor_distancia){
			menor_distancia = distancia;
			melhor_musica = (*param).casos_treino[i];
		}
	    pthread_mutex_unlock(&lock);
	}
	
	pthread_exit(0);
}

void calcula_menor_distancia(Musica caso_teste, 
							 Musica *casos_treino,
							 int qdeElem,
							 int size_treino,
							 int nth){
	pthread_t t[nth];

	StructParamThread param[nth];
	/*
	Cria N threads
	*/
	for(int j=0;j<nth;j++){
		param[j].caso_teste = caso_teste;
		param[j].casos_treino = casos_treino;
		param[j].qdeElem = qdeElem;
		param[j].size_treino = size_treino;
		param[j].nth = nth;
		param[j].index = j;
		pthread_create(&t[j],NULL,thread_musica,(void*) &param[j]);
	}	
	for(int j=0;j<nth;j++){
    	pthread_join(t[j], NULL);
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
 