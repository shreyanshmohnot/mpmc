/* SEMAPHORE LIBRARY DECLARAIONS
*  Declaring the strucutre to work same as semaphore
*  Implementing Mutex and Conditions Variables to acheive the semaphore virtualizations.
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/* Semaphore implementation using strucutre variable. It has a pthread mutex, pthread condition variable, semaphore value and wait thread.
 * Mutex is also locking the readand write to the sempahore. 
 * Condition variable is for checking the condition for semaphore to wait for a thread or wakeup blocked thread and continue its work. 
 * Wait thread is used to check the total waiting threads inside the semaphore who have been blocked and waiting for wakeup or mutex unlock.
 * Value is specified by the seamphore. It is used as the main variable to check for each condition variable and acquiring and release of mutex.
 */

struct semaphore {
    int value;
    int wait_thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/* Function declarations to be used in the library.
 * initialize_Semaphore is used to initialize the semaphore strucutre values to defaults. Takes argument as the value.
 * P() is the wait signal same as sem_wait(). Takes argument as semaphore pointer.
 * V() is the signal signal same as sem_signal(). Takes argument as semaphore pointer.
 */

struct semaphore* initialize_Semaphore(int);
void P(struct semaphore*);
void V(struct semaphore*);
#endif