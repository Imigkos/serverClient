#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server_functions.c"

void sendMenuChoice(int clientSocket, char *buffer, int choice)
{
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
}

int doRequest(int choice, int clientSocket)
{
    char buffer[BUFFER_SIZE];

    // Prompt the user to enter the location query
    printf("Location example:'athens'\nBed Number example:'2'\nPrice Number example:'150-250'\n");
    printf("Enter search query:");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    sendMenuChoice(clientSocket, buffer, choice);

    // Receive the number of strings
    long str_count;
    int bytes_received = recv(clientSocket, &str_count, sizeof(long), 0);
    if (bytes_received == -1)
    {
        perror("Failed to receive string count");
        close(clientSocket);
        return 0;
    }

    // Create an array of strings to store the received strings
    char **receivedStrings = malloc(str_count * sizeof(char *));

    if (str_count == 0){
        return 0;
    } 

    // Receive each string in the array
    for (int i = 0; i < str_count; i++)
    {
        // Receive the size of the current string
        long str_size;
        bytes_received = recv(clientSocket, &str_size, sizeof(long), 0);
        if (bytes_received == -1)
        {
            perror("Failed to receive string size");
            close(clientSocket);
            return 0;
        }

        // Create a buffer to receive the string
        char *rbuffer = malloc(str_size);

        // Receive the string itself
        bytes_received = recv(clientSocket, rbuffer, str_size, 0);
        if (bytes_received == -1)
        {
            perror("Failed to receive string");
            close(clientSocket);
            return 0;
        }

        // Null-terminate the received string
        rbuffer[str_size - 1] = '\0';

        // Store the received string in the array
        receivedStrings[i] = rbuffer;
    }

    // Print the received strings
    for (int i = 0; i < str_count; i++)
    {
        printf("%s\n", receivedStrings[i]);
    }

    // Free the memory allocated for the received strings
    for (int i = 0; i < str_count; i++)
    {
        free(receivedStrings[i]);
    }
    free(receivedStrings);
    return 1;
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
    serverAddr.sin_port = htons(8050); // Same port number used by the server
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
        printf("\nWhat do you want to search based on?\n");
        printf("1) Location\n2)Number of Beds\n3)Price Range\n4)Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character from the input buffer

        if (choice <= 3 && choice >= 1)
        {
            if (doRequest(choice, clientSocket))
            {
                int roomID = 0;
                printf("\nEnter the id of the room you want to book. Otherwise enter 0 to go back to the menu: ");
                scanf("%d", &roomID);
                getchar();

                if (roomID == 0)
                {
                    continue;
                }

                printf("Enter the Date you want to book");
            }
            else //incase something goes wrong while doing request
            {
                continue;
            }
        }
        else if (choice == 4)
        {
            menu = 0;
            sendMenuChoice(clientSocket, "", choice);
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    // Close the client socket
    close(clientSocket);
}