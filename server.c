#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "header.h"
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

Hotel **hotels;

void *clientHandler(void *arg)
{
    int clientSocket = *(int *)arg;
    SearchRequest request;

    // Receive client request
    ssize_t bytesRead = recv(clientSocket, &request, sizeof(SearchRequest), 0);
    if (bytesRead < 0)
    {
        perror("Error receiving client request");
        close(clientSocket);
        return NULL;
    }

    // Process client requests based on the received input
    switch (request.choice)
    {
    case 1:
        Hotel *found_hotel;
        found_hotel = searchHotelByLocation(hotels, request.query);

        // Convert the found hotel to JSON
        char *json_hotel= hotelToJson(found_hotel);

        ssize_t bytesSent = send(clientSocket, json_hotel, sizeof(Hotel), 0);

        if (bytesSent < 0)
        {
            perror("Error sending search results");
            close(clientSocket);
            return NULL;
        }
        break;
        printHotelData(found_hotel);
        break;
    case 2:
        // Handle case 2: perform action for choice 2
        break;
    case 3:
        // Handle case 3: perform action for choice 3
        break;
    case 4:
        // Handle case 4: perform action for choice 4
        break;
    default:
        // Handle invalid choice: send an error response or perform other actions
        break;
    }
    close(clientSocket);
    return NULL;
}

int main(int argc, char ** argv)
{
    // Get the hotel data from the function
    hotels = getHotelData();

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    pthread_t threadId;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // You can choose a different port number if needed

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    // Listen for client connections
    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening for connections...\n");

    while (1)
    {
        socklen_t clientLen = sizeof(clientAddr);

        // Accept client connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0)
        {
            perror("Error accepting client connection");
            exit(EXIT_FAILURE);
        }

        printf("New client connected. Client IP: %s\n", inet_ntoa(clientAddr.sin_addr));

        // Create a new thread to handle the client
        if (pthread_create(&threadId, NULL, clientHandler, (void *)&clientSocket) != 0)
        {
            perror("Error creating client handler thread");
            exit(EXIT_FAILURE);
        }

        // Detach the thread as we don't need to join it
        pthread_detach(threadId);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
