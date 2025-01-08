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
} CharChannel;

void CHARCH_init_channel(CharChannel* chan, int chan_size, int mess_size);
void CHARCH_destroy_channel(CharChannel* chan);
void CHARCH_send(CharChannel* chan, const char* value);
void CHARCH_receive(CharChannel* chan, char* buffer, int buffer_size);

typedef struct
{
    int queue_size;
    int *queue;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond_non_empty;
    pthread_cond_t cond_non_full;
} IntChannel;

void INTCH_init_channel(IntChannel *chan, int chan_size);
void INTCH_destroy_channel(IntChannel* chan);
void INTCH_send(IntChannel *chan, int value);
int INTCH_receive(IntChannel *chan);


#endif