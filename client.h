/* 
 * Project 2: Online Chat Room
 * Course: COP4635
 * Students: Ti Chen and Yizhi Hu
 */

#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <stdio.h>      //Standard library
#include <stdlib.h>     //Standard library
#include <sys/socket.h> //API and definitions for the sockets
#include <sys/types.h>  //more definitions
#include <netinet/in.h> //Structures to store address information
#include <fcntl.h>      // for open
#include <unistd.h>     // for close
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/sendfile.h>
#include <pthread.h>
#include <sys/stat.h>

#define PORT 60050
#define MAX_MESSAGE 128
#define MAX_FILE_NAME 32
#define MAX_ADDRESS 32

#define USERNAME_LENGTH 50
#define PASSWORD_LENGTH 50
#define MESSAGE_LENGTH 128
#define INIT_ID -100

char tcp_server_response[MAX_MESSAGE];

void getReqMessage(char *message, char *input, int id); // Construct a proper request message by file name
void testTimeOut(int socket_fd);                        // Timer out testing
void *client_recvMessage(void *p);
int tcp_client_socket; // Socket descriptor

void registerAccount(int tcp_client_socket);
int isAbleTologin(int tcp_client_socket);
void loginMenu(int tcp_client_socket);
void mainMenu(int tcp_client_socket);
void startMenu(int tcp_client_socket);
void set_id();
void client_private_chat_request(char *message, int from, int to);
void client_private_chat();
void client_group_chat();
void client_admin();
void print_menu();
void generate_request_message(char *message, int from, char *header);
void client_num_online();
void generate_admin_message(char *message, int choice, int target);
void view_chat_history();
void view_chat_by_property(char p);
void generate_file_name(int socket_id, char *file_name, char property);
void client_change_password();
void client_file_transfer();

int id;
char *kick_out_message;
char *exit_chat_message;

#endif // CLIENT_H_INCLUDED