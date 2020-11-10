/*
A tribe of savages eats communal dinners from a large pot that can hold M servings of stewed missionary.
When a savage wants to eat, he helps himself from the pot, unless it is empty.
If the pot is empty, the savage wakes up the cook and then waits until the cook has refilled the pot

The synchronization constraints are:
•Savages cannot invoke getServingFromPot() if the pot is empty.
•The cook can invoke putServingsInPot() only if the pot is empty.
*/
#include <stdio.h>
#include <pthread.h>

//DEFAULT VALUES:
int M = 5; //How much servings the pot can hold
int POT = 0; //Servings in the pot currently
int savagesNumber = 5; //Number of saveges
int savageHungry = 6; //How much each savage needs to eat

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutexPot
//pthread_mutex_t mutexCook = PTHREAD_MUTEX_INITIALIZER; //Cozinheiro deve ser acessado unicamente por um único selvagem

pthread_cond_t emptyPot = PTHREAD_COND_INITIALIZER;
pthread_cond_t fullPot = PTHREAD_COND_INITIALIZER;

void putServingsInPot(){
     POT = M;
     printf("\nThe cook filled the pot!\n\n");
}
void *cook(){
     //faz até acabar o programa... ou seja, até todos os selvagens ficarem sem fome
     while(1){
          pthread_mutex_lock(&mutex);
          //Wait for the pot to be empty
          while (POT != 0){
               //printf("Cook is waiting\n");
               pthread_cond_wait(&emptyPot, &mutex);    
          } 
          //Fill the POT
          putServingsInPot();

          //Wake up all savages waiting for fullPot and unlock the mutex
          pthread_cond_broadcast(&fullPot);
          pthread_mutex_unlock(&mutex);
     }
    return NULL;
}

void getServingFromPot(int id){
     printf("Savage[%d] is getting one serving from the pot\n", id);
     POT --;
}
void eat(int id, int hungry){
     if (hungry == 0){
          printf("Savage[%d]: is eating, need no more\n",id);
     }
     else{
          printf("Savage[%d]: is eating, need %d more\n",id,hungry);
     }
}

void *savage(void *arg){
     //Variables of each specific savage thread
     int this_savageID = *(int *)arg;
     int this_savageHungry = savageHungry;

     //While the savage is hungry
     while(this_savageHungry){
          pthread_mutex_lock(&mutex);
          while (POT == 0){
               printf("Savage[%d]: The pot is empty. I need food\n", this_savageID);
               pthread_cond_signal(&emptyPot);
               pthread_cond_wait(&fullPot,&mutex);    
          }
          getServingFromPot(this_savageID);
          //Unlock the mutex and eat(dont need the mutex to eat)
          pthread_mutex_unlock(&mutex);
          eat(this_savageID, --this_savageHungry);
     }
     return NULL;
}


                                                                             
int main(void) {
     printf("There are %d savages that need to eat %d times\n",savagesNumber, savageHungry);
     printf("The POT have %d initial servings\n\n", POT);

     pthread_t cook_t;
     pthread_create(&cook_t, NULL, cook, NULL);

     pthread_t savage_t[savagesNumber];
     int savageIDs[savagesNumber];
     for(int i = 0; i < savagesNumber; i++){
          savageIDs[i] = i+1; //start at index 1
          pthread_create(&savage_t[i], NULL, savage, (void *)&savageIDs[i]); 
     }

     //Waits for all savages threads to end
     for (int i = 0; i < savagesNumber; i++){
          pthread_join(savage_t[i],NULL);
     }
     return 0;
}
