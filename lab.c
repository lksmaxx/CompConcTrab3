#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#include "timer.h"

#define N_THREADS 4
#define CHAR_LIMIT 100

int N,M;//tamanhos dos blocos de leitura do arquivo e de armazenamento no buffer
long long int s;//quantidade de numeros no arquivo

int *fns_blocks ,fns_threads[2];

pthread_cond_t cond1,cond2,cond_bloco;
pthread_mutex_t mt;

int debug = 0;

typedef struct
{
	int* v;
	int size,n_blocks,r_blocks;
}buffer;


typedef struct//argumentos da thread "file_handler"
{
	FILE *file;
	buffer *buffer;
}fh_args;

typedef struct
{
	int* fns_blocks;
	int printa;
	buffer *buffer;
}tp_args;

typedef struct
{
	int printa ;
	buffer *buffer;
}sq_args;

typedef struct
{
	int printa;
	buffer *buffer;
}sc_args;


float teste(int argc,char* argv[],float a_ratio,float b_ratio,int printa);
void test(const char* msg);//debug

void* file_handler(void* argt);//thread que cuida da leitura do arquivo e da escrita no buffer
void* triplets(void* argt);//thread que cuida das triplas
void* seq_eq(void* argt);
void* seq_cres(void* argt);


int main(int argc,char* argv[])
{
	if(argc < 2) exit(-1);
	double tempo = 0;
	int n_tests = 10;
	float a = 50,b = 10;
	printf("----DEMONSTACAO-----\n");
	teste(argc,argv,10,5,1);//demonstracao
	printf("-----------------------\n");
	//alguns testes
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio  : %f\n",tempo/n_tests);
	tempo = 0;
	a = 20,b = 5;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	tempo = 0;
	a = 2,b = 5;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	tempo = 0;
	a = 2,b = 10;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	tempo = 0;
	a = 1,b = 10;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	tempo = 0;
	a = 1,b = 2;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	tempo = 0;
	a = 1,b = 1;
	printf("teste(n_tests: %d,N:size/%f,M:N/%f)\n",n_tests,a,b);
	for(int i = 0; i < n_tests; i++)
	{
		tempo += teste(argc,argv,a,b,0);
	}
	printf("tempo medio : %f\n",tempo/n_tests);
	
	return 0;
}

float teste(int argc,char* argv[],float a_ratio, float b_ratio,int printa)
{
	
	FILE *file = fopen(argv[1],"r+b");
	
	if(!fread(&s,sizeof(long long int),1,file) )
	{
		printf("ERRO: LEITURA DO ARQUIVO \n");
		exit(-1);
	}
	N = floor(s/a_ratio);
	if(N <= 0) N = 1;
	M = floor(N/b_ratio);
	if(M <= 0) M = 1;
	if(printa)printf("numero de elementos: %lld\n",s);
	buffer bf;
	bf.v = malloc(sizeof(int)*s);
	bf.n_blocks = ceil((float)s/M);
	bf.size = M;
	if(printa)printf("valores de N,M: %d,%d\n",N,M);
	
	
	pthread_t t_ids[N_THREADS];
	pthread_mutex_init(&mt,NULL);
	
	fns_threads[0] = 0;
	fns_threads[1] = 0;

	
	fns_blocks = malloc(sizeof(int)*bf.n_blocks);
	for(int i = 0;i < bf.n_blocks;i++)
		fns_blocks[i] = 0;
	double t_inicio,t_fim;
	GET_TIME(t_inicio);
	//cria a thread leitora (le o arquivo e escreve no buffer)
	fh_args t0_args;
	t0_args.file = file;
	t0_args.buffer = &bf;
	pthread_create(&t_ids[0],NULL,file_handler,(void*) &t0_args);

	//cria a thread de sequencia iguais
	sq_args t1_args;
	t1_args.buffer = &bf;
	t1_args.printa = printa;
	pthread_create(&t_ids[1],NULL,seq_eq,(void*)&t1_args);
	
	//cria a thread de sequencias triplas
	tp_args t2_args;
	t2_args.buffer = &bf;
	t2_args.printa = printa;
	t2_args.fns_blocks = malloc(sizeof(int) * bf.n_blocks);
	pthread_create(&t_ids[2],NULL,triplets,(void*) &t2_args);

	//cria a thread de sequencias crescentes
	sc_args t3_args;
	t3_args.buffer = &bf;
	t3_args.printa = printa;
	pthread_create(&t_ids[3],NULL,seq_cres,(void*)&t3_args);
	
	//print_arrayi(buffer,s);
	//printf("%lld \n",s);
	test("join");
	for(int i = 0; i < 4; i++) pthread_join(t_ids[i],NULL);
	
	GET_TIME(t_fim);
	if(printa)printf("Tempo: %f\n",t_fim - t_inicio);
	pthread_mutex_destroy(&mt);
	fclose(file);
	test("final");

	//free(fns_blocks);
	if(printa)printf("FIM\n");
	return t_fim - t_inicio;
}

void test(const char*msg)
{
	if(debug)
	{
		static int point = 0;
		printf("point%d:%s\n", point++,msg);
	}
}

void* file_handler(void* argt)//thread leitora
{
	test("inicio file_handler");
	fh_args args = *(fh_args*) argt;
	FILE *file = args.file;
	buffer* bf = args.buffer;
	int* tb = malloc(sizeof(int) * M );

	test("entrada do while");
	int c_blc = 0;
	while(c_blc < bf->n_blocks)
	{
		int index = c_blc*bf->size;
		int lidos = 0;
		int temp = 1;
		while(lidos < M - N && temp != 0)
		{
			temp = fread((tb + lidos),sizeof(int),N,file);
			lidos+= temp;		
		}
		if(temp != 0)lidos += fread((tb + lidos),sizeof(int),M%N,file);

		if(lidos == 0) break;
		
		int i =0;
		for(; i < lidos && i < bf->size; i++)
			bf->v[i + index] = tb[i];
		if(i == bf->size) bf->r_blocks++;
		if(c_blc != bf->r_blocks) 
		{
			fns_blocks[c_blc] = 1;
			pthread_cond_broadcast(&cond_bloco);

			//printf("bloco %d finalizado\n",c_blc);
			c_blc++;
		}

		if(lidos == 0){printf("Erro ao ler o arquivo\n");pthread_exit(NULL);}
	}
	free(tb);

	fns_blocks[c_blc] =1;
	pthread_cond_broadcast(&cond_bloco);

	//printf("BLOCO FINAL %d\n",bf->readed_blocks);


	test("fim da file_handler");
	pthread_exit(NULL);	
}



void* seq_eq(void* argt)
{
	test("inicio seq_eq");
	sq_args args = *(sq_args*) argt;
	buffer* bf = args.buffer;
	int printa  = args.printa;
	int maior = -1,seq = 1,element = -1,last = -1,pos = -1,c_blc =0,pos_maior = -1;
	while(c_blc < bf->n_blocks)
	{
		int index = c_blc*bf->size;
		pthread_mutex_lock(&mt);
		if(!fns_blocks[c_blc]) pthread_cond_wait(&cond_bloco,&mt);
		pthread_mutex_unlock(&mt);
		fns_blocks[c_blc] = 1;

		int *v = &bf->v[index];
		for(int i = 0; i < bf->size;i++)
		{
			if(last == v[i]) seq++;
			else {seq = 1;pos = i+index;}
			if(seq > maior){maior = seq;element = last; pos_maior = pos;}
			last = v[i];
		}
		c_blc++;
	}
	if(printa)printf("Maior sequencia de valores identicos: %d %d %d \n",pos_maior,maior,element);
	pthread_cond_broadcast(&cond_bloco);
	fns_threads[0] = 1;
	pthread_cond_broadcast(&cond1);

	test("fim seq_eq");
	pthread_exit(NULL);
}


void* triplets(void* argt)
{
	test("inicio triplets");
	tp_args args = *(tp_args*) argt;
	buffer *bf = args.buffer;
	int printa = args.printa;
	int c_blc = 0,element,seq = 0;
	int total = 0;
	test("triplets while");
	while(c_blc < bf->n_blocks)
	{
		int index = c_blc*bf->size;

		//printf("esperando bloco %d\n",c_blc);
		//printf("teste %d\n",fns_blocks[c_blc]);
		pthread_mutex_lock(&mt);
		if(fns_blocks[c_blc] == 0) pthread_cond_wait(&cond_bloco,&mt);
		pthread_mutex_unlock(&mt);
		fns_blocks[c_blc] = 1;

		//printf("iniciando bloco %d\n",c_blc);
		int* v = &bf->v[index] ;

		for(int i = 0; i < bf->size; i++)
		{
			if(element == v[i]) seq++;
			if(seq == 3)
			{
				total++;
				seq = 0;
			}
			element = v[i];
		}
		//printf("trip fim do bloco %d\n",c_blc);
		c_blc++;
	
		
	}	
	pthread_cond_broadcast(&cond_bloco);
	test("trip esperando");
	//printf("t test\n");
	pthread_mutex_lock(&mt);
	if(fns_threads[0] == 0)pthread_cond_wait(&cond1,&mt);
	pthread_mutex_unlock(&mt);

	if(printa)printf("Quantidade de ocorrencias de sequencias contınuas de tamanho 3 do mesmo valor: %d\n", total);
	
	fns_threads[1] = 1;
	pthread_cond_broadcast(&cond2);
	test("fim triplets");
	pthread_exit(NULL);
}






void* seq_cres(void* argt)
{
	test("inicio seq_cres");
	sc_args args = *(sc_args*) argt;
	buffer *bf = args.buffer;
	int printa = args.printa;
	int last = -9,seq = 0,qtd = 0,c_blc = 0;
	while(c_blc < bf->n_blocks)
	{
		int index = c_blc*bf->size;
		//printf("cres esperando bloco %d\n",c_blc);
		//printf("teste %d\n",fns_blocks[c_blc]);
		pthread_mutex_lock(&mt);
		if(fns_blocks[c_blc] == 0) pthread_cond_wait(&cond_bloco,&mt);
		pthread_mutex_unlock(&mt);
		//printf("cres iniciando bloco %d\n",c_blc);
		int* v = &bf->v[index];
		for(int i = 0; i < bf->size; i++)
		{
			test("cres bloco");
			if(v[i] == 0) seq = 0;
			else if(last + 1 == v[i]) seq++;
			else seq = 0;
			
			if(seq == 5) qtd++;
			last = v[i];
		}
		c_blc++;
		
	}
	test("seq_cres esperando");

	//printf("cres test\n");
	pthread_mutex_lock(&mt);
	if(fns_threads[1] == 0) pthread_cond_wait(&cond2,&mt);
	pthread_mutex_unlock(&mt);
	if(printa)printf("Quantidade de ocorrencias da sequencia <012345>: %d\n",qtd);
	test("fim seq_cres");
	pthread_exit(NULL);
}

















