#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <condition_variable>
#include <map>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "payload.pb.h"
#include <errno.h>
#include <string>
#include <string.h>
#define ACTIVE 1
#define BUSY 2
#define INACTIVE 3
#define DEFAULT_IP "0.0.0.0"
#define BUFFER_SIZE 4096
int uid = 0;

/*User structure*/
typedef struct
{
  struct sockaddr_in address;/////////////////////////////
  int socket_d;
  int uid;
  const char* name;
  int status;
} client_struct;

//map<string, client_struct *> userList = {};
// limit of users 200
client_struct *userList[200];

using namespace std;

string getTypeStatus (int status) {
  if (status == 1) {
    return "ACTIVE";
  }
  if (status == 2) {
    return "BUSY";
  }
  if (status == 3) {
    return "INACTIVE";
  }
}
// 
void sendResponse(int socket, string sender, string message, Payload_PayloadFlag flag, int code, char *buffer)
{
  string data;
  Payload *response = new Payload();
  response->set_sender(sender);
  response->set_message(message);
  response->set_code(code);
  response->set_flag(flag);
  
  response->SerializeToString(&data);
  strcpy(buffer, data.c_str());
  send(socket, buffer, data.size() + 1, 0);
  printf("Sent...");
}


void sendError(int socket, string message)
{
  string data;
  Payload *errorResponse = new Payload();
  errorResponse->set_sender("server");
  errorResponse->set_message(message);
  errorResponse->set_code(500);
  errorResponse->SerializeToString(&data);

  char buffer[data.size() + 1];
  strcpy(buffer, data.c_str());
  send(socket, buffer, sizeof buffer, 0);
  printf("Error sent..."); 
  //hoal hoal 
  //if(){
    //int send=
  //}
}



void *client_manager(void *params)
{
  char buffer[BUFFER_SIZE];
  client_struct client;
  client_struct *newClient = (client_struct *)params;
  int socket = newClient->socket_d;

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

        // maybe later we will use it 
        bool flagUserExist = false;
        for (int i = 0; i < 200; i++) {
          if(userList[i]->name == payload.sender()){
            flagUserExist = true;
          }
        }

        if (flagUserExist == false)
        {
          string responseMessage = "Welcome to chickensalad chat!";
          sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);

          client.name = payload.sender().c_str();
          client.socket_d = socket;
          client.status = ACTIVE;

          // aqu[i tenaimos algo pendiente]
          client.address = newClient->address;

          //strcpy(client.address, newClient->address);
          if (uid <= 200) {
            userList[uid] = &client;
            uid = uid + 1;
          } else {
            printf("The server is full of users");
          }


          
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
        string responseMessage = "\tClient name\tClient IP\tClient Status\n";
        //map<string, client_struct *>::iterator itr;

        for (int i = 0; i <= uid; i++) {
          client_struct *client = userList[i];
          string strStatus = getTypeStatus(client->status);
          string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, strStatus);
        }
        /*for (itr = userList.begin(); itr != userList.end(); ++itr)
        {
          client_struct *client = itr->second;
          string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, client->status);
          responseMessage = responseMessage + clientString;
        }*/
        sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_user_info)
      {

        bool flagUserIsOrNot = false;
        int userUid;
        for (int i = 0; i <= uid; i++) {
          client_struct *client = userList[i];
          if (client->name == payload.extra()) {
            flagUserIsOrNot = true;
            userUid = i;
          }
        }
        //if (userList.count(payload.extra()) > 0)
        if (flagUserIsOrNot == true)
        {
          string responseMessage = "\tClient name\tClient IP\tClient Status\n";
          client_struct *client = userList[userUid];
          string strStatus = getTypeStatus(client->status);
          string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, strStatus);
          //string clientString = ("\t%s\t%s\t%s\n", client->name, client->address, client->status);
          responseMessage = responseMessage + clientString;
          sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);
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
        string s = payload.extra();
        stringstream geek(s);
        
        //client.status = stoi(payload.extra());
        //flechita
        geek >> client.status;        
        string strStatus = getTypeStatus(client.status);
        string responseMessage = "Your status was updated to " + strStatus;
        sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_general_chat)
      {
        printf("Broadcast iniciated!");

        Payload *broadcast = new Payload();
        broadcast->set_sender("server");
        broadcast->set_message("broadcast from " + payload.sender() + payload.message() + "\n");
        broadcast->set_code(200);

        string responseMessage;
        broadcast->SerializeToString(&responseMessage);
        strcpy(buffer, responseMessage.c_str());

        //map<string, client_struct *>::iterator itr;
        for (int i = 0; i <= uid; i++) {
          client_struct *client = userList[i];
          int flag = 0;
          if (client->name != newClient->name)
          {
            send(client->socket_d, buffer, responseMessage.size() + 1, flag);
          }
        }
        /*
        for (itr = userList.begin(); itr != userList.end(); ++itr)
        {
          client_struct *client = itr->second;
          flag = 0;
          if (client->name != newClient->name)
          {
            send(client->socket_d, buffer, responseMessage.size() + 1, flag);
          }
        }
        */
        responseMessage = "Your Broadcast was executed succesfully!";
        sendResponse(socket, "server", responseMessage, payload.flag(), 200, buffer);
      }

      else if (payload.flag() == Payload_PayloadFlag_private_chat)
      {

        bool flagUserIsOrNot2 = false;
        int userUid2;
        for (int i = 0; i <= uid; i++) {
          client_struct *client = userList[i];
          if (client->name == payload.extra()) {
            flagUserIsOrNot2 = true;
            userUid2 = i;
          }
        }
        if (flagUserIsOrNot2 == true)
        {
          printf("Private message sended");

          client_struct *client2 = userList[userUid2];
          Payload *message = new Payload();
          message->set_sender(client2->name);
          message->set_message("*PRIVATE* from " + payload.sender() + ": " + payload.message() + "\n");
          message->set_code(200);
          message->set_flag(payload.flag());

          string responseMessage2;
          message->SerializeToString(&responseMessage2);
          strcpy(buffer, responseMessage2.c_str());
          int flag = 0;
          send(client2->socket_d, buffer, responseMessage2.size() + 1, flag);
        }
        else
        {
          sendError(socket, "Unable to send private message!");
        }
      }

      else
      {
        sendError(socket, "Invalid option! ");
      }
    }
  }
}



int main(int argc, char *argv[])
{
  int port = atoi(argv[1]);

  char *ip = DEFAULT_IP;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  socklen_t client_size;
  int socket_fd = 0, conn_fd = 0;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  signal(SIGPIPE, SIG_IGN);

  while (true)
  {
    client_size = sizeof(client_addr);
    conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size);

    pthread_t thread_id;
    pthread_attr_t thread_attr;

    client_struct *client = (client_struct *)malloc(sizeof(client_struct));
    client->address = client_addr;
    client->socket_d = conn_fd;
    client->uid = uid++;
    client->status = ACTIVE;

    pthread_attr_init(&thread_attr);
    pthread_create(&thread_id, &thread_attr, client_manager, (void *)client);

    sleep(1);
  }
  close(socket_fd);
  return EXIT_SUCCESS;
}