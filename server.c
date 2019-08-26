/* 
 * Project 2: Online Chat Room
 * Course: COP4635
 * Students: Ti Chen and Yizhi Hu
 */

#include "server.h"

int main()
{
    // Check if file existed, delete
    num_group_chat = 0;
    nextUserID = 0;
    num_of_sockets = 0;
    num_of_ban = 0;
    char *address = "127.0.0.1";

    char buff[MAX_HTML_LEN]; // HTML Content

    // create the server socket
    int tcp_server_socket;                               //variable for the socket descriptor
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0); //calling the socket function. Params: Domain of the socket (Internet in this case), type of socket stream (TCP), Protocol (default, 0 for TCP)

    // define the server address
    struct sockaddr_in tcp_server_address;     //declaring a structure for the address
    tcp_server_address.sin_family = AF_INET;   //Structure Fields' definition: Sets the address family of the address the client would connect to
    tcp_server_address.sin_port = htons(PORT); //Passing the port number - converting in right network byte order
    tcp_server_address.sin_addr.s_addr = inet_addr(address);

    addAdmin();
    // binding the socket to the IP address and port
    if (bind(tcp_server_socket, (struct sockaddr *)&tcp_server_address, sizeof(tcp_server_address)) < 0)
    { // Params: which socket, cast for server address, its size
        perror(" In bind");
        exit(EXIT_FAILURE);
    }

    // listen for simultaneous connections
    if (listen(tcp_server_socket, MAX_SERVER) < 0)
    { // which socket, how many connections
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    int tcp_client_socket;
    char requestMessage[MAX_MESSAGE]; // Request Message by client
    while (1)
    {
        printf("Server listening and waiting for client request\n");
        if ((tcp_client_socket = accept(tcp_server_socket, NULL, NULL)) < 0)
        { // server socket to interact with client, structure like before - if you know - else NULL for local connection
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // Make sure all buffers are empty
        memset(requestMessage, 0, MAX_MESSAGE);
        memset(buff, 0, MAX_HTML_LEN);

        pthread_t pid;
        pthread_create(&pid, 0, server_recvMessage, &tcp_client_socket);

        // close(tcp_client_socket);
    }

    close(tcp_server_socket);
    printf("Server close  \n");
    return 0;
}

void *server_recvMessage(void *p)
{
    int socket_list[PRIVATE];
    int num;
    int respond;
    int temp_socket = *(int *)p;
    // char *success = "Successful!\n";
    char *fail = "Fail!\n";
    char *send_id = "id: ";
    while (1)
    {
        char userId[MAX_ADDRESS];
        char buff[MAX_MESSAGE] = {};
        int tempID;
        recv(temp_socket, &buff, sizeof(buff), 0); // params: where (socket), what (string), how much - size of the server response, flags (0)
        printf("\nReceive: %s \n", buff);

        int choice = (int)buff[0] - 48;
        printf("Choice: %d\n", choice);
        switch (choice)
        {
        case 0:
            pthread_exit(0);
            break;
        case 1:
            respond = createUser(buff);
            memset(buff, 0, MAX_MESSAGE);
            if (respond == 1)
            {
                client_sockets[nextUserID] = temp_socket;
                sprintf(userId, "%d", nextUserID++);
            }
            else
            {
                sprintf(userId, "%d", -1);
            }
            strcpy(buff, send_id);
            strcat(buff, userId);
            send(temp_socket, buff, sizeof(buff), 0);
            break;
        case 2:
            tempID = getUserID(buff);
            if (tempID >= 0)
            {
                client_sockets[tempID] = temp_socket;
            }

            memset(buff, 0, MAX_MESSAGE);
            sprintf(userId, "%d", tempID);
            strcpy(buff, send_id);
            strcat(buff, userId);
            send(temp_socket, buff, sizeof(buff), 0);
            break;
        case 3:
            // Check online number
            send_num_in_group_chat(buff);
            break;
        case 4:
            // Group chat
            printf("In group chat ");
            group_chat(buff);
            break;
        case 5:
            // Private chat
            num = tokensize_message(buff, socket_list);
            private_chat(socket_list, num);
            break;
        case 6:
            // File transfer
            file_transfer(buff);
            break;
        case 7:
            // Change the password
            changePassword(buff);
            break;
        case 8:
            // Logout
            break;
        case 9:
            // Admin
            admin_funtions(buff);
            break;
        default:
            printf("Cannot recoginize the choose");
            send(temp_socket, fail, strlen(fail), 0);
            break;
        }
    }
    close(temp_socket);
}

void file_transfer(char *message)
{
    char buff[MAX_MESSAGE];
    strcpy(buff, message);

    strtok(buff, " ");
    char *from_folder = strtok(NULL, " ");
    char *to_folder = strtok(NULL, " ");
    char *file_name = strtok(NULL, " ");

    char from_pass[MAX_MESSAGE];
    char to_pass[MAX_MESSAGE];

    generate_transfer_file_name(from_pass, from_folder, file_name);
    generate_transfer_file_name(to_pass, to_folder, file_name);

    rename(from_pass, to_pass);
    char *send_request = "\nYou have received a file!";
    int to_id;
    sscanf(to_folder, "%d", &to_id);
    send(client_sockets[to_id], send_request, strlen(send_request), 0);
}

void generate_transfer_file_name(char *pass, char *folder, char *file_name)
{
    strcpy(pass, folder);
    strcat(pass, "/");
    strcat(pass, file_name);
}

void admin_funtions(char *message)
{
    char buff[MAX_MESSAGE];
    strcpy(buff, message);
    int choice, target;
    char *token = strtok(buff, " ");

    token = strtok(NULL, " ");
    sscanf(token, "%d", &choice);

    token = strtok(NULL, " ");
    sscanf(token, "%d", &target);

    printf("Admin choice: %d, target %d\n", choice, target);
    if (choice == 1)
    {
        add_in_ban(target);
    }
    else if (choice == 2)
    {
        remove_from_ban(target);
    }
    else if (choice == 3)
    {
        kick_out(target);
    }
    else
    {
        printf("Choice not found\n");
    }
    char *successful = "Admin function finished!";
    send(client_sockets[0], successful, strlen(successful), 0);
}

void add_in_ban(int socket_id)
{
    ban_sockets[num_of_ban++] = socket_id;
}

void remove_from_ban(int socket_id)
{
    int i, j;
    for (i = 0; i < num_of_ban; i++)
    {
        if (ban_sockets[i] == socket_id)
        {
            break;
        }
    }
    num_of_ban--;

    for (j = i; j < num_of_ban; j++)
    {
        ban_sockets[j] = ban_sockets[j + 1];
    }
}

void kick_out(int socket_id)
{
    char *kick_out_message = "0 admin request you to kick out, password 1234";
    send(client_sockets[socket_id], kick_out_message, strlen(kick_out_message), 0);
}

void send_num_in_group_chat(char *message)
{
    char buff[MAX_MESSAGE];
    char *header_num = "Number of users in group chat: ";
    char num_str[10];
    sprintf(num_str, "%d", num_group_chat);
    strcpy(buff, header_num);
    strcat(buff, num_str);

    int from_id;
    char *token = strtok(message, " ");
    token = strtok(NULL, " ");
    sscanf(token, "%d", &from_id);

    send(client_sockets[from_id], buff, strlen(buff), 0);
    printf("%s\n", buff);
}

int tokensize_message(char *message, int *socket_list)
{
    char buff[MAX_MESSAGE];
    strcpy(buff, message);
    printf("%s\n", buff);
    char *token = strtok(buff, " ");
    int i = 0;
    int x;
    while (token)
    {
        token = strtok(NULL, " ");
        if (token == NULL)
            break;

        sscanf(token, "%d", &x);
        socket_list[i++] = x;
    }
    return i;
}

void exit_group_chat(char *message)
{
    char buff[MAX_MESSAGE];
    int from_id;
    strcpy(buff, message);
    char *token = strtok(buff, " ");
    token = strtok(NULL, " ");
    sscanf(token, "%d", &from_id);

    remove_from_group_chat(from_id);
    print_group_chat_socket();
}

void group_chat(char *message)
{
    char buff[MAX_MESSAGE];
    int from_id;
    strcpy(buff, message);
    char *token = strtok(buff, " ");
    token = strtok(NULL, " ");
    sscanf(token, "%d", &from_id);

    char *flag = "-=| group chat |=-";
    send(client_sockets[from_id], flag, strlen(flag), 0);

    add_in_group_chat(from_id);
    print_group_chat_socket();
    while (1)
    {
        char buff[MAX_MESSAGE] = {};
        recv(client_sockets[from_id], &buff, sizeof(buff), 0);
        if (strcmp(buff, "exit") == 0)
        {
            remove_from_group_chat(from_id);
            break;
        }
        if (is_ban(from_id) != 1 && strlen(buff) != 0)
        {
            send_group(buff, from_id);
        }
    }
}

int is_ban(int socket_id)
{
    int i;
    for (i = 0; i < num_of_ban; i++)
    {
        if (ban_sockets[i] == socket_id)
        {
            return 1;
        }
    }
    return 0;
}

void send_group(char *message, int from_id)
{
    char send_mssage[MAX_MESSAGE];
    char *start = "Received a group message from ";
    char *temp = ": ";
    char id_str[10];
    sprintf(id_str, "%d", from_id);
    strcpy(send_mssage, start);
    strcat(send_mssage, id_str);
    strcat(send_mssage, temp);
    strcat(send_mssage, message);
    int i;
    for (i = 0; i < num_group_chat; i++)
    {
        if (group_chat_sockets[i] == from_id)
            continue;
        send(client_sockets[group_chat_sockets[i]], send_mssage, strlen(send_mssage), 0);
        store_chat(group_chat_sockets[i], send_mssage, 'g');
    }
}

void private_chat(int *socket_ls, int num)
{
    char *flag = "-=| private chat |=-";
    send(client_sockets[socket_ls[0]], flag, strlen(flag), 0);

    while (1)
    {
        char buff[MAX_MESSAGE] = {};
        recv(client_sockets[socket_ls[0]], &buff, sizeof(buff), 0);
        if (strcmp(buff, "exit") == 0)
        {
            break;
        }
        if (strlen(buff) != 0)
        {
            send_private(buff, socket_ls);
        }
    }
}

void send_private(char *message, int *socket_ls)
{
    char send_mssage[MAX_MESSAGE];
    char *start = "Received a private message from ";
    char *temp = ": ";
    char id_str[10];
    sprintf(id_str, "%d", socket_ls[1]);
    strcpy(send_mssage, start);
    strcat(send_mssage, id_str);
    strcat(send_mssage, temp);
    strcat(send_mssage, message);

    send(client_sockets[socket_ls[1]], send_mssage, strlen(send_mssage), 0);
    store_chat(socket_ls[1], send_mssage, 'p');
}

int isValidUsername(char *userName)
{
    printf("In is ValidUsername\n");
    FILE *fpR = fopen("accountInfo.txt", "r+");
    char existingUsername[USERNAME_LENGTH];
    while (1)
    {
        while (fgetc(fpR) != ' ')
            ;
        fscanf(fpR, "%s", existingUsername);
        printf("get existing user name: %s\n", existingUsername);
        if (strcmp(userName, existingUsername) == 0)
        {
            fclose(fpR);
            return 0;
        }
        memset(existingUsername, 0, USERNAME_LENGTH);
        while (fgetc(fpR) != '\n')
            ;
        if (fgetc(fpR) == EOF)
        {
            printf("In the end of the file\n");
            break;
        }
    }
    fclose(fpR);
    return 1;
}

int createUser(char *message)
{
    printf("in createUser\n");
    char *username;
    char *password;
    const char *delimiter = " ";
    strtok(message, delimiter); // consume the first number
    username = strtok(NULL, delimiter);
    password = strtok(NULL, delimiter);
    if (isValidUsername(username) == 0)
    {
        printf("Invalid user name\n");
        return 0;
    }
    FILE *fpA = fopen("accountInfo.txt", "a+");
    printf("Get username: %s\t password: %s\n", username, password);
    fprintf(fpA, "%d %s %s\n", nextUserID, username, password);
    printf("valid user name\n");
    fclose(fpA);
    return 1;
}

void addAdmin()
{
    FILE *fpA = fopen("accountInfo.txt", "w+");
    fprintf(fpA, "%d %s %s\n", nextUserID, "admin", "admin1234");
    nextUserID++;
    fclose(fpA);
}

void add_in_group_chat(int socket_id)
{
    group_chat_sockets[num_group_chat++] = socket_id;
}

void remove_from_group_chat(int socket_id)
{
    int i, j;
    for (i = 0; i < num_group_chat; i++)
    {
        if (group_chat_sockets[i] == socket_id)
        {
            break;
        }
    }
    num_group_chat--;

    for (j = i; j < num_group_chat; j++)
    {
        group_chat_sockets[j] = group_chat_sockets[j + 1];
    }
}

void print_group_chat_socket()
{
    int i;
    printf("In chat room: ");
    for (i = 0; i < num_group_chat; i++)
    {
        printf("%d ", group_chat_sockets[i]);
    }
    printf("\n");
}

int getUserID(char *message)
{
    if (message == NULL)
    {
        return -1;
    }
    int currentUserID;
    char *username;
    char *password;
    char temp;

    strtok(message, " "); // consume the first number
    username = strtok(NULL, " ");
    password = strtok(NULL, " ");
    FILE *fp = fopen("accountInfo.txt", "r+");
    char existingUsername[USERNAME_LENGTH];
    char truePassword[PASSWORD_LENGTH];
    while (1)
    {
        // currentUserID = (int)fgetc(fp) - 48;
        fscanf(fp, " %d", &currentUserID);
        printf("Current now: %d\n", currentUserID);
        fscanf(fp, "%s", existingUsername);
        printf("get existing user name:  %s\n", existingUsername);
        if (strcmp(username, existingUsername) == 0)
        {
            fscanf(fp, "%s", truePassword);
            printf("get true password: %s\n", truePassword);
            fclose(fp);
            if (strcmp(password, truePassword) == 0)
            {
                printf("Current id: %d\n", currentUserID);
                return currentUserID;
            }
            return -1;
        }
        memset(existingUsername, 0, USERNAME_LENGTH);
        while (fgetc(fp) != '\n')
            ;

        temp = fgetc(fp);
        if (temp == EOF)
        {
            printf("In the end of the file\n");
            break;
        }
        ungetc(temp, fp);
    }
    fclose(fp);
    return -1;
}

void store_chat(int socket_id, char *message, char property)
{
    strcat(message, "\n");
    char file_name[MAX_FILE_NAME];
    server_generate_file_name(socket_id, file_name, property);
    FILE *fp;
    fp = fopen(file_name, "a+");
    fprintf(fp, "%s", message);
    fclose(fp);
}

void server_generate_file_name(int socket_id, char *file_name, char property)
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

void changePassword(char *buff)
{
    char ID;
    char *newPassword;
    char temp;
    char username[USERNAME_LENGTH];
    char newLine[MESSAGE_LENGTH];

    strtok(buff, " ");
    ID = buff[ID_IDX];
    strtok(NULL, " "); // consum ID
    newPassword = strtok(NULL, " ");

    printf("get ID: %c\n", ID);
    printf("get newPassword: %s\n", newPassword);

    FILE *fpR = fopen("accountInfo.txt", "r+");
    FILE *fpW = fopen("accountNew.txt", "w+");

    while (1)
    {
        char tempStr[MESSAGE_LENGTH];
        temp = fgetc(fpR);
        if (temp == EOF)
        {
            break;
        }
        if (temp == ID)
        {
            //fgetc(fpR);      // consume space after ID
            fscanf(fpR, "%s", username);
            newLine[0] = ID;
            newLine[1] = '\0';
            strcat(newLine, " ");
            strcat(newLine, username);
            strcat(newLine, " ");
            strcat(newLine, newPassword);
            strcat(newLine, "\n");

            printf("newline is %s\n", newLine);
            fprintf(fpW, "%s", newLine);
            fgets(tempStr, MESSAGE_LENGTH, fpR);
        }
        else
        {
            ungetc(temp, fpR);
            fgets(tempStr, MESSAGE_LENGTH, fpR);
            fprintf(fpW, "%s", tempStr);
        }
    }

    fclose(fpR);
    fclose(fpW);
    remove("accountInfo.txt");
    int a = rename("accountNew.txt", "accountInfo.txt");
    if (a != 0)
    {
        perror("file name cannot change: \n");
    }
}