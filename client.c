#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server_functions.c"

void doRequest(int choice, int clientSocket)
{
    char buffer[BUFFER_SIZE];

    // Prompt the user to enter the location query
    printf("Enter search query: ");
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

    // Create a buffer to receive the string
    char rbuffer[2048];

    // Receive the size of the string
    long str_size;
    int bytes_received = recv(clientSocket, &str_size, sizeof(long), 0);
    if (bytes_received == -1)
    {
        perror("Failed to receive string size");
        close(clientSocket);
        return ;
    }

    // Receive the string itself
    bytes_received = recv(clientSocket, rbuffer, str_size, 0);
    if (bytes_received == -1)
    {
        perror("Failed to receive string");
        close(clientSocket);
        return ;
    }

    // Null-terminate the received string
    rbuffer[str_size - 1] = '\0';

    // Print the received string
    printf("Received string: %s\n", rbuffer);
}

int main(int argc, char **argv)
{
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Set server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8040); // Same port number used by the server
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
    int menu = 1;

    while (menu)
    {
        /* code */

        // Display the menu and get user's choice
        printf("What do you want to search based on?\n");
        printf("1) Location\n2)Number of Beds\n3)Price Range");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character from the input buffer

        if (choice == 1)
        {
            doRequest(choice,clientSocket);
        }

        else if (choice == 2)
        {
            doRequest(choice,clientSocket);
        }
        
        else
        {
            printf("Invalid choice\n");
        }
    }
    // Close the client socket
    close(clientSocket);
}