//TODO: Написать логику добавления в message сначала Save_buffer-а, а затем проверку recv_buffer-а.
// Перепроверить мысленно код, чтобы все сходилось...

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 12513
#define IP "127.0.0.1"

#define MAX_MESSAGE_LEN 128
#define BUFFER_LEN 512

// Сохраняет байты из массива со \0 в другой массив save_buffer
// input должен иметь \0
// Еще эта функция не проверяет количество памяти в save_buffer
int SaveOverflow(char *input, int inp_size, char* save_buffer, int *save_occupied)
{
    int nullindx = -1;
    for (int i = 0; i < inp_size; i++) if (input[i] == '\0') nullindx = i;
    if (nullindx == -1) return 1;

    int added_counter = 0;
    for (int i = 0; i < (inp_size - (nullindx + 1)); i++)
    {
        save_buffer[*save_occupied + i] = input[nullindx + i];
        added_counter++;
    }
    *save_occupied += added_counter;
    return 0;
}

// DEST И INPUT ОБЯЗАНЫ ИМЕТЬ В СЕБЕ \0
int ConcatStrings(char *dest, int dest_len, char *input, int inp_len)
{
    int nullterm_indx = -1;
    for (int indx = 0; indx < dest_len; indx++) if (dest[indx] == '\0') nullterm_indx = indx;

    if (nullterm_indx == -1) return 1;
    if (inp_len > dest_len) return 1;

    for (int i = 0; i < inp_len; i++)
    {
        char c_ch = input[i];
        dest[nullterm_indx + i] = c_ch;
    }
    return 0;
}

int ReadAll(SOCKET *socket, char *message, char *save_buffer, int *save_occupied)
{
    int left_bytes = MAX_MESSAGE_LEN;
    char *recv_buffer = malloc(BUFFER_LEN);

    int message_len = 0;
    message[0] = '\0';

    while (left_bytes > 0)
    {
        int recv_bytes = recv(socket, recv_buffer, BUFFER_LEN, NULL);
        
        int end_indx = -1;
        for (int i = 0; i < recv_bytes; i++) if (recv_buffer[i] == '\0') end_indx == i;
        
        if (recv_bytes > left_bytes)
        {
            if (end_indx > left_bytes-1 || end_indx == -1)
            {
                printf("ERROR - received big amount of bytes but no end of message;\n");
                break;
            }
            //TODO: нужно написать логику прибавления байтов до \0 в message и байтов после \0 в sabe_buffer (recv_bytes-left_bytes)

            break;
        }
        if (recv_bytes < left_bytes)
        {
            if (end_indx == -1)
            {
                // Добавить все имеющиеся байты в message
                continue;
            }
            // Добавить байты до \0 в message, остальное в save-buffer
            SaveOverflow(recv_buffer, recv_bytes, save_buffer, save_occupied);
            break;
        }
        if (recv_bytes == left_bytes)
        {
            if (end_indx == -1)
            {
                printf("ERROR: message too big or doesnt have \\0 terminator;\n");
                break;
            }
            // Перенести байты до \0 в message, после \0 в save_buffer.
            break;
        }
    }
    return message_len;
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Error while trying to startup the WSA\n");
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Error while creating a socket\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in server_addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};
    inet_pton(AF_INET, IP, &server_addr);

    while (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) printf("\0");

    printf("Looks like its connected\n");
    int isConnected = 1;
    while (isConnected)
    {
        // Основное взаимодействие между клиентом и сервером.
    }
    WSACleanup();
    return 0;
}