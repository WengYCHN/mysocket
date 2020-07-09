#include <stdio.h>
#include <windows.h>

void get_message();

SOCKET client_socket;

int main(int argc, char const *argv[])
{
        // Get protocol version.
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2),&wsaData);
        if ( LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
                printf("Error:Request protocol version failed.\n");

                return -1;
        }
        printf("Info:Request protocol version successful.\n");

        // Create socket.
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_socket == SOCKET_ERROR)
        {
                printf("Error:Socket creation failed.\n");
                WSACleanup();
                return -1;
        }
        printf("Info:Socket creation successful.\n");

        // Create protocol cluster.
        SOCKADDR_IN addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.6");
        addr.sin_port = htons(12345);

        // Connect to the server.
        int r = connect(client_socket, (struct sockaddr*)&addr, sizeof(addr));
        if (r == -1)
        {
                printf("Error:Connection failed.\n");
                return -1;
        }
        printf("Info:Connection successful.\n");

        // Create thread for get message.
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)get_message, NULL, NULL, NULL);

        // Send message.
        char buff[1024];
        while(1)
        {
                memset(buff, 0, 1024);
                printf("What do you want to say:");
                gets(buff);
                r = send(client_socket, buff, strlen(buff), NULL);
                if (r == -1)
                {
                        printf("Error:Send failed.\n");
                        return -1;
                }
                printf("Info:Send successful.\n");
        }

        closesocket(client_socket);
        WSACleanup();

        return 0;
}


void get_message()
{
        char recv_buff[1024];
        int r;
        while(1)
        {
                r = recv(client_socket, recv_buff, 1023, NULL);
                if (r > 0)
                {
                        recv_buff[r] = 0;
                        printf("\nYou got a new message:%s\n", recv_buff);
                }
        }
}