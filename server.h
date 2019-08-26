/* 
 * Project 2: Online Chat Room
 * Course: COP4635
 * Students: Ti Chen and Yizhi Hu
 */
#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include "client.h"
#define MAX_SERVER 30
#define MAX_HTML_LEN 512
#define MAX_IMG 1000000

#define PRESENCE_REQ 18
#define INDEX_REQ 15
#define MAIN_REQ 6
#define WORKING_REQ 21
#define IMG_REQ 13
#define PRIVATE 2
#define ID_IDX 2

int num_of_sockets;
int client_sockets[MAX_SERVER];
int group_chat_sockets[MAX_SERVER];
int ban_sockets[MAX_SERVER];
int num_group_chat;
int num_of_ban;

void *server_recvMessage(void *p);
void private_chat(int *sockets, int num);
void send_private(char *message, int *socket_ls);
int tokensize_message(char *message, int *socket_list);
int isValidUsername(char *userName);
int createUser(char *message);
void add_in_group_chat(int socket_id);
void remove_from_group_chat(int socket_id);
void print_group_chat_socket();
void group_chat(char *message);
void send_group(char *message, int from_id);
void exit_group_chat(char *message);
void send_num_in_group_chat(char *message);
int getUserID(char *message);
void addAdmin();
void admin_funtions(char *message);
void add_in_ban(int socket_id);
void remove_from_ban(int socket_id);
int is_ban(int socket_id);
void kick_out(int socket_id);
void store_chat(int socket_id, char *message, char property);
void server_generate_file_name(int socket_id, char *file_name, char property);
void changePassword(char *buff);
void file_transfer(char *message);
void generate_transfer_file_name(char *pass, char *folder, char *file_name);

int nextUserID;

#endif // SERVER_H_INCLUDED