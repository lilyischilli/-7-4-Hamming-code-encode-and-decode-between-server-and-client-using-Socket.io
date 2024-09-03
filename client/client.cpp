#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define SERVER_IP "127.0.0.1"

void generateHammingCode(int data[], int hammingCode[]) {
    hammingCode[0] = data[0];
    hammingCode[1] = data[1];
    hammingCode[2] = data[2];
    hammingCode[3] = data[3];
    hammingCode[4] = data[0] ^ data[2] ^ data[3];
    hammingCode[5] = data[0] ^ data[1] ^ data[2];  // data3
    hammingCode[6] = data[1] ^ data[2] ^ data[3];  // data3
}

bool isValidInput(int data[]) {
    // Check if each data bit is either 0 or 1
    for (int i = 0; i < 4; i++) {
        if (data[i] != 0 && data[i] != 1) {
            return false;
        }
    }
    return true;
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

    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    int data[4];
    int hammingCode[7];

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    srand(time(NULL));  // Seed for random number generation

    // Main loop
    while (1) {
        printf("Enter 4 bits (separated by spaces): ");
        scanf_s("%d %d %d %d", &data[0], &data[1], &data[2], &data[3]);

        if (isValidInput(data)) {
            // Calculate 7 bits Hamming code and introduce error randomly
            generateHammingCode(data, hammingCode);
            

            // Print encoded 7 bits
            printf("Encoded 7 bits: ");
            for (int i = 0; i < 7; i++) {
                printf("%d ", hammingCode[i]);
            }
            printf("\n");

            int errorIndex = rand() % 7;
            hammingCode[errorIndex] = !hammingCode[errorIndex];
            printf("Encoded 7 bits with 1 bit error: ");
            for (int i = 0; i < 7; i++) {
                printf("%d ", hammingCode[i]);
            }
            printf("\n");

            // Send to server
            send(clientSocket, (char*)hammingCode, sizeof(hammingCode), 0);
            printf("Data sent to server.\n");
            printf("--------------------------------------------------\n");

        }
        else {
            printf("Invalid input. Please enter 4 bits (0 or 1) only.\n");
        }

        // Receive acknowledgment from server
        char ack;
        recv(clientSocket, &ack, sizeof(ack), 0);
    }

    // Close socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
