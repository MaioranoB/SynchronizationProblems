/*
This problem was originally based on the Senate bus at Wellesley College.
Riders come to a bus stop and wait for a bus.
When the bus arrives, all the waiting riders invoke boardBus(), but anyone who arrives while the bus is boarding has to wait for the next bus.
The capacity of the bus is 50 people; if there are more than 50 people waiting, some will have to wait for the next bus.

When all the waiting riders have boarded, the bus can invoke depart().
If the bus arrives when there are no riders, it should depart() immediately.
*/
#include <stdio.h>
#include <pthread.h>
#include<unistd.h> //for sleep func

#define CAPACITY 50
#define MIN(a,b) (((a)<(b)) ? (a):(b))

//DEFAULT VALUES:
int ridersWaiting = 0;
int ridersNumber = 30;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t busArrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t boarded = PTHREAD_COND_INITIALIZER;
/*
mutex protects ridersWaiting, which keeps track of how many riders are waiting;
multiplex makes sure there are no more than 50 riders in the boarding area.
Riders wait on bus, which gets signaled when the bus arrives.
The bus waits on allAboard, which gets signaled by the last student to board.
*/

//http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf

void depart(int ridersNum){
    printf("\t\tBUS DEPART WITH %d RIDERS\n",ridersNum);
}
void *bus(){
    while(1){
        //When the bus arrives, it gets mutex, which prevents late arrivals from entering the boarding area
        pthread_mutex_lock(&mutex);
        int n = MIN(ridersWaiting,CAPACITY);
        printf("\tTHE BUS ARRIVED, %d RIDERS IN LINE WLL ENTER\n",n);
        
        for (int i=0; i<n; i++){
            pthread_cond_signal(&busArrived);
            pthread_cond_wait(&boarded,&mutex);
        }

        depart(n);
        pthread_mutex_unlock(&mutex);
        sleep(2); //melhorar esses valores do sleep e ver se tem alguma outra func pra isso
    }
    return NULL;
}


void board(){
    printf("Rider board.\n");
    ridersWaiting --;
    pthread_cond_signal(&boarded);
}

void *riders(){
    while(1){
        pthread_mutex_lock(&mutex);
        ridersWaiting ++;
        printf("Rider arrived. Waiting for the bus. Total waiting = %d\n",ridersWaiting);
        pthread_cond_wait(&busArrived,&mutex);

        board();
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main(void){

    pthread_t bus_t;
    pthread_create(&bus_t, NULL, bus, NULL);

    pthread_t riders_t[ridersNumber];
    for(int i = 0; i < ridersNumber; i++){
        pthread_create(&riders_t[i], NULL, riders, NULL);
    }

    pthread_join(bus_t,NULL);
    
    return 0;
}
