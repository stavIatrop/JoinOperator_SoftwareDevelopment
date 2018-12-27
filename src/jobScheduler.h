#include <pthread.h>
#include <unistd.h>

typedef struct JobNode jobNode; 

struct Job {
    void (*function)(void *);
    void *argument;
};

struct JobNode {
    struct Job * job;
    jobNode * next;
};

typedef struct JobQueueHead {
    int size;
    jobNode * firstNode;
    jobNode * lastNode;
} jobQueueHead;

struct JobScheduler {
    int numbOfThreads;
    pthread_t * threads;
    pthread_mutex_t queueMutex;

    //We will only allow one reader or one writer (since we are using a queue)
    pthread_cond_t cond_write;
    pthread_cond_t cond_read;
    int writing;
    int reading;
    int shutdown;

    jobQueueHead * jobQueue;
};

extern struct JobScheduler jobScheduler;

void initialiseScheduler();

//Queue synched push/pop
void writeOnQueue(struct Job * job);
struct Job * readFromQueue();

//Queue struct basic functions
jobQueueHead * createQueue();
jobNode * createNode(struct Job * job);
void insertInQueue(struct Job * job);
struct Job * popFromQueue();
void deleteQueue();
void deleteQueueNode(jobNode * node);

//Thread read/write synchronisation
void enterWrite();
void exitWrite();
void enterRead();
void exitRead();
