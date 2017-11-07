/* SEMAPHORE LIBRARY DEFINITION
*  With PTHREADS and POSIX Library functions
*  Implements P() / Wait() function as well as V() / Singal() funciton as equivalent to sem_wait() and sem_signal().
*/

#include <stdlib.h>
#include <pthread.h>
#include "semaphore.h"

/* Semaphore Initialization from semaphore library. 
 * It initializes all the mutex and condition variables to default NULL values and other value as specified by semaphores in main thread.
 */

struct semaphore *initialize_Semaphore(int count) {
	struct semaphore *s;
	s = (struct semaphore *) malloc(sizeof(struct semaphore));
	if (s == NULL) {
		return (NULL);
	}
	s->value = count;
	s->wait_thread = 0;
	pthread_cond_init(&(s->cond), NULL);
	pthread_mutex_init(&(s->mutex), NULL);
	return (s);
}

/* Same as Wait() function in semaphore.h. It is made using pthread mutex and pthread condition variables. 
 * It uses mutex lock and then decrements the values. If value is less than zero then calling thread waits and blcoks itself. 
 * After wakeup by thread it unlocks the mutex. 
 */

void P(struct semaphore *S) {
	pthread_mutex_lock(&(S->mutex));
	S->value--;
	while (S->value < 0) {
		if (S->wait_thread < (-1 * S->value)) {
			S->wait_thread++;
			pthread_cond_wait(&(S->cond), &(S->mutex));
			S->wait_thread--;
		}
		else {
			break;
		}
	}
	pthread_mutex_unlock(&(S->mutex));
	return;
}

/* Same as Signal() function in semaphore.h. It is made using pthread mutex and pthread condition variables. 
 * It uses mutex lock and then increments the values. If value is less than equal to zero then calling thread wakes up blocking thread. 
 * After wakeup by thread it unlocks the mutex. 
 */

void V(struct semaphore *S) {
	pthread_mutex_lock(&(S->mutex));
	S->value++;
	if (S->value <= 0) {
		pthread_cond_signal(&(S->cond));
	}
	pthread_mutex_unlock(&(S->mutex));
}