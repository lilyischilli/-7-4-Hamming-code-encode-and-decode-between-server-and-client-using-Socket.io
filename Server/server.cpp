#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080

void decodeHamming(int hammingCode[], int data[]) {
    int circle1;
    int circle2;
    int circle3;
    circle1 = hammingCode[0] ^ hammingCode[2] ^ hammingCode[3] ^ hammingCode[4];
    circle2 = hammingCode[0] ^ hammingCode[1] ^ hammingCode[2] ^ hammingCode[5];
    circle3 = hammingCode[1] ^ hammingCode[2] ^ hammingCode[3] ^ hammingCode[6];

    if ((circle1 == 1)&&(circle2 == 1) && (circle3 == 0))
        data[0] = !hammingCode[0];
    else
        data[0] = hammingCode[0];
    if ((circle2 == 1) && (circle3 == 1) && (circle1 == 0))
        data[1] = !hammingCode[1];
    else
        data[1] = hammingCode[1];
    if ((circle1 == 1) && (circle3 == 1) && (circle2 == 0))
        data[3] = !hammingCode[3];
    else
        data[3] = hammingCode[3];
    if ((circle1 == 1) && (circle2 == 1) && (circle3 == 1))
        data[2] = !hammingCode[2];
    else
        data[2] = hammingCode[2];

    /*data[0] = hammingCode[0];
    data[1] = hammingCode[1];
    data[2] = hammingCode[2];
    data[3] = hammingCode[4];*/
}

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize WinSock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int recvLen;
    int hammingCode[7], data[4];

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind server socket
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Client connected.\n");

    while (1) {
        // Receive data
        recvLen = recv(clientSocket, (char*)hammingCode, sizeof(hammingCode), 0);
        if (recvLen == SOCKET_ERROR) {
            printf("Recv failed: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        printf("Received 7 bits Hamming code: ");
        for (int i = 0; i < 7; i++) {
            printf("%d ", hammingCode[i]);
        }
        printf("\n");
        // Decode 7 bits Hamming code to 4 bits
        decodeHamming(hammingCode, data);
        printf("Decoded data: ");
        for (int i = 0; i < 4; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
        printf("--------------------------------------------------\n");
        // Send acknowledgment to client
        char ack = 'A';
        send(clientSocket, &ack, sizeof(ack), 0);
    }

    // Close client socket
    closesocket(clientSocket);

    // Close server socket
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
