#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "payload.pb.h"

#define BUFFER_SIZE 2048
#define IP "0.0.0.0"

using namespace std;

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
char ipServer[BUFFER_SIZE];

void sigOut(int sig)
{
  flag = 1;
}

bool isInfoUser(char message[])
{
  int end = 6;
  char temp[BUFFER_SIZE] = {};
  for (int i = 0; i < end; i++)
  {
    temp[i] = message[i];
  }
  if (strcmp(temp, "-info ") == 0)
  {
    bzero(temp, BUFFER_SIZE);
    return true;
  }
  else
  {
    bzero(temp, BUFFER_SIZE);
    return false;
  }
}

int statusType(char message[])
{
  char temp[BUFFER_SIZE] = {};
  for (int i = 0; i < 3; i++)
  {
    temp[i] = message[i];
  }
  if (strcmp(temp, "-s ") == 0)
  {
    bzero(temp, BUFFER_SIZE);
  }
  else
  {
    bzero(temp, BUFFER_SIZE);
    return -1;
  }
  if (strcmp(message, "-s ACTIVE") == 0)
  {
    return 1;
  }
  else if (strcmp(message, "-s INACTIVE") == 0)
  {
    return 2;
  }
  else if (strcmp(message, "-s BUSY") == 0)
  {
    return 3;
  }
  return -1;
}

bool isPrivateMessage(char message[])
{
  int end = 3;
  char holder[BUFFER_SIZE] = {};
  for (int i = 0; i < end; i++)
  {
    holder[i] = message[i];
  }
  if (strcmp(holder, "-p ") == 0)
  {
    bzero(holder, BUFFER_SIZE);
    return true;
  }
  else
  {
    bzero(holder, BUFFER_SIZE);
    return false;
  }
}

void *sendMsgManager(void *arg)
{
  char message[BUFFER_SIZE] = {};
  char buffer[BUFFER_SIZE];

  while (true)
  {

    fgets(message, BUFFER_SIZE, stdin);
    Payload payload;
    payload.set_sender(name);
    if (strcmp(message, "exit") == 0)
    {
      break;
    }
    else
    {
      if (strcmp(message, "--list") == 0)
      {
        payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_user_list);
        string uList;
        payload.SerializeToString(&uList);
        sprintf(buffer, "%s", uList.c_str());
      }
      else if (isInfoUser(message) == true)
      {
        payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_user_info);
        int i;
        int offset = 0;
        int end = offset + 6;
        char new_message[BUFFER_SIZE] = {};
        char username[BUFFER_SIZE] = {};
        int goOn = 0;
        int extraOffset = 0;
        for (i = end; i < strlen(message); i++)
        {
          if (message[i] == ' ' && goOn == 0)
          {
            goOn = 1;
          }
          else if (goOn == 1)
          {
            new_message[i - (end + extraOffset) - 1] = message[i];
          }
          else
          {
            username[i - end] = message[i];
            extraOffset++;
          }
        }
        payload.set_extra(username);
        string out;
        payload.SerializeToString(&out);
        sprintf(buffer, "%s", out.c_str());
        bzero(new_message, BUFFER_SIZE);
        bzero(username, BUFFER_SIZE);
      }
      else if (isPrivateMessage(message) == true)
      {
        payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_private_chat);
        int i;
        int offset = 0;
        int end = offset + 3;
        char new_message[BUFFER_SIZE] = {};
        char username[BUFFER_SIZE] = {};
        int goOn = 0;
        int extraOffset = 0;

        for (i = end; i < strlen(message); i++)
        {
          if (message[i] == ' ' && goOn == 0)
          {
            goOn = 1;
          }
          else if (goOn == 1)
          {
            new_message[i - (end + extraOffset) - 1] = message[i];
          }
          else
          {
            username[i - end] = message[i];
            extraOffset++;
          }
        }
        payload.set_message(new_message);
        payload.set_extra(username);
        string out;
        payload.SerializeToString(&out);
        sprintf(buffer, "%s", out.c_str());
        bzero(new_message, BUFFER_SIZE);
        bzero(username, BUFFER_SIZE);
      }
      else if (statusType(message) > -1)
      {
        int status = statusType(message);
        if (status == 1)
        {
          payload.set_message("ACTIVE");
          payload.set_extra("ACTIVE");
        }
        else if (status == 2)
        {
          payload.set_extra("INACTIVE");
          payload.set_message("INACTIVE");
        }
        else
        {
          payload.set_extra("BUSY");
          payload.set_message("BUSY");
        }
        payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_update_status);
        string out;
        payload.SerializeToString(&out);
        sprintf(buffer, "%s", out.c_str());
      }
      else
      {
        payload.set_message(message);
        payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_general_chat);
        string out;
        payload.SerializeToString(&out);
        sprintf(buffer, "%s", out.c_str());
      }
      send(sockfd, buffer, strlen(buffer), 0);
    }
    bzero(message, BUFFER_SIZE);
    bzero(buffer, BUFFER_SIZE);
  }
  flag = 1;
}

void *getMsgManager(void *arg)
{
  char message[BUFFER_SIZE] = {};
  while (1)
  {
    int received_message = recv(sockfd, message, BUFFER_SIZE, 0);
    if (received_message > 0)
    {
      Payload server_payload;
      server_payload.ParseFromString(message);
      if (server_payload.code() == 200 || server_payload.flag() == Payload_PayloadFlag::Payload_PayloadFlag_general_chat)
      {
        printf("message: %s \n", server_payload.message().c_str());
      }
      else if (server_payload.code() == 401)
      {
        printf("error: %s \n", server_payload.message().c_str());
        flag = 1;

        break;
      }
      else
      {
        printf("Error del server %d -- %s!\n", server_payload.code(), server_payload.message().c_str());
      }
    }
    else if (received_message == 0)
    {
      break;
    }
    else
    {
      // -1
    }
    memset(message, 0, sizeof(message));
  }
}

int main(int argc, char **argv)
{
  printf("hola");
  char buffer[BUFFER_SIZE];

  int port = atoi(argv[1]);
  signal(SIGINT, sigOut);
  strcpy(name, argv[2]);
  strcpy(ipServer, argv[3]);

  struct sockaddr_in server_addr;
  char *ip_normalized = ipServer;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip_normalized);
  server_addr.sin_port = htons(port);

  //int hola;

  int i = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (i == -1)
  {
    fprintf(stderr, "Error creating socket...\n");
  }

  Payload register_payload;
  register_payload.set_sender(name);
  string my_ip = IP;
  register_payload.set_ip(my_ip);
  register_payload.set_flag(Payload_PayloadFlag::Payload_PayloadFlag_register_);

  string out;
  register_payload.SerializeToString(&out);
  sprintf(buffer, "%s", out.c_str());
  send(sockfd, buffer, strlen(buffer), 0);
  printf("Welcome to chickensalad chat \n");

  pthread_t send_msg_thread;
  if (pthread_create(&send_msg_thread, NULL, &sendMsgManager, NULL) != 0)
  {
    printf("ERROR in pthread\n");
    return EXIT_FAILURE;
  }

  pthread_t recv_msg_thread;
  if (pthread_create(&recv_msg_thread, NULL, &getMsgManager, NULL) != 0)
  {
    printf("ERROR in pthread\n");
    return EXIT_FAILURE;
  }

  while (1)
  {
    if (flag)
    {
      printf("Closing chickensalad chat >:(\n");
      break;
    }
  }

  close(sockfd);
  return EXIT_SUCCESS;
}