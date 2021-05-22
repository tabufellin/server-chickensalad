#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <map>
#include <vector>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "payload.pb.h"

#define BUFFER_SIZE 4096
#define IP "127.0.0.1"

int socketfd = 0;

void *get_msg_manager(void *arg)
{
}

void *send_msg_manager(void *arg)
{
  char message[BUFFER_SIZE] = {};
  char buffer[BUFFER_SIZE];

  while (true)
  {
  }
}

int main(int argc, char *argv[])
{
  int socketfd, err;
  struct sockaddr_in server_addr;
  char serverIP[INET_ADDRSTRLEN];
  char buffer[BUFFER_SIZE];
  char *name, *server, *port;

  signal(SIGINT, catch_ctrl_c_and_exit);
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  name = argv[1];
  server = argv[2];
  port = argv[3];

  /*Connection to server*/
  socketfd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(serverIP);
  server_addr.sin_port = htons(port);

  int err = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == = 1)
  {
    printf("Connection error...\n")
  }
  else
  {
    printf("Connected to Chicken Salad server")
        Payload payload_r = new Payload()
                                payload_r.set_sender(name);
    string myIP = IP;
    payload_r.set_ip(my_ip);
    payload_r.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_register_);
    string output;
    payload_r.SerializeToString(&output);
    sprintf(buffer, "%s", output.c_str());
    send(socketfd, buffer, strlen(buffer), 0);
    printf("Welcome to Chicken Salad Server!\n")

        pthread_t send_msg_thread;
    pthread_create(&send_msg_thread, NULL, &send_msg_manager, NULL)
        pthread_t get_msg_thread;
    pthread_create(&get_msg_thread, NULL, &get_msg_manager, NULL)

        while (true)
    {
      if (ward == false)
      {
        printf("Cerrando Chicken Salad :(\n") break;
      }
    }
  }
}