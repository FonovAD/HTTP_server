#ifndef CHANNEL_H
#define CHANNEL_H

#include <pthread.h>

typedef struct{
    int queue_size;
    char **queue;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond_non_empty;
    pthread_cond_t cond_non_full;
} Channel;

void CHinit_channel(Channel* chan, int chan_size, int mess_size);
void CHdestroy_channel(Channel* chan);
void CHsend(Channel* chan, const char* value);
void CHreceive(Channel* chan, char* buffer, int buffer_size);

#endif