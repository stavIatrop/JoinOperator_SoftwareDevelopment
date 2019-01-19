#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "jobScheduler.h"

struct JobScheduler jobScheduler;
int debugInt = 0;

void initialiseScheduler() {
    int32_t err = 0;
    jobScheduler.numbOfThreads = NUMB_OF_THREADS;
    
    //Initialise synch mechanics
    pthread_mutex_init(&(jobScheduler.queueMutex), 0);
    pthread_cond_init(&(jobScheduler.cond_read), 0);
    pthread_cond_init(&(jobScheduler.cond_write), 0);
    jobScheduler.writing = 0;
    jobScheduler.reading = 0;
    jobScheduler.working = 0;
    jobScheduler.shutdown = 0;


    //Thread allocation
    if ((jobScheduler.threads = malloc(jobScheduler.numbOfThreads * sizeof(pthread_t))) == NULL) 
    {
        perror("Malloc of threads failed");  
        exit(1); 
    }

    //Queue initialisation
    if((jobScheduler.jobQueue = (jobQueueHead *) malloc(sizeof(jobQueueHead))) == NULL) {
        perror("Malloc of queue head failed");
    }
    jobScheduler.jobQueue->firstNode = NULL;
    jobScheduler.jobQueue->lastNode = NULL;
    jobScheduler.jobQueue->size = 0;
    
    //Run Threads
    for (int whichThread = 0 ; whichThread < jobScheduler.numbOfThreads; whichThread++) 
    {
        if ((err = pthread_create(jobScheduler.threads + whichThread, NULL, jobExecutor, 0))) {
            perrorThread("Failed to create scheduler thread", err);   exit(1);} 
    }

}

void shutdownAndFreeScheduler(){
    int32_t err = 0;

    //Broadcast shutdown
    jobScheduler.shutdown = 1;
    pthread_cond_broadcast(&(jobScheduler.cond_read));

    //Join threads
    for (int whichThread = 0 ; whichThread < jobScheduler.numbOfThreads; whichThread++) 
    {
        if ((err = pthread_join(*(jobScheduler.threads + whichThread), NULL))) 
        {
            perrorThread("Failed to pthread_join", err); 
            exit(1); 
        }         
    }

    free(jobScheduler.threads);
    pthread_cond_destroy(&(jobScheduler.cond_read));
    pthread_cond_destroy(&(jobScheduler.cond_write));
    pthread_mutex_destroy(&(jobScheduler.queueMutex));

    deleteQueue();
}

void Barrier() {
    //fprintf(stderr, "Waiting\n");
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    while (jobScheduler.working>0 || jobScheduler.jobQueue->size>0)
    {
        pthread_cond_wait(&(jobScheduler.cond_barrier),&(jobScheduler.queueMutex));
    }
    //fprintf(stderr, "Woke\n");
    pthread_mutex_unlock(&(jobScheduler.queueMutex));
}

void * jobExecutor() {
    while(jobScheduler.shutdown == 0) {
        struct Job * job = readFromQueue();
        (*(job->function))(job->argument);
        free(job);
    }
    pthread_exit(0);
    return NULL;
}

void writeOnQueue(struct Job * job)
{
    enterWrite();
    insertInQueue(job);
    exitWrite();
}

struct Job * readFromQueue()
{
    struct Job * job;
    enterRead();
    job = popFromQueue();
    exitRead();
    return job;
}


void enterWrite()
{
    pthread_mutex_lock(&(jobScheduler.queueMutex));
    while((jobScheduler.writing > 0) || (jobScheduler.reading > 0))
    {
            pthread_cond_wait(&(jobScheduler.cond_write), &(jobScheduler.queueMutex));
    }

    jobScheduler.writing++;

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

    return retNode;
}

void insertInQueue(struct Job * job) {
    if(jobScheduler.jobQueue->size == 0) {
        jobScheduler.jobQueue->firstNode = createNode(job);
        jobScheduler.jobQueue->lastNode = jobScheduler.jobQueue->firstNode;
    }
    else {
        jobScheduler.jobQueue->lastNode->next = createNode(job);
        jobScheduler.jobQueue->lastNode = jobScheduler.jobQueue->lastNode->next;
    }
    jobScheduler.jobQueue->size += 1;
}

struct Job * popFromQueue() {

    if(jobScheduler.jobQueue->size == 0) {
        pthread_mutex_unlock(&(jobScheduler.queueMutex));
        return NULL;
    }

    jobNode * retNode = jobScheduler.jobQueue->firstNode;
    if(jobScheduler.jobQueue->size == 1) {
        jobScheduler.jobQueue->firstNode = NULL;
        jobScheduler.jobQueue->lastNode = NULL;
    }
    else {
        jobScheduler.jobQueue->firstNode = retNode->next;
    }

    jobScheduler.jobQueue->size -= 1;

    struct Job * retJob = retNode->job;

    free(retNode);

    return retJob;
}

void deleteQueue()
{
    if(jobScheduler.jobQueue->size != 0) {
        fprintf(stderr, "Unexpected: Queue is not empty when deleting\n");
    }
    deleteQueueNode(jobScheduler.jobQueue->firstNode);
    free(jobScheduler.jobQueue);
}

void deleteQueueNode(jobNode * node)
{
    if(node == NULL)
        return;
    if(node->next != NULL)
        deleteQueueNode(node->next);
    free(node);
}

