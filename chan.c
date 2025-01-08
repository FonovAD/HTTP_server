#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

typedef struct
{
    int queue_size;
    char **queue;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond_non_empty; // будет сигнализировать потокам о том, что очередь не пуста
    pthread_cond_t cond_non_full;  // будет сигнализировать о том, что очередь не переполнена
} CharChannel;

// Size - максимальный размер сообщения, которое способен обработать сервер.
// Нужно добавить обработку слишком большого сообщения, чтобы сигнализировать об этом пользователю.
void CHARCH_init_channel(CharChannel *chan, int chan_size, int mess_size)
{
    chan->queue_size = chan_size;
    // уже тут возникает вопрос, каким размером должен быть каждый массив char
    chan->queue = malloc(sizeof(char *) * chan_size);
    if (chan->queue == NULL)
    {
        printf("Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < chan_size; i++)
    {
        chan->queue[i] = malloc(sizeof(char) * mess_size);
        if (chan->queue[i] == NULL)
        {
            printf("Error: malloc failed\n");
            exit(EXIT_FAILURE);
        }
    }
    chan->front = 0;
    chan->rear = 0;
    pthread_mutex_init(&chan->mutex, NULL);
    pthread_cond_init(&chan->cond_non_empty, NULL);
    pthread_cond_init(&chan->cond_non_full, NULL);
}

void CHARCH_destroy_channel(CharChannel *chan)
{
    for (int i = 0; i < chan->queue_size; i++)
    {
        free(chan->queue[i]);
    }
    free(chan->queue);
    pthread_mutex_destroy(&chan->mutex);
    pthread_cond_destroy(&chan->cond_non_empty);
    pthread_cond_destroy(&chan->cond_non_full);
}

void CHARCH_send(CharChannel *chan, const char *value)
{
    pthread_mutex_lock(&chan->mutex);
    // если очередь заполнена - блокиремся
    while ((chan->rear + 1) % chan->queue_size == chan->front)
    {
        pthread_cond_wait(&chan->cond_non_full, &chan->mutex);
    }
    strncpy(chan->queue[chan->rear], value, strlen(chan->queue[chan->rear]) - 1);
    chan->queue[chan->rear][strlen(chan->queue[chan->rear]) - 1] = '\0';
    chan->rear = (chan->rear + 1) % chan->queue_size;
    pthread_cond_signal(&chan->cond_non_empty);
    pthread_mutex_unlock(&chan->mutex);
}

void CHARCH_receive(CharChannel *chan, char *buffer, int buffer_size)
{
    pthread_mutex_lock(&chan->mutex);
    while (chan->front == chan->rear)
    {
        pthread_cond_wait(&chan->cond_non_empty, &chan->mutex);
    }
    strncpy(buffer, chan->queue[chan->front], buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    chan->front = (chan->front + 1) % chan->queue_size;
    pthread_cond_signal(&chan->cond_non_full);
    pthread_mutex_unlock(&chan->mutex);
}

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


void INTCH_init_channel(IntChannel *chan, int chan_size)
{
    chan->queue_size = chan_size;
    chan->queue = malloc(sizeof(int) * chan_size);
    if (chan->queue == NULL)
    {
        printf("Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    chan->front = 0;
    chan->rear = 0;
    pthread_mutex_init(&chan->mutex, NULL);
    pthread_cond_init(&chan->cond_non_empty, NULL);
    pthread_cond_init(&chan->cond_non_full, NULL);
}


void INTCH_destroy_channel(IntChannel *chan)
{
    free(chan->queue);
    pthread_mutex_destroy(&chan->mutex);
    pthread_cond_destroy(&chan->cond_non_empty);
    pthread_cond_destroy(&chan->cond_non_full);
}


void INTCH_send(IntChannel *chan, int value)
{
    pthread_mutex_lock(&chan->mutex);
    while ((chan->rear + 1) % chan->queue_size == chan->front)
    {
        pthread_cond_wait(&chan->cond_non_full, &chan->mutex);
    }
    chan->queue[chan->rear] = value;
    chan->rear = (chan->rear + 1) % chan->queue_size;
    pthread_cond_signal(&chan->cond_non_empty);
    pthread_mutex_unlock(&chan->mutex);
}

int INTCH_receive(IntChannel *chan)
{
    pthread_mutex_lock(&chan->mutex);
    while (chan->front == chan->rear)
    {
        pthread_cond_wait(&chan->cond_non_empty, &chan->mutex);
    }
    int value = chan->queue[chan->front];
    chan->front = (chan->front + 1) % chan->queue_size;
    pthread_cond_signal(&chan->cond_non_full); 
    pthread_mutex_unlock(&chan->mutex);
    return value;
}