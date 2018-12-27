#include <pthread.h>
#include <unistd.h>

#include "jobScheduler.h"

struct JobScheduler jobScheduler;

void enterWrite()
{
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    while((jobScheduler.writing > 0) || (jobScheduler.reading > 0))
    {
            pthread_cond_wait(&(jobScheduler.cond_write), &(jobScheduler.queueMutex));
    }

    jobScheduler.writing++;
    jobScheduler.jobQueue->size += 1;

    pthread_mutex_unlock(&(jobScheduler.queueMutex));
}

void exitWrite()
{
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    jobScheduler.writing--;
    
    //Only one thread writes so we do not have someone to wake up
    pthread_cond_signal(&(jobScheduler.cond_read));
    pthread_mutex_unlock(&(jobScheduler.queueMutex));
}

void enterRead()
{
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    if(jobScheduler.shutdown == 1)
    {
        pthread_mutex_unlock(&(jobScheduler.queueMutex));
        pthread_exit(0);
    }
    while((jobScheduler.writing > 0) || (jobScheduler.reading > 0) || (jobScheduler.jobQueue->size == 0))
    {
        if(jobScheduler.shutdown == 1)
        {
            pthread_mutex_unlock(&(jobScheduler.queueMutex));
            pthread_exit(0);
        }
        if(jobScheduler.jobQueue->size == 0)
                pthread_cond_signal(&(jobScheduler.cond_write));
        pthread_cond_wait(&(jobScheduler.cond_read), &(jobScheduler.queueMutex));
    }

    jobScheduler.reading++;
    jobScheduler.jobQueue->size -= 1;

    pthread_mutex_unlock(&(jobScheduler.queueMutex));
}

void exitRead()
{
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    jobScheduler.reading--;
    pthread_cond_signal(&(jobScheduler.cond_read));
    pthread_cond_signal(&(jobScheduler.cond_write));
    pthread_mutex_unlock(&(jobScheduler.queueMutex));
    if(jobScheduler.shutdown == 1)
        pthread_exit(0);
}


//Queue manipulation
jobNode * createNode(struct Job * job) {
    jobNode * retNode = (jobNode *) malloc(sizeof(jobNode));
    retNode->job = job;
    retNode->next = NULL;
}

void insertInQueue(struct Job * job) {
    if(jobScheduler.jobQueue->size == 0) {
        jobScheduler.jobQueue->firstNode = createNode(job);
        jobScheduler.jobQueue->firstNode = jobScheduler.jobQueue->lastNode;
    }
    else {
        jobScheduler.jobQueue->lastNode->next = createNode(job);
        jobScheduler.jobQueue->lastNode = jobScheduler.jobQueue->lastNode->next;
    }
    jobScheduler.jobQueue->size == 1;
}

