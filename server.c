#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "chan.h"

#define BUFFER_SIZE 1024
#define POOL_SIZE 20
#define CHAN_SIZE 1000

typedef struct
{
    int port;
    int socket_fd;
    struct sockaddr_in serv_addr;

} Server;

typedef struct
{
    pthread_t threads[POOL_SIZE];
} ThreadPool;

void init_thread_pool(ThreadPool *pool, void *func, IntChannel *chan)
{
    for (int i = 0; i < POOL_SIZE; i++)
    {
        pthread_create(&pool->threads[i], NULL, func, chan);
        
    }
}

void init_server(Server *server, int port)
{
    server->port = port;
    if ((server->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    server->serv_addr.sin_family = AF_INET;
    server->serv_addr.sin_addr.s_addr = INADDR_ANY;
    server->serv_addr.sin_port = htons(server->port);

    if (bind(server->socket_fd, (struct sockaddr *)&server->serv_addr, sizeof(server->serv_addr)) < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
}

void send_response(int client_fd)
{
    const char *response_body = "OK!";
    const char *response_header_template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %ld\r\n"
        "\r\n";

    size_t content_length = strlen(response_body);

    char response[1024];
    int response_length = snprintf(response, sizeof(response), response_header_template, content_length);

    if (response_length < 0)
    {
        perror("snprintf failed");
        return;
    }

    if (send(client_fd, response, response_length, 0) < 0)
    {
        perror("send header failed");
        return;
    }

    if (send(client_fd, response_body, content_length, 0) < 0)
    {
        perror("send body failed");
        return;
    }
}

void request_processing(int client_fd)
{
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("bytes received: %ld\n", bytes_received);
    if (bytes_received > 0)
    {
        printf("%s\n", buffer);
    }
    usleep(1000);
    send_response(client_fd);
    close(client_fd);
    free(buffer);
}

void thread(void *arg)
{
    while (1)
    {
        IntChannel *chan = (IntChannel *)arg;
        request_processing(INTCH_receive(chan));
    }
}

void start(Server *server)
{
    if (listen(server->socket_fd, 10) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", server->port);
    IntChannel ch;
    INTCH_init_channel(&ch, CHAN_SIZE);
    ThreadPool th_pool;
    init_thread_pool(&th_pool, thread, &ch);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd;

        if ((client_fd = accept(server->socket_fd,
                                (struct sockaddr *)&client_addr,
                                &client_addr_len)) < 0)
        {
            perror("accept failed");
            continue;
        }
        printf("Client connected\n");
        INTCH_send(&ch, client_fd);
    }
    INTCH_destroy_channel(&ch);
    close(server->socket_fd);
}

int main(int argc, char *argv[])
{
    Server server;
    init_server(&server, 8080);
    start(&server);
    return 0;
}
