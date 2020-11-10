/*
  * Adapted from the ostep book http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf
  */
#include <stdio.h>
#include <pthread.h>

#define MAX 10


int buffer[MAX];
int posicaoInsercao = 0;
int posicaoLeitura = 0;
int contador = 0;

int produtosTotais = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t livre = PTHREAD_COND_INITIALIZER;
pthread_cond_t preenchido = PTHREAD_COND_INITIALIZER;


void inserirBuffer(int valor) {
     buffer[posicaoInsercao] = valor;
     posicaoInsercao++;
     if (posicaoInsercao == MAX)
         posicaoInsercao = 0; 
     contador++;
}

int lerBuffer() {
     int tmp = buffer[posicaoLeitura];
     posicaoLeitura++;
     if (posicaoLeitura == MAX)
         posicaoLeitura = 0;
     contador--; 
     return tmp; 
}

void *produtor(void *arg) {
     int i;         
     for (i = 0; i < produtosTotais ; i++){
          printf("Produtor tenta lock %d\n",i);
          pthread_mutex_lock(&mutex);
          printf("Produtor verifica se pode produzir %d\n",i);
          while (contador == MAX)
                pthread_cond_wait(&livre,&mutex);
          printf("Produziu! %d\n",i);
          inserirBuffer(i);
          pthread_cond_signal(&preenchido);
          pthread_mutex_unlock(&mutex);
     } 
     return NULL;
}

void *consumidor(void *arg) {
     int i; 
     for (i = 0; i < produtosTotais ; i++){
          printf("Consumidor tenta lock %d\n",i);
          pthread_mutex_lock(&mutex);
          printf("Consumidor verifica se pode consumir %d\n",i);
          while (contador == 0)
                pthread_cond_wait(&preenchido,&mutex);
          printf("Consumiu! %d\n",i);
          int tmp = lerBuffer();
          pthread_cond_signal(&livre);
          pthread_mutex_unlock(&mutex);
          printf("Lido: %d\n",tmp);
     } 
     return NULL;
}
                                                                             
int main(void) {                    
    pthread_t prod,consu;
    pthread_create(&prod, NULL, produtor, NULL); 
    pthread_create(&consu, NULL, consumidor, NULL); 
    pthread_join(prod,NULL);
    pthread_join(consu,NULL);
    return 0;
}

