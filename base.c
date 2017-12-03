#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

float *v1,*v2,*r;
long int nth,tam;
/*escreve vetor na tela*/
void printv(float v[],int n){
	long int i;
	printf("[");
	for(i=0;i<n;i++){
		printf("%.1f ",v[i]);
	}
	printf("]\n");
}
/*soma vetor*/
void *soma(void *arg){
	long int tid = (long int)arg;
	long int i;
	for(i=tid;i<tam;i+=nth){
		r[i]=v1[i]+v2[i];
	}
}

int main(int argc, char *argv[]){
	long int i;
	tam=atoi(argv[1]); nth=atoi(argv[2]);
	pthread_t *t = (pthread_t *) malloc (nth * sizeof(pthread_t));/*aloca a as esturuturas
															das threads que serao usadas */
	v1 = (float*) malloc (tam * sizeof(float));/*aloca v1 dinamicamente*/
	v2 = (float*) malloc (tam * sizeof(float));/*aloca v2 dinamicamente*/
	r  = (float*) malloc (tam * sizeof(float));/*aloca o vetor resposta dinamicamente*/
	
	/*preenche os vetores de maneira aleatoria*/
	for(i=0;i<tam;i++){
		v1[i]=rand()%100;
		v2[i]=rand()%100;
	}
	/*cria thrads*/
	for(i=0;i<nth;i++){
		pthread_create(&t[i],NULL,soma,(void*)i);
	}
	/*aguarda as threads encerrarem a execucao*/
	for(i=0;i<nth;i++){
		pthread_join(t[i],NULL);
	}	
}