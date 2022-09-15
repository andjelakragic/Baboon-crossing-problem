#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define MAX_BR_BABUNA_NA_KONOPCU 5


static int brojLevih = 0;
static int brojDesnih = 0;
static int brojacLevih;
static int brojacDesnih;

static sem_t konopac;
static sem_t leviMultiplex;
static sem_t levi[MAX_BR_BABUNA_NA_KONOPCU];
static sem_t desniMultiplex;
static sem_t desni[MAX_BR_BABUNA_NA_KONOPCU];
static sem_t turnstille;
static sem_t pustajLeve;
static sem_t pustajDesne;
static pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexLevih=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexDesnih=PTHREAD_MUTEX_INITIALIZER;


void *prolaziLeviBabun(void *arg);
void *prolaziDesniBabun(void *arg);
// void enter(pthread_mutex_t *lock, sem_t *can_enter, int counter);
// void leave(pthread_mutex_t *lock, sem_t *can_enter, int counter);

int main()
{
  srand(time(NULL));
  brojLevih = 5;
  brojDesnih = 5;

  printf("%d babuna sa leve strane i %d babuna sa desne strane\n", brojLevih, brojDesnih);
  int i, j, a[brojLevih], b[brojDesnih];
  pthread_t tid1[brojLevih], tid2[brojDesnih];

  sem_init(&leviMultiplex, 0, 5);
  sem_init(&desniMultiplex, 0, 5);
  sem_init(&turnstille, 0, 1);
  sem_init(&konopac, 0, 1);
  sem_init(&pustajDesne,0,1);
  sem_init(&pustajLeve,0,1);

  for (i = 0; i < brojLevih; i++)
  {
    sem_init(&levi[i], 0, 1);
  }
  for (j = 0; j < brojDesnih; j++)
  {
    sem_init(&desni[j], 0, 1);
  }
  for (i = 0; i < brojLevih; i++)
  {
    a[i] = i;
    if (pthread_create(&tid1[i], NULL, prolaziLeviBabun, &(a[i])) != 0)
    {
      perror("Nije kreirana nit\n");
    }
  }
  for (j = 0; j < brojDesnih; j++)
  {
    b[j] = j;
    if (pthread_create(&tid2[j], NULL, prolaziDesniBabun, &(b[j])) != 0)
    {
      perror("Nije kreirana nit\n");
    }
  }
  for (i = 0; i < brojLevih; i++)
  {
    if (pthread_join(tid1[i], NULL) != 0)
    {
      perror("Nije kreirana nit\n");
    }
  }

  for (j = 0; j < brojDesnih; j++)
  {
    if (pthread_join(tid2[j], NULL) != 0)
    {
      perror("Nije kreirana nit\n");
    }
  }

  sem_destroy(&turnstille);
  sem_destroy(&pustajDesne);
  sem_destroy(&pustajLeve);
  sem_destroy(&konopac);
  sem_destroy(&leviMultiplex);
  sem_destroy(&desniMultiplex);

  return 0;
}

void *prolaziLeviBabun(void *arg)
{

  int brojBabuna = *(int *)arg;

  sem_wait(&turnstille);

  pthread_mutex_lock(&mutexLevih);
  // sem_wait(&pustajLeve);
  brojacLevih=1;
  if(brojacLevih==1){
    sem_wait(&konopac);
  }
  // sem_post(&pustajLeve);
  pthread_mutex_unlock(&mutexLevih);

  sem_post(&turnstille);

 pthread_mutex_lock(&mutex);
  sem_wait(&leviMultiplex);
  printf("\n%d babun sa leve strane dolazi, %d brojacLevih, %dbrojacDesnih\n", brojBabuna + 1, brojacLevih,brojacDesnih);

  sem_wait(&levi[brojBabuna]);
  printf("Babun %d prelazi sa leve na desnu stranu\n", brojBabuna + 1);
  sleep(2);
  sem_post(&levi[brojBabuna]);

  sem_post(&leviMultiplex);
  printf("\nPresao babun %d sa leve na desnu stranu, %d brojacLevih, %dbrojacDesnih\n", brojBabuna + 1, brojacLevih, brojacDesnih);
 pthread_mutex_unlock(&mutex);

  pthread_mutex_lock(&mutexLevih);
  // sem_wait(&pustajLeve);
  brojacLevih=brojacLevih-1;
  if(brojacLevih==0){
    sem_post(&konopac);
  }
  // sem_post(&pustajLeve);
  pthread_mutex_unlock(&mutexLevih);

 
}

void *prolaziDesniBabun(void *arg)
{
  int brojBabuna = *(int *)arg;
  
  sem_wait(&turnstille);
  pthread_mutex_lock(&mutexDesnih);
  // sem_wait(&pustajDesne);
  brojacDesnih=1;
if(brojacDesnih==1){
    sem_wait(&konopac);
  }
  // sem_post(&pustajDesne);
  pthread_mutex_unlock(&mutexDesnih);

  sem_post(&turnstille);

 pthread_mutex_lock(&mutex);
  sem_wait(&desniMultiplex);
  printf("\nDolazi %d babun sa desne strane konopca, %d brojacLevih, %d brojacDesnih\n", brojBabuna + 1, brojacLevih,brojacDesnih);

  sem_wait(&desni[brojBabuna]);
  printf("Babun %d prelazi sa desne na levu stranu\n", brojBabuna + 1);
  sleep(2);
  sem_post(&desni[brojBabuna]);
  printf("\nPresao babun %d sa desne na levu stranu, %d brojacLevih, %d brojacDesnih\n", brojBabuna + 1, brojacLevih, brojacDesnih);

  sem_post(&desniMultiplex);
  pthread_mutex_unlock(&mutex);

  pthread_mutex_lock(&mutexDesnih);
  // sem_wait(&pustajDesne);
  brojacDesnih=brojacDesnih-1;
  if(brojacDesnih==0){
    sem_post(&konopac);
  }
  // sem_post(&pustajDesne);
  pthread_mutex_unlock(&mutexDesnih);
 
  
}
