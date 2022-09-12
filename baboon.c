#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define MAX_BR_BABUNA_NA_KONOPCU 5

/*typedef struct{

	pthread_mutex_t *lock;
	sem_t s;
	int brojac;
}Prekidac;*/

typedef struct
{
	int brojac;
	pthread_mutex_t *prekidac;
} Prekidac;

static int brojacDesnih = 0;
static int brojacLevih = 0;

static sem_t semaforZaCistProlaz;
static sem_t semaforLevih;
static sem_t semaforDesnih;
static sem_t turnstille;
static Prekidac prekidacDesnih;
static Prekidac prekidacLevih;
static pthread_mutex_t p=PTHREAD_MUTEX_INITIALIZER;

// funkcije
void ukljuci(Prekidac *p, sem_t *semafor);
void iskljuci(Prekidac *p, sem_t *semafor);
void *leviProlaze(void *arg);
void *desniProlaze(void *arg);

int main(){

	srand(time(NULL));
	brojacLevih = (rand() % 10);
	brojacDesnih = (rand() % 10);
	printf("Broj babuna sa leve strane %d\n", brojacLevih);
	printf("Broj babuna sa desne strane %d\n", brojacDesnih);
	//pthread_attr_t attribs;
	int i,j;
	int nizLevih[brojacLevih];
    int nizDesnih[brojacDesnih];
	pthread_t t_levih[brojacLevih];
	pthread_t t_desnih[brojacDesnih];
	sem_init(&semaforDesnih, 0, MAX_BR_BABUNA_NA_KONOPCU);
	sem_init(&semaforLevih, 0, MAX_BR_BABUNA_NA_KONOPCU);
	sem_init(&semaforZaCistProlaz, 0, 1);
	sem_init(&turnstille, 0, 0);
	

	
	for (i=0; i< brojacLevih; i++)		
	{
		nizLevih[i] = i;
		if(pthread_create(&(t_levih[i]), NULL, &leviProlaze, &(nizLevih[i]))!=0){
			perror("Neuspesno kreirana nit");
			
		}
	}

	for (j=0; j< brojacDesnih; j++)		
	{
		nizDesnih[j] = j;
		if(pthread_create(&(t_desnih[j]),NULL,&desniProlaze, &(nizDesnih[j]))!=0){
			perror("Neuspesno kreirana nit");
		
		}
	}
	
	for (i=0; i< brojacLevih; i++)		
		if(pthread_join(t_levih[i], NULL)!=0){
			return -1;
		}
	
	for (j=0; j< brojacDesnih; j++)	{	
		if(pthread_join(t_desnih[j], NULL)!=0){
			return -1;
		}
	}
	sem_destroy(&semaforDesnih);
	sem_destroy(&semaforLevih);
	sem_destroy(&semaforZaCistProlaz);
	sem_destroy(&turnstille);

	return 0;
}

/*void ukljuci(pthread_mutex_t *p, sem_t *s, int brojac){
	pthread_mutex_lock(p);
	if(++brojac==0){
	sem_wait(s);
	}
	pthread_mutex_unlock(p);
}

void iskljuci(pthread_mutex_t *p, sem_t *s, int brojac){
	pthread_mutex_lock(p);
	if(--brojac==-1){
	sem_post(s);
	}
	pthread_mutex_unlock(p);
}*/

void ukljuci(Prekidac *p, sem_t *semafor)
{
	pthread_mutex_lock(p->prekidac);
	p->brojac += 1;
	if (p->brojac == 0)
	{
		sem_post(semafor);
	}
	pthread_mutex_unlock(p->prekidac);
}
void iskljuci(Prekidac *p, sem_t *semafor)
{
	pthread_mutex_lock(p->prekidac);
	p->brojac -= 1;
	if (p->brojac == 0)
	{
		sem_wait(semafor);
	}
	pthread_mutex_unlock(p->prekidac);
}
/*	FUNKCIJA KOJA ZOVE I UKLJUCIVANJE I ISKLJUCIVANJE PREKIDACA

	void* prekidac(void *podatak){
	Prekidac *prekidac= (struct arg*)podatak;
	lock(&prekidac->lock,&prekidac->s,prekidac->brojac);
	unlock(&prekidac->lock,&prekidac->s,prekidac->brojac);
}*/

void *leviProlaze(void *arg)
{
	prekidacLevih.brojac = brojacLevih;
	prekidacLevih.prekidac = (pthread_mutex_t *)&semaforLevih;
	int indexBabuna = *(int *)arg;
	sem_wait(&turnstille);
	ukljuci(&prekidacLevih, &semaforZaCistProlaz);
	sem_post(&turnstille);

	printf("Dolazi babun sa leve strane\n");
	sem_wait(&semaforLevih);

		//struct arsgs *args=(struct args *) arg;		
		// int in,out;
		// for (int i = 0; i < MAX_BR_BABUNA_NA_KONOPCU; i++){
  		// 	if (pthread_mutex_trylock (incoming_mutex[i]))
    	// 	{
      	// 		in = i;
      	// 		break;
    	// 	}
		// }
	printf(" Prelazi babun sa leve na desnu stranu\n");
	sleep(rand() % 4);
		// for (int i = 0; i < MAX_BR_BABUNA_NA_KONOPCU; i++){
  		// if (pthread_mutex_trylock (outgoing_mutex[i]))
    	// 	{
     	//  	 out = i;
      	// 	 break;
    	// }	
		// }
		// pthread_mutex_unlock (incoming_mutex[in]);
		// pthread_mutex_unlock (outgoing_mutex[out]);
	sem_post(&semaforLevih);
	printf("Presao babun na desnu stranu\n");
	iskljuci(&prekidacLevih, &semaforZaCistProlaz);

	free(arg);
}
void *desniProlaze(void *arg)
{
	prekidacDesnih.brojac = brojacDesnih;
	prekidacDesnih.prekidac = (pthread_mutex_t *)&semaforDesnih;
	int indexBabuna = *(int *)arg;
	sem_wait(&turnstille);
	ukljuci(&prekidacDesnih, &semaforZaCistProlaz);
	sem_post(&turnstille);
	printf("Dolazi babun sa desne strane\n");
	sem_wait(&semaforDesnih);
	// int in=arg->in,out=(in+1)%5;
	// if(in<out && out<=5){
		printf(" Prelazi babun sa desne na levu stranu\n");
		sleep(rand() % 4);
	
	sem_wait(&semaforDesnih);
	
	
	// 	for (int i = 0; i < MAX_BR_BABUNA_NA_KONOPCU; i++){
  	// 	if (pthread_mutex_trylock (outgoing_mutex[i]))
    // 		{
    //  	 	 out = i;
    //   		 break;
    // 	}	
	// 	}
	// 	pthread_mutex_unlock (incoming_mutex[in]);
	// 	pthread_mutex_unlock (outgoing_mutex[out]);
	sem_post(&semaforDesnih);
	printf("Presao babun na levu stranu\n");
	iskljuci(&prekidacDesnih, &semaforZaCistProlaz);

	free(arg);
}