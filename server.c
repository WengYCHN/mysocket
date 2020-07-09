#include <stdio.h>
#include <windows.h>

int count = 0;
SOCKET client_socket[1024];
SOCKADDR_IN cAddr = {0};
int cAddr_len = sizeof(cAddr);

void get_connection(int index);

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
        SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == SOCKET_ERROR)
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

        // Bind server socket with protocol cluster.
        int r = bind(server_socket, (struct sockaddr*)&addr, sizeof(addr));
        if (r == -1)
        {
                printf("Error:Bind failed.\n");
                closesocket(server_socket);
                WSACleanup();
                return -1;
        }
        printf("Info:Bind successful.\n");

        // Set socket listener.
        r = listen(server_socket, 10);
        if (r == -1)
        {
                printf("Error:Listen failed.\n");
                closesocket(server_socket);
                WSACleanup();
                return -1;
        }
        printf("Info:Listen successful.\n");

        // Get client connection.
        while(1)
        {
                client_socket[count] = accept(server_socket, (struct sockaddr*)&cAddr, &cAddr_len);

                if (client_socket[count] == SOCKET_ERROR)
                {
                        printf("Error:Server down.\n");
                        closesocket(server_socket);
                        WSACleanup();
                        return -1;
                }
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)get_connection, (char*)count, NULL, NULL);
                count++;
                printf("Info:Connection successful:%s.\n",inet_ntoa(cAddr.sin_addr));
        }

        closesocket(server_socket);
        WSACleanup();

        return 0;
}

void get_connection(int index)
{
        char buff[1024];
        int r;
        while(1)
        {
                r = recv(client_socket[index], buff, 1023, NULL);
                if (r > 0)
                {
                        buff[r] = 0;
                        printf(">>%d %s\n",index,buff );
                        for (int i = 0; i < count; ++i)
                        {
                                send(client_socket[i], buff, strlen(buff), NULL);
                        }
                }
                else if(r == -1)
                {
                        printf("Error:Client down.\n");
                        return;
                }
        }
}