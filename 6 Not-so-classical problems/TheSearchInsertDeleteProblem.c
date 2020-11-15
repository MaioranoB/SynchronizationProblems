/*
Three kinds of threads share access to a singly-linked list: searchers, inserters and deleters.
Searchers merely examine the list; hence they can execute concurrently with each other.
Inserters add new items to the end of the list; insertions must be mutually exclusive to preclude two inserters from inserting new items at about the same time.
However, one insert can proceed in parallel with any number of searches.
Finally, deleters remove items from anywhere in the list.
At most one deleter process can access the list at a time, and deletion must also be mutually exclusive with searches and insertions.
*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//DEFAULT VALUES:
int searchersNum = 5;
int insertersNum = 3;
int deletersNum  = 2;

pthread_rwlock_t ReaderWriterLOCK = PTHREAD_RWLOCK_INITIALIZER; //reader-writer lock variable handling

void *searchers(){
     while(1){
          //Try to acquire read lock for RWLOCK.
          pthread_rwlock_tryrdlock(&ReaderWriterLOCK);
          printf("SEARCHING\n");
          pthread_rwlock_unlock(&ReaderWriterLOCK);
          sleep(1);
     }
     return NULL;
}

void *inserters(){
    while(1){
         //Try to acquire write lock for RWLOCK.
         pthread_rwlock_trywrlock(&ReaderWriterLOCK);
         printf("\tINSERTING\n");
         pthread_rwlock_unlock(&ReaderWriterLOCK);
         sleep(1);
     }
     return NULL;
}

void *deleters(){
     while(1){
          //Try to acquire write lock for RWLOCK.
          pthread_rwlock_trywrlock(&ReaderWriterLOCK);
          printf("\t\tDELETING\n");
          pthread_rwlock_unlock(&ReaderWriterLOCK);
          sleep(1);
     }
     return NULL;
}

int main(void){
     printf("Number of searchers: %d\n", searchersNum);
     printf("Number of inserters: %d\n", insertersNum);
     printf("Number of deleters:  %d\n\n", deletersNum);
     
     pthread_t searchers_t[searchersNum];
     for(int i = 0; i < searchersNum; i++){
          pthread_create(&searchers_t[i], NULL, searchers, NULL);
     }

     pthread_t inserters_t[insertersNum];
     for(int i = 0; i < insertersNum; i++){
          pthread_create(&inserters_t[i], NULL, inserters, NULL);
     }

     pthread_t deleters_t[deletersNum];
     for(int i = 0; i < deletersNum; i++){
          pthread_create(&deleters_t[i], NULL, deleters, NULL);
     }

     //will wait forever for thread searchers_t[0] to end
     pthread_join(searchers_t[0],NULL);
     return 0;
}
