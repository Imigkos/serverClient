#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server_functions.c"

int main(int argc, char **argv)
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
            long file_size;
            ssize_t bytesReceived = recv(clientSocket, &file_size, sizeof(long), 0);
            if (bytesReceived < 0)
            {
                perror("Error receiving file size");
                exit(EXIT_FAILURE);
            }

            FILE *file = fopen("received_hotels.csv", "wb");
            if (file == NULL)
            {
                perror("Failed to open the file for writing");
                exit(EXIT_FAILURE);
            }

            char buffer[1024];
            long bytesRemaining = file_size;
            while (bytesRemaining > 0)
            {
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error receiving file");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }

                size_t bytesWritten = fwrite(buffer, sizeof(char), bytesRead, file);
                if (bytesWritten < bytesRead)
                {
                    perror("Error writing to the file");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }

                bytesRemaining -= bytesRead;
            }

            fclose(file);

            Hotel **hotels_c;
            hotels_c = getHotelData("received_hotels.csv");
            for (int i = 0; i < getsize(hotels_c); i++)
            {
                printHotelData(hotels_c[i]);
            }

            return 0;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    // Close the client socket
    close(clientSocket);
}