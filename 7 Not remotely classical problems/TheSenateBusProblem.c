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
#include<unistd.h>
#include <stdlib.h>
#include<time.h> 

#define CAPACITY 50 //capacity of the bus
#define MIN(a,b) (((a)<(b)) ? (a):(b))

int ridersWaiting = 0; //keeps track of how many riders are waiting in the boarding area
int ridersNumber = 80; //number of riders

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex protects ridersWaiting (prevent concurrent access)
pthread_cond_t busArrived = PTHREAD_COND_INITIALIZER; //busArrived signals when the bus has arrived
pthread_cond_t boarded = PTHREAD_COND_INITIALIZER; //boarded signals that a rider has boarded

void sleepRandom(int min, int max){
    sleep((rand()%(max-min+1))+min);
}

void depart(int departNum){
    if (departNum == 0){
        printf("\t\tBUS DEPART IMMEDIATELY\n");
    }
    else{
        printf("\t\tBUS DEPART WITH %d RIDERS\n",departNum);
    }
}
void *bus(){
    while(1){
        //When the bus arrives, it gets mutex, which prevents late arrivals from entering the boarding area
        pthread_mutex_lock(&mutex);

        //Prevents more than the capacity of the bus from boarding
        int n = MIN(ridersWaiting,CAPACITY);
    
        if (n == 0){
            printf("\tTHE BUS ARRIVED. NO RIDERS IN THE BOARDING AREA\n");
        }
        else{
            printf("\tTHE BUS ARRIVED, %d RIDERS IN LINE WLL ENTER\n",n);
        }
        
        //The loop signals each rider in turn and waits for him to board
        for (int i=0; i<n; i++){
            pthread_cond_signal(&busArrived);
            pthread_cond_wait(&boarded,&mutex);
        }

        depart(n);
        pthread_mutex_unlock(&mutex);
        sleepRandom(3,5); //next bus in some random seconds
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
        sleepRandom(2,10); //riders arrives at different times
        pthread_mutex_lock(&mutex);
        
        //Riders wait on bus until the bus arrives
        printf("Rider arrived. Waiting for the bus. Total waiting = %d\n", ++ridersWaiting);
        pthread_cond_wait(&busArrived,&mutex);

        board();
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void){
    printf("Capacity of the bus: %d\n", CAPACITY);
    printf("Number of riders: %d\n\n", ridersNumber);
    srand(time(0)); //current time as seed

    pthread_t bus_t;
    pthread_create(&bus_t, NULL, bus, NULL);

    pthread_t riders_t[ridersNumber];
    for(int i = 0; i < ridersNumber; i++){
        pthread_create(&riders_t[i], NULL, riders, NULL);
    }

    //wait forever for bus thread to end
    pthread_join(bus_t,NULL);
    return 0;
}
