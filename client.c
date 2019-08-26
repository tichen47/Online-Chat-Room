/* 
 * Project 2: Online Chat Room
 * Course: COP4635
 * Students: Ti Chen and Yizhi Hu
 */

#include "client.h"

int main()
{
    id = INIT_ID;
    pthread_t pid;
    kick_out_message = "0 admin request you to kick out, password 1234";
    exit_chat_message = "exit";
    char serverAddress[MAX_ADDRESS] = "127.0.0.1"; // server address by user input
    while (1)
    {

        //creating the TCP socket
        tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0); //Calling the socket function - args: socket domain, socket stream type, TCP protocol (default)

        //specify address and port of the remote socket
        struct sockaddr_in tcp_server_address;                         //declaring a structure for the address
        tcp_server_address.sin_family = AF_INET;                       //Structure Fields' definition: Sets the address family of the address the client would connect to
        tcp_server_address.sin_port = htons(PORT);                     //Specify and pass the port number to connect - converting in right network byte order
        tcp_server_address.sin_addr.s_addr = inet_addr(serverAddress); //Connecting to server address by user input

        printf("Start connnecting\n");

        //connecting to the remote socket
        int connection_status = connect(tcp_client_socket, (struct sockaddr *)&tcp_server_address, sizeof(tcp_server_address)); //params: which socket, cast for address to the specific structure type, size of address
        if (connection_status == -1)
        { //return value of 0 means all okay, -1 means a problem

            printf(" Problem connecting to the socket! Sorry!! \n");
            continue;
        }
        pthread_create(&pid, 0, client_recvMessage, 0);
        printf("Input: \n");
        startMenu(tcp_client_socket);

        close(tcp_client_socket);
        printf("Connection close\n");
    }

    printf("Client close\n");
    return 0;
}

void *client_recvMessage(void *p)
{
    while (1)
    {
        memset(tcp_server_response, 0, MAX_MESSAGE);
        recv(tcp_client_socket, &tcp_server_response, sizeof(tcp_server_response), 0); // params: where (socket), what (string), how much - size of the server response, flags (0)
        if (strlen(tcp_server_response) == 0)
        {
            continue;
        }
        if (strcmp(tcp_server_response, kick_out_message) == 0)
        {
            send(tcp_client_socket, exit_chat_message, strlen(exit_chat_message), 0);
            send(tcp_client_socket, kick_out_message, strlen(kick_out_message), 0);
            sleep(1);
            exit(0);
        }

        printf("%s\n", tcp_server_response);
        set_id();
    }
}

void getReqMessage(char *message, char *input, int id)
{
    char id_str[10];
    sprintf(id_str, "%d", id);
    char *temp = ": ";
    strcpy(message, id_str);
    strcat(message, temp);
    strcat(message, input);
}

void registerAccount(int tcp_client_socket)
{
    char userName[USERNAME_LENGTH];
    char passWord[PASSWORD_LENGTH];
    char passWordReEnter[PASSWORD_LENGTH];
    printf("Please enter username to register: ");
    scanf("%s", userName);
    printf("Please enter password: ");
    scanf("%s", passWord);
    printf("Please enter password again: ");
    scanf("%s", passWordReEnter);
    if (strcmp(passWord, passWordReEnter) != 0)
    {
        printf("Password does not match!!!\n");
        registerAccount(tcp_client_socket);
    }
    else
    {
        char message[MESSAGE_LENGTH] = "1 ";
        strcat(message, userName);
        strcat(message, " ");
        strcat(message, passWord);
        send(tcp_client_socket, message, strlen(message), 0);
        while (id == INIT_ID)
            ;
        if (id == -1)
        {
            printf("Invaid registration\n");
            id = INIT_ID;
            registerAccount(tcp_client_socket);
        }
    }
}

void set_id()
{
    if (id > -1)
    {
        //printf("Out set_id");
        return;
    }
    char s[MAX_MESSAGE];
    strcpy(s, tcp_server_response);
    strtok(s, " ");
    if (strcmp(s, "id:") == 0)
    {
        char *str_id;
        str_id = strtok(NULL, " ");
        id = atoi(str_id);
        if (id == -1)
        {
            printf("Username already exist! Please use a new username!\n");
        }
        else
        {
            printf("Registration success with id %d!\n", id);
        }
    }
}

int isAbleTologin(int tcp_client_socket)
{
    printf("-=|   Login   -=|\n");

    printf("Please enter the username: ");
    char username[USERNAME_LENGTH];
    scanf("%s", username);

    printf("Please enter the password: ");
    char password[PASSWORD_LENGTH];
    scanf("%s", password);

    char message[MESSAGE_LENGTH];
    strcpy(message, "2 ");
    strcat(message, username);
    strcat(message, " ");
    strcat(message, password);

    send(tcp_client_socket, message, strlen(message), 0);

    while (id == INIT_ID)
        ;
    int returnID = id;

    printf("server response: %d\n", returnID);

    return (returnID);
}

void client_change_password()
{
    printf("-=|   Change Password   |=-\n");

    char newPassword[PASSWORD_LENGTH];
    printf("Enter the new password: ");
    scanf("%s", newPassword);

    char newPassword2[PASSWORD_LENGTH];
    printf("Re-enter the new password: ");
    scanf("%s", newPassword2);

    if (strcmp(newPassword, newPassword2) != 0)
    {
        printf("password does not match!!!\n");
        client_change_password();
    }
    else
    {
        char message[MESSAGE_LENGTH] = "7 ";
        char ID[USERNAME_LENGTH];
        sprintf(ID, "%d", id);
        strcat(message, ID);
        strcat(message, " ");
        strcat(message, newPassword);

        send(tcp_client_socket, message, strlen(message), 0);

        printf("Change password success!\n");
    }
}

void loginMenu(int tcp_client_socket)
{
    int choice;
    int canLogin;
    while (1)
    {
        printf("-=|   Online Chat Room   |=-\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Quit\n");
        printf("Enter an action: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1)
        {
            registerAccount(tcp_client_socket);
            break;
        }
        else if (choice == 2)
        {
            canLogin = isAbleTologin(tcp_client_socket);
            printf("can login is %d\n", canLogin);

            if (canLogin == -1)
            {
                printf("Login Unsuccessful!\n");
                id = INIT_ID;
                continue;
            }
            printf("Login successful!\n");
            break;
        }
        else if (choice == 3)
        {
            printf("Connection closed!\n");
            close(tcp_client_socket);
            exit(0);
        }
        else
        {
            printf("Invalid input! Please enter again\n");
        }
    }
}

void print_menu()
{
    printf("\n-=|MAIN MENU|=-\n");
    printf("1. View current online number\n");
    printf("2. Enter the group chat\n");
    printf("3. Enter the private chat\n");
    printf("4. View chat history\n");
    printf("5. File transfer\n");
    printf("6. Change the password\n");
    printf("7. Logout\n");
    printf("8. Administrator\n");
    printf("0. Return to the login screen\n");
}

void mainMenu(int tcp_client_socket)
{
    print_menu();
    int choice;
    scanf("%d", &choice);
    getchar();

    char str_id[12];
    sprintf(str_id, "%d", id);
    mkdir(str_id, 0700);

    switch (choice)
    {
    case 1:
        // View current online member
        client_num_online();
        break;
    case 2:
        // Group chat
        client_group_chat();
        break;
    case 3:
        // Private chat
        client_private_chat();
        break;
    case 4:
        view_chat_history();
        // View chat history
        break;
    case 5:
        // File transfer
        client_file_transfer();
        break;
    case 6:
        // Change the password
        client_change_password();
        break;
    case 7:
        // Logout
        send(tcp_client_socket, kick_out_message, strlen(kick_out_message), 0);
        sleep(1);
        exit(0);
    case 8:
        // Admin function
        client_admin();
        break;
    case 0:
        // Return to login menu
        id = INIT_ID;
        loginMenu(tcp_client_socket);
    default:
        break;
    }
}

void client_file_transfer()
{
    char file_name[MAX_FILE_NAME];
    int target;
    printf("\n-=|File transfer|=-\n");
    printf("Input the file name: ");
    scanf("%s", file_name);
    printf("Input the target id: ");
    scanf("%d", &target);
    getchar();

    char message[MAX_MESSAGE];
    char *header = "6 ";
    char *space = " ";
    char from_str[10];
    char to_str[10];
    sprintf(from_str, "%d", id);
    sprintf(to_str, "%d", target);
    strcpy(message, header);
    strcat(message, from_str);
    strcat(message, space);
    strcat(message, to_str);
    strcat(message, space);
    strcat(message, file_name);

    send(tcp_client_socket, message, strlen(message), 0);
    printf("Transfer request sent: %s\n", message);
}

void view_chat_history()
{
    int choice;
    printf("\n-=|Chat history|=-\n");
    printf("1. Group Chat\n");
    printf("2. Private Chat\n");
    printf("0. Quit\n");
    scanf("%d", &choice);
    getchar();
    if (choice == 1)
    {
        printf("\n***|Start of group history|***\n");
        view_chat_by_property('g');
        printf("\n***|End of group history|***\n");
    }
    else if (choice == 2)
    {
        printf("\n***|Start of private history|***\n");
        view_chat_by_property('p');
        printf("\n***|End of private history|***\n");
    }
}

void view_chat_by_property(char property)
{
    char c;
    char file_name[MAX_FILE_NAME];
    generate_file_name(id, file_name, property);
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp)
    {
        while ((c = getc(fp)) != EOF)
            putchar(c);
        fclose(fp);
    }
}

void generate_file_name(int socket_id, char *file_name, char property)
{
    char str_id[12];
    sprintf(str_id, "%d", socket_id);
    strcpy(file_name, str_id);
    strcat(file_name, "/");
    if (property == 'p')
    {
        strcat(file_name, "private");
    }
    else if (property == 'g')
    {
        strcat(file_name, "group");
    }
    else
    {
        printf("Shouldn't reach here: generate_file_name\n");
    }
}

void client_admin()
{
    if (id != 0)
    {
        printf("You are not administartor!\n");
        return;
    }
    int choice;
    int target;
    printf("\n-=|Administrator|=-\n");
    printf("1. Ban a member\n");
    printf("2. Dismiss a member\n");
    printf("3. Kick a member out of the chat room\n");
    printf("Enter an action: \n");

    char message[MAX_MESSAGE];
    while (1)
    {
        scanf("%d", &choice);
        getchar();
        if (choice == 1)
        {
            printf("Enter the ID you want to ban\n");
            break;
        }
        else if (choice == 2)
        {
            printf("Enter the ID you want to release/dismiss\n");
            break;
        }
        else if (choice == 3)
        {
            printf("Enter the ID you want to kick out\n");
            break;
        }
        else
        {
            printf("Wrong input, input choice again\n");
        }
    }

    scanf("%d", &target);
    getchar();

    generate_admin_message(message, choice, target);
    send(tcp_client_socket, message, strlen(message), 0);
    printf("Send admin message: %s\n", message);
}

void generate_admin_message(char *message, int choice, int target)
{
    char *header = "9 ";
    char *space = " ";
    char choice_str[10];
    char target_str[10];
    sprintf(choice_str, "%d", choice);
    sprintf(target_str, "%d", target);
    strcpy(message, header);
    strcat(message, choice_str);
    strcat(message, space);
    strcat(message, target_str);
}

void startMenu(int tcp_client_socket)
{
    loginMenu(tcp_client_socket);
    while (1)
    {
        mainMenu(tcp_client_socket);
    }
}

void client_private_chat()
{
    char input[MAX_MESSAGE];
    int chat_id;
    printf("Enter the id you want to chat with: \n");
    scanf("%d", &chat_id);
    getchar();

    client_private_chat_request(input, id, chat_id);
    send(tcp_client_socket, input, strlen(input), 0);

    while (1)
    {
        memset(input, 0, MAX_MESSAGE);
        fgets(input, MAX_MESSAGE, stdin);
        input[strlen(input) - 1] = '\0';

        send(tcp_client_socket, input, strlen(input), 0);
        if (strcmp(input, "exit") == 0)
        {
            break;
        }
        //printf("Message send successfully!\n");
    }
}

void client_private_chat_request(char *message, int from, int to)
{
    char *header = "5 ";
    char *space = " ";
    char from_str[10];
    char to_str[10];
    sprintf(from_str, "%d", from);
    sprintf(to_str, "%d", to);
    strcpy(message, header);
    strcat(message, from_str);
    strcat(message, space);
    strcat(message, to_str);
}

void client_group_chat()
{
    char *exit_message = "exit";
    char input[MAX_MESSAGE];
    generate_request_message(input, id, "4 ");
    send(tcp_client_socket, input, strlen(input), 0);
    printf("\nSend message: %s\n", input);
    while (1)
    {
        memset(input, 0, MAX_MESSAGE);
        fgets(input, MAX_MESSAGE, stdin);
        input[strlen(input) - 1] = '\0';
        if (strcmp(input, exit_message) == 0)
        {
            send(tcp_client_socket, exit_message, strlen(exit_message), 0);
            break;
        }

        send(tcp_client_socket, input, strlen(input), 0);
    }
}

void generate_request_message(char *message, int from, char *header)
{
    char from_str[10];
    sprintf(from_str, "%d", from);
    strcpy(message, header);
    strcat(message, from_str);
}

void client_num_online()
{
    char message[MAX_MESSAGE];
    generate_request_message(message, id, "3 ");
    send(tcp_client_socket, message, strlen(message), 0);
}