#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/call>
#include <pthread.h>
#include <condition_variable>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "payload.pb.h"

#define ACTIVE 1
#define BUSY 2
#define INACTIVE 3
#define DEFAULT_IP "0.0.0.0"
#define BUFFER_SIZE 4096
int uid = 100;

/*User structure*/
typedef struct
{
  struct sockaddr_in address;
  int socket_d;
  int uid;
  char name[32];
  int status;
} client_struct;

using namespace std;

int main(int argc, char *argv[])
{
  int port = atoi(argv[1]);

  char ip = DEFAULT_IP;
  sockaddr_in server_addr;
  sockaddr_in client_addr;
  socklen_t client_size;
  indt socket_fd = 0, conn_fd = 0;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  signal(SIGPIPE, SIG_IGN)

      void *
      client_manager(void *params)
  {
    char buffer[BUFFER_SIZE];
    client_struct
  }

  while (true)
  {
    client_size = sizeof(client_addr);
    conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size);

    pthread_t thread_id;
    pthread_attr_t thread_attr;

    client_struct *client = (client_struct)malloc(sizeof(client_struct));
    client->address = client_addr;
    client->socket_d = conn_fd;
    client->uid = uid++;
    client->status = ACTIVE;

    pthread_attr_init(&thread_attr);
    pthread_create(&thread_id, &thrad_attr, client_manager, (void *)client)

        sleep(1)
  }

  return EXIT_SUCCESS
}