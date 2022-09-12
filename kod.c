#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>


#define MAX_BR_BABUNA_NA_KONOPCU 5
#define MAX 15

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
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static ls_t prekidacLevih;
static ls_t prekidacDesnih;
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

    int i,j,a[MAX];
	pthread_t tid[MAX];
	
	sem_init(&leviMultiplex,0,5);
    sem_init(&desniMultiplex,0,5);
    sem_init(&turnstille,0,0);
    sem_init(&konopac,0,1);
	
	for(i=0;i<MAX;i++){
		sem_init(&levi[i],0,1);
    }
    for(j=0;j<MAX;j++){
		sem_init(&desni[j],0,1);
    }		
	for(i=0;i<MAX;i++){
		a[i]=i;
		if(pthread_create(&tid[i],NULL,prolaziLeviBabun,&(a[i]))!=0){
            perror("Nije kreirana nit\n");
        }
	}
    for(j=0;j<MAX;j++){
		a[j]=j;
		if(pthread_create(&tid[j],NULL,prolaziDesniBabun,&(a[j]))!=0){
            perror("Nije kreirana nit\n");
        }
	}
	for(i=0;i<MAX;i++){
		if (pthread_join(tid[i],NULL)!=0){
            perror("Nije kreirana nit\n");
        }
    }
    	

	for(j=0;j<MAX;j++){
		if (pthread_join(tid[j],NULL)!=0){
            perror("Nije kreirana nit\n");
        }
    }
    
    sem_destroy(&konopac);
    sem_destroy(&leviMultiplex);
    sem_destroy(&desniMultiplex);

    return 0;
}

void *prolaziLeviBabun(void* arg){
    sem_wait(&turnstille);
    pthread_mutex_lock (&lock);
  if (++brojLevih == 1){
    sem_wait (&konopac);}
  pthread_mutex_unlock (&lock);
    sem_post(&turnstille);
    int brojBabuna=*(int *) arg;
    
    sem_wait(&leviMultiplex);
    printf("\n%d babun sa leve strane dolazi\n",brojBabuna+1);

    sem_wait(&levi[brojBabuna]);
    printf("Babun %d prelazi sa leve na desnu stranu\n",brojBabuna+1);
    sleep(2);
    sem_post(&levi[brojBabuna]);
    printf("\nPresao babun %d sa leve na desnu stranu\n", brojBabuna+1);


   pthread_mutex_lock (&lock);
  if (--brojLevih == 0){
    sem_post (&konopac);}
  pthread_mutex_unlock (&lock);
   sem_post(&leviMultiplex);
}

void *prolaziDesniBabun(void* arg){
    sem_wait(&turnstille);
    
     pthread_mutex_lock (&lock);
  if (++brojDesnih == 1){
    sem_wait (&konopac);}
  pthread_mutex_unlock (&lock);
    sem_post(&turnstille);
    
    int brojBabuna=*(int *) arg;
    sem_wait(&desniMultiplex);
    printf("\nDolazi %d babun sa desne strane konopca\n",brojBabuna+1);

    sem_wait(&desni[brojBabuna]);
    printf("Babun %d prelazi sa desne na levu stranu\n",brojBabuna+1);
    sleep(2);
    sem_post(&desni[brojBabuna]);
    printf("\nPresao babun %d sa desne na levu stranu\n", brojBabuna+1);
    
    pthread_mutex_lock (&lock);
  if (--brojDesnih == 0){
    sem_post (&konopac);}
  pthread_mutex_unlock (&lock);
    sem_post(&desniMultiplex);

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