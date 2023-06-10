#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server_functions.c"

int main(int argc, char ** argv)
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Set server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Same port number used by the server
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr)) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    int choice;

    // Display the menu and get user's choice
    printf("What do you want to search based on?\n");
    printf("1) Location\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Clear the newline character from the input buffer

    if (choice == 1)
    {
        // Prompt the user to enter the location query
        printf("Enter the location: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // Create the search request
        SearchRequest request;
        request.choice = choice;
        strcpy(request.query, buffer);

        // Send the search request to the server
        if (send(clientSocket, &request, sizeof(SearchRequest), 0) < 0)
        {
            perror("Error sending search request");
            exit(EXIT_FAILURE);
        }

        char buffer[1024];

        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        // Parse the received JSON string into a Hotel struct
        Hotel *found_hotel = jsonToHotel(buffer);

        if (bytesRead < 0)
        {
            perror("Error receiving search results");
            exit(EXIT_FAILURE);
        }
        else if (bytesRead == 0)
        {
            printf("No search results received.\n");
        }
        else
        {
            printHotelData(found_hotel);
        }

        // Close the client socket
        close(clientSocket);

        return 0;
    }
    else
    {
        printf("Invalid choice\n");
        exit(EXIT_FAILURE);
    }
}