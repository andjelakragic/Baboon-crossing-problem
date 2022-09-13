#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>


#define MAX_BR_BABUNA_NA_KONOPCU 5
#define MAX 7

typedef struct lightswitch {
  pthread_mutex_t *lock;
  sem_t *can_enter;
  int counter;
} ls_t;

static int brojLevih=0;
static int brojDesnih=0;

static sem_t konopac;
static sem_t leviMultiplex;
static sem_t levi[MAX_BR_BABUNA_NA_KONOPCU];
static sem_t desniMultiplex;
static sem_t desni[MAX_BR_BABUNA_NA_KONOPCU];
static sem_t turnstille;
static sem_t pustajLeve;
static sem_t pustajDesne;
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

// static struct prekidacLevih{
//     ls_t *lightswitch;
//     sem_t *sem;
// };
// static struct prekidacDesnih{
//     ls_t *lightswitch;
//     sem_t *sem;
// };
// funkcije
void *prolaziLeviBabun(void* arg);
void *prolaziDesniBabun(void* arg);
void enter (pthread_mutex_t *lock, sem_t *can_enter, int counter);
void leave (pthread_mutex_t *lock, sem_t *can_enter, int counter);




int main(){
  srand(time(NULL));
  brojLevih=rand()%7;
  brojDesnih=rand()%7;

printf("%d babuna sa leve strane i %d babuna sa desne strane\n",brojLevih,brojDesnih);
    int i,j,a[brojLevih],b[brojDesnih];
	pthread_t tid1[brojLevih],tid2[brojDesnih];
	
	sem_init(&leviMultiplex,0,5);
    sem_init(&desniMultiplex,0,5);
    sem_init(&turnstille,0,0);
    sem_init(&konopac,0,1);
	
	for(i=0;i<brojLevih;i++){
		sem_init(&levi[i],0,1);
    }
    for(j=0;j<brojDesnih;j++){
		sem_init(&desni[j],0,1);
    }		
	for(i=0;i<brojLevih;i++){
		a[i]=i;
		if(pthread_create(&tid1[i],NULL,prolaziLeviBabun,&(a[i]))!=0){
            perror("Nije kreirana nit\n");
        }
	}
    for(j=0;j<brojDesnih;j++){
		b[j]=j;
		if(pthread_create(&tid2[j],NULL,prolaziDesniBabun,&(b[j]))!=0){
            perror("Nije kreirana nit\n");
        }
	}
	for(i=0;i<brojLevih;i++){
		if (pthread_join(tid1[i],NULL)!=0){
            perror("Nije kreirana nit\n");
        }
    }
    	

	for(j=0;j<brojDesnih;j++){
		if (pthread_join(tid2[j],NULL)!=0){
            perror("Nije kreirana nit\n");
        }
    }
    
    sem_destroy(&konopac);
    sem_destroy(&leviMultiplex);
    sem_destroy(&desniMultiplex);

    return 0;
}

void *prolaziLeviBabun(void* arg){

   int brojBabuna=*(int *) arg;
 
    sem_wait(&turnstille);
// prekidac ukljucen
    pthread_mutex_lock (&lock);
    brojLevih+=1;
    if (brojLevih == 1){
    sem_wait (&konopac);}
    pthread_mutex_unlock (&lock);
    
    sem_post(&turnstille);
    sem_wait(&leviMultiplex);
    printf("\n%d babun sa leve strane dolazi\n",brojBabuna+1);

    sem_wait(&levi[brojBabuna]);
    printf("Babun %d prelazi sa leve na desnu stranu\n",brojBabuna+1);
    sleep(rand()%5); 
    sem_post(&levi[brojBabuna]);
   
    sem_post(&leviMultiplex);
   printf("\nPresao babun %d sa leve na desnu stranu\n", brojBabuna+1);
// prekidac iskljucen
   pthread_mutex_lock (&lock);
   brojLevih-=1;
  if (brojLevih == 0){
    sem_post (&konopac);}
  pthread_mutex_unlock (&lock);
  
}

void *prolaziDesniBabun(void* arg){
    int brojBabuna=*(int *) arg;
   
    sem_wait(&turnstille);
    // prelidac ukljucen
     pthread_mutex_lock (&lock);
  if (++brojDesnih == 1){
    sem_wait (&konopac);}
   pthread_mutex_unlock (&lock);
    sem_post(&turnstille);
    
  
    sem_wait(&desniMultiplex);
    printf("\nDolazi %d babun sa desne strane konopca\n",brojBabuna+1);

    sem_wait(&desni[brojBabuna]);
    printf("Babun %d prelazi sa desne na levu stranu\n",brojBabuna+1);
    sleep(rand()%5);
    sem_post(&desni[brojBabuna]);
    printf("\nPresao babun %d sa desne na levu stranu\n", brojBabuna+1);
   
   sem_post(&desniMultiplex); 

   //prekidac iskljucen
    pthread_mutex_lock (&lock);
   if (--brojDesnih == 0){
    sem_post (&konopac);}
  pthread_mutex_unlock (&lock);
    

}
void enter (pthread_mutex_t *lock, sem_t *can_enter, int counter)
{
  pthread_mutex_lock (lock);
  if (++counter == 1){
    sem_wait (can_enter);}
  pthread_mutex_unlock (lock);
}

void leave (pthread_mutex_t *lock, sem_t *can_enter, int counter)
{
  pthread_mutex_lock (lock);
  if (--counter == 0){
    sem_post (can_enter);}
  pthread_mutex_unlock (lock);
}