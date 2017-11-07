/************************* MULTIPLE PRODUCE AND MULTIPLE CONSUMER PROBLEM ***************************
* ----------------------------------------------------------------------------------------------------
*  An attempt to implement the producer-consumer problem through own set of semaphore library methods.
*  Executable takes five arguments exactly in the following order:- 
*  Number_Producers - Integer variable
*  Number_Consumers - Integer variable
*  Max_Sleep_Seconds - Integer variable
*  Total_Number_Items2Produce - Integer variable
*  Ring_Buffer_Size - Integer variable
*  Producer or Consumer can sleep inbetween 1 second to Max_Sleep_Seconds.
*  The program has some bugs. Since the semaphore implementation also does not gurrantee deadlock or
*  starvation problem. These problems could be encountered in this problem in custom set of inputs.
* ----------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "semaphore.h"

/*Ring Buffer structure*/
struct RingBuffer {
	int *buffer;
	int size;
	unsigned int head;
	unsigned int tail;
	struct semaphore *semaphore_mutex;
	struct semaphore *semaphore_fillCount;
	struct semaphore *semaphore_emptyCount;
};

/* Global Buffer Size*/
int *BUFFER_SIZE;

/* Function to read from Buffer*/
int read_Buffer(struct RingBuffer*);

/* Function to add to Buffer*/
void add_Buffer(struct RingBuffer*, int);

/* Initializing Ring Buffer with all the seamphore and allocating memory to ring buffer*/
struct RingBuffer* initialize_RingBuffer(int buffer_S) {
	struct RingBuffer *s_area;
	s_area = (struct RingBuffer *) malloc(sizeof(struct RingBuffer));
	s_area->size = buffer_S;
	s_area->buffer = malloc(buffer_S);
	s_area->head = 0;
	s_area->tail = 0;
	s_area->semaphore_emptyCount = initialize_Semaphore(buffer_S);
	s_area->semaphore_mutex = initialize_Semaphore(1);
	s_area->semaphore_fillCount = initialize_Semaphore(0);
	return s_area;
}

/* Sum to calculate the reult sum for both producer and consumer */
unsigned long long producer_Sum = 0;
unsigned long long consumer_Sum = 0;

/* Global value for sleep seconds and count to check the total value produced */
int *sleep_seconds;
int *count;

/* Segmentation Fault occurence */
void segfault_sigaction(int signal, siginfo_t *si, void *arg) {
	printf("Caught segfault at address %p\n", si->si_addr);
	exit(0);
}

/* A shared strucutre for producer and consumer threads taking Ring Buffer, thread id and each thread to produce/consume from this. */
struct producerThread {
	struct RingBuffer *ringbuff;
	int thread_id;
	int each_item;
};

/* A shared strucutre for producer and consumer threads taking Ring Buffer, thread id and each thread to produce/consume from this. */
struct consumerThread {
	struct RingBuffer *ringbuff;
	int thread_id;
	int each_item;
};

/* Producer Thread for producing items and placnig then in ring buffer */
void * producer(void * arg) {
	struct producerThread *pObj = (struct producerThread*) arg;
	int i;
	for (i = 0; i < pObj->each_item; i++) {
		sleep(*sleep_seconds);
		P(pObj->ringbuff->semaphore_emptyCount);
		P(pObj->ringbuff->semaphore_mutex);
		while ((pObj->ringbuff->head - pObj->ringbuff->tail) == (pObj->ringbuff->size)) {
			/* Error, buffer is full */
		}
		unsigned index = pObj->ringbuff->head % (pObj->ringbuff->size);
		pObj->ringbuff->buffer[index] = pObj->thread_id;
		pObj->ringbuff->head++;
		producer_Sum++;
		V(pObj->ringbuff->semaphore_fillCount);
		V(pObj->ringbuff->semaphore_mutex);

		/* Print function.. Uncomment below line to check for each producer thread producing till the items are produced finally  */
		// printf("Produced:%d - %llu\n", pObj->thread_id, producer_Sum);
	}
	return NULL;
}

/* Consumer function for consuming products put in by buffer */
void * consumer(void * arg) {
	struct consumerThread *cObj = (struct consumerThread*) arg;
	int i;
	for (i = 0; i < cObj->each_item; i++) {
		P(cObj->ringbuff->semaphore_fillCount);
		P(cObj->ringbuff->semaphore_mutex);
		while ((cObj->ringbuff->head - cObj->ringbuff->tail) == 0) {
			/* Error: buffer is empty */
		}
		unsigned index = cObj->ringbuff->tail % (cObj->ringbuff->size);
		cObj->ringbuff->tail++;
		int tmp = cObj->ringbuff->buffer[index];
		consumer_Sum++;
		V(cObj->ringbuff->semaphore_emptyCount);
		V(cObj->ringbuff->semaphore_mutex);
		sleep(*sleep_seconds);
		
		/* Print function.. Uncomment below line to check for each consumer thread consuming till the items are consumed all  */
		// printf("consumed: %d - %llu\n", cObj->thread_id, consumer_Sum);
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 6) {
		printf("Error in command line arguments\nResupply\n");
		exit(0);
	}

	int i = 1;
	int total_Producers = atoi(argv[i++]);
	if (total_Producers <= 0) {
		printf("Sorry No producers. What do you want.\nPlease Resupply\n");
		exit(0);
	}
	int total_Consumers = atoi(argv[i++]);
	if (total_Consumers <= 0) {
		printf("Sorry No consumer. What do you want.\nPlease Resupply\n");
		exit(0);
	}
	int max_SleepSeconds = atoi(argv[i++]);
	if (max_SleepSeconds <= 0) {
		printf("Sorry Sleep Seconds are not enough.\nResupply\n");
		exit(0);
	}

	sleep_seconds = &max_SleepSeconds;
	int total_ItemToProduce = atoi(argv[i++]);
	if (total_ItemToProduce <= 0) {
		printf("Sorry supplied items cannot be produced.\nResupply\n");
		exit(0);
	}

	int buffer_Size = atoi(argv[i++]);
	if (buffer_Size <= 0) {
		printf("Sorry Buffer Size is not enough.\nResupply\n");
		exit(0);
	}

	/* Declaring global buffer size for all structes, variables and methods. */
	BUFFER_SIZE = &buffer_Size;

	/* Segmentation Fault trying to catch and display error line */
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = segfault_sigaction;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);

	/* RingBuffer initializing */
	struct RingBuffer *shared_area;
	shared_area = initialize_RingBuffer(buffer_Size);

	/*
	************ MULTIPLE PRODUCER THREAD FUNCTIONALITY *****************
	*/

	/* Finding values for producer threads to run for how much time and finding the cycles to loop for */
	int Pcount_val = 0;
	int Pmin_Cycles = 1;
	int Premaining_Cycles = -1;

	if (total_ItemToProduce <= total_Producers) {
		total_Producers = total_ItemToProduce;
	} else {
		Pmin_Cycles = total_ItemToProduce / total_Producers;
		Premaining_Cycles = total_ItemToProduce % total_Producers;
	}

	/* Allocating producer Threads and producer object */
	pthread_t *producer_threads;
	producer_threads = (pthread_t*) malloc(sizeof(pthread_t) * total_Producers);

	struct producerThread *pThread;

	pThread = (struct producerThread *) malloc(total_Producers * sizeof(struct producerThread));
	if (pThread == NULL) {
		exit(1);
	}

	/* Creating Producer Threads and joining them */
	for (i = 0; i < total_Consumers; i++) {
		pThread[i].each_item = Pmin_Cycles;
		if (Pcount_val <= Premaining_Cycles) {
			pThread[i].each_item += Pcount_val++;
		}
		pThread[i].thread_id = i;
		pThread[i].ringbuff = shared_area;
		if (pthread_create(&producer_threads[i], NULL, producer, (void*) &pThread[i]) != 0) {
			perror("pthread_create()");
			exit(1);
		}
	}

	/*
	************ MULTIPLE CONSUMER THREAD FUNCTIONALITY *****************
	*/

	/* Finding values for consumer threads to run for how much time and finding the cycles to loop for */
	int Ccount_val = 0;
	int Cmin_Cycles = 1;
	int Cremaining_Cycles = -1;

	if (total_ItemToProduce <= total_Consumers) {
		total_Consumers = total_ItemToProduce;
	} else {
		Cmin_Cycles = total_ItemToProduce / total_Consumers;
		Cremaining_Cycles = total_ItemToProduce % total_Consumers;
	}

	/* Allocating Consumer Threads and consumer object */
	pthread_t *consumer_threads;
	consumer_threads = (pthread_t*) malloc(sizeof(pthread_t) * total_Consumers);

	struct consumerThread *cThread;

	cThread = (struct consumerThread *) malloc(total_Consumers * sizeof(struct consumerThread));
	if (cThread == NULL) {
		exit(1);
	}

	/* Creating Consumer Threads and joining them */
	for (i = 0; i < total_Consumers; i++) {
		cThread[i].each_item = Cmin_Cycles;
		if (Ccount_val <= Cremaining_Cycles) {
			cThread[i].each_item += Ccount_val++;
		}
		cThread[i].thread_id = i;
		cThread[i].ringbuff = shared_area;
		if (pthread_create(&consumer_threads[i], NULL, consumer, (void*) &cThread[i]) != 0) {
			perror("pthread_create()");
			exit(1);
		}
	}

	/* Joining Threads for Producer Threads and Consumer Threads so that each thread is completed before main() thread exits */

	for (i = 0; i < total_Producers; i++) {
		pthread_join(producer_threads[i], NULL);
	}
	for (i = 0; i < total_Consumers; i++) {
		pthread_join(consumer_threads[i], NULL);
	}

	return 0;
}