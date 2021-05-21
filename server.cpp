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

map<string, client_struct *> userlist = {};

using namespace std;

void *client_manager(void *params)
{
  char buffer[BUFFER_SIZE];
  client_struct client;
  client_struct *newClient = (client_struct *)params;
  int socket = newClient->socket_d

                   Payload payload;

  while (true)
  {
    int data = recv(socket, buffer, BUFFER_SIZE, 0);
    if (data > 0)
    {
      payload.ParseFromString(buffer);

      if (payload.flag() == Payload_PayloadFlag_register_)
      {
        printf("Registration started!");

        if (userlist.count(payload.sender()) == 0)
        {
          string responseMessage = "Welcome to chickensalad chat!";
          sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);

          client.name = payload.sender();
          client.socket_d = socket;
          client.status = ACTIVE;

          strcpy(client.address, newClient->address);
          userList[client.name] = &client;
        }
        else
        {
          sendError(socket, "Cannot register with that name!");
        }
      }

      else if (payload.flag() == Payload_PayloadFlag_user_list)
      {
        printf("List of all users: ");

        Payload *response = new Payload();
        string responseMessage = '\tClient name\tClient IP\tClient Status\n';
        map<string, client_struct *>::iterator itr;

        for (itr = userlist.begin(); itr != userlist.end(); ++itr)
        {
          client_struct *client = itr->second;
          string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, client->status);
          responseMessage = responseMessage + clientString;
        }
        sendResponse(socket, 'server', responseMessage, payload.flag(), 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_user_info)
      {
        if (userlist.count(payload.extra()) > 0)
        {
          string responseMessage = '\tClient name\tClient IP\tClient Status\n';
          client_struct *client = userlist[payload.extra()];

          string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, client->status);
          responseMessage = responseMessage + clientString;
          sendResponse(socket, 'server', responseMessage, payload.flag(), 200, buffer);
        }
        else
        {
          printf("That user doesnt exist!");
          sendError(socket, "That user doesnt exist!");
        }
      }

      else if (payload.flag() == Payload_PayloadFlag_update_status)
      {
        printf("Updating a user status!");

        client.status = payload.extra();

        string responseMessage = "Your status was updated to " + client->status;
        sendResponse(socket, 'server', responseMessage, payload.flag(), 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_general_chat)
      {
        printf("Broadcast iniciated!");

        Payload *broadcast = new Payload();
        broadcast->sender = 'server';
        broadcast->message = 'broadcast from ' + payload.sender + payload.message + '\n';
        broadcast->code = 200;
        string responseMessage;
        broadcast->SerializeToString(&binary);
        strcpy(buffer, binary.c_str());

        map<string, client_struct *>::iterator itr;
        for (itr = userlist.begin(); itr != userlist.end(); ++itr)
        {
          client_struct *client = itr->second;
          flag = 0;
          if (client->name != newClient->name)
          {
            send(client->socket_d, buffer, responseMessage.size() + 1, flag);
          }
        }

        string responseMessage = "Your Broadcast was executed succesfully!";
        sendResponse(socket, 'server', responseMessage, payload.flag, 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_private_chat)
      {
        if (userlist.count(payload.extra()) > 0)
        {
          printf("Private message sended");

          client_struct *client2 = userlist[payload.extra()];
          Payload *message = new Payload();
          message.sender = client2.name;
          message.message = "*PRIVATE* from " + payload.sender() + ": " + payload.message() + "\n";
          message.code = 200;
          message.flag() = payload.flag();

          string responseMessage;
          broadcast->SerializeToString(&binary);
          strcpy(buffer, binary.c_str());
          flag = 0;
          send(message->socket, buffer, responseMessage.size() + 1, flag);
        }
        else
        {
          sendError(socket, "Unable to send private message!")
        }
      }

      else
      {
        sendError(socket, "Invalid option! ");
      }
    }
  }
}

void sendResponse()
{
}

void sendError()
{
}

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