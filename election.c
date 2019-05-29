#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

int N;
typedef struct rank{
	int tid;
	int best_rank;
	pthread_mutex_t lock;
}rank_t;

typedef struct thread_t{
	int id;
	int rank;
}thread_t;

thread_t *th_rank;

rank_t rank;

static void *election_threads(void *arg){
	pthread_detach(pthread_self());
	thread_t *th_r = (thread_t *)arg;	
	int id = th_r->id;
	int r = th_r->rank;
	pause();
	//printf("I'm thread %d with rank %d \n",id, r);
	
	pthread_mutex_lock(&rank.lock);
		if(r >= rank.best_rank){
			rank.tid = id;
			rank.best_rank = r;
			printf("I'm the leader thread: thread %d with best_rank %d \n",id, r);
		} else{ printf("Thread %d: leader: thread %d with best_rank %d \n",id, rank.tid, rank.best_rank); }
	pthread_mutex_unlock(&rank.lock);	
	printf("\n");
	pthread_exit((void *)pthread_self());
}

void handler(int sig){
	return;
}

void init(){
	th_rank = (thread_t *)malloc(N * sizeof(thread_t));
	rank.best_rank = 0;
	pthread_mutex_init(&rank.lock, NULL);
}

int main(int argc, char *argv[]){
	if(argc!=2){
		printf("usage: %s nthreads\n",argv[0]);
		exit(1);
	}
	N = atoi(argv[1]);
	if(N<=8){
		printf("N should be > 8\n");
		exit(1);
	}
	pthread_t *th_el;
	init();
	signal(SIGUSR1, handler);
	
	th_el = (pthread_t *)malloc(N * sizeof(pthread_t));
		
	int i, *pi;
	
	srand(time(NULL));
	for(i=0; i<N; i++){
		
		pi = (int *)malloc(sizeof(int)); 
		*pi = i;
		th_rank[i].id = *pi;
		th_rank[i].rank = ((rand() % N)+1);
		printf("THREAD %d RANK: %d\n",th_rank[i].id, th_rank[i].rank);
		pthread_create(&th_el[i], NULL, election_threads, (void *)&th_rank[i]);
		sleep(2);
	}
	/*START ELECTION CAMPAIGN*/
	sleep((rand() % 5) + 2);	
	int *thread_id;
	
	printf("******************THREADS SELECTED****************\n");
	thread_id = (int *)malloc((N-3) * sizeof(int));
	
	srand(time(NULL));
	for(i = 0; i < N-3; i++){	
		thread_id[i] = ((rand() % N ));
		printf(" %d ",thread_id[i]);
		sleep(2);
	}
	printf("\n");
	for(i = 0; i < N-3; i++){
		pthread_kill(th_el[thread_id[i]], SIGUSR1);
	}
	
	/*signal remaining 3 threads*/
	for(i = 0; i < N; i++){
		pthread_kill(th_el[i], SIGUSR1);
	}
	
	pthread_exit(0);
	
	
}

