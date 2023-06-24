#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server_functions.c"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

Hotel **hotels;

int getStringArraySize(char **stringArray)
{
    int size = 0;
    if (stringArray != NULL)
    {
        while (stringArray[size] != NULL)
        {
            size++;
        }
    }
    return size;
}

void receiveRequest(int clientSocket, char **found_hotels)
{
    int num_strings = getStringArraySize(found_hotels);
    long str_size = num_strings; // Number of strings

    // Send the number of strings to the client
    int bytes_sent = send(clientSocket, &str_size, sizeof(long), 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string count");
        close(clientSocket);
        freeStringArray(found_hotels, num_strings); // Free the string array
        return;
    }

    // Send each string in the array to the client
    for (int i = 0; i < num_strings; i++)
    {
        if (sendString(found_hotels[i], clientSocket) == 0)
        {
            freeStringArray(found_hotels, num_strings);
            return;
        }
    }
    freeStringArray(found_hotels, num_strings); // Free the string array
    printf("1");
}

// Helper function to free the memory allocated for the string array
void freeStringArray(char **strings, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(strings[i]);
    }
    free(strings);
}

void *clientHandler(void *arg)
{
    int clientSocket = *(int *)arg;
    SearchRequest request;
    int menu = 1;
    while (menu)
    {
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
            char **found_hotels;
            found_hotels = searchHotelByLocation(hotels, request.query);
            receiveRequest(clientSocket, found_hotels);
            break;
        case 2:
            found_hotels = RoomsByBedCount(hotels, atoi(request.query));
            receiveRequest(clientSocket, found_hotels);
            break;
        case 3:
            found_hotels = RoomsByPrice(hotels, request.query);
            receiveRequest(clientSocket, found_hotels);
            break;
        case 4:
            menu = 0;
            break;
        default:
            if (bookDate(request.choice, request.query) == 1)
            {
                sendString("Room booked succesfully", clientSocket);
            }
            else
            {
                sendString("Room cannot be booked for those dates", clientSocket);
            }
            break;
        }
    }
    close(clientSocket);
    return NULL;
}

int main(int argc, char **argv)
{
    // Get the hotel data from the function
    hotels = getHotelData("hotels.csv");
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    // pthread_t threadId;

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
    serverAddr.sin_port = htons(8060); // You can choose a different port number if needed

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

    pthread_t threadId[MAX_CLIENTS];
    int threadStatus[MAX_CLIENTS]; // Array to track thread availability

    // Initialize threadStatus array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        threadStatus[i] = 0; // 0 indicates thread is available
    }

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

        // Find an available thread slot
        int threadIndex = -1;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (threadStatus[i] == 0)
            {
                // Thread is available
                threadIndex = i;
                threadStatus[i] = 1; // Mark thread as busy
                break;
            }
        }

        if (threadIndex == -1)
        {
            // No available thread slots, handle the error accordingly
            perror("No available thread slot");
            close(clientSocket);
            continue; // Continue to the next iteration
        }

        // Create a new thread to handle the client
        if (pthread_create(&threadId[threadIndex], NULL, clientHandler, (void *)&clientSocket) != 0)
        {
            perror("Error creating client handler thread");
            exit(EXIT_FAILURE);
        }

        // Detach the thread as we don't need to join it
        pthread_detach(threadId[threadIndex]);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

void splitDateRange(const char *dateRange, char *startDate, char *endDate)
{
    strncpy(startDate, dateRange, 4);
    startDate[4] = '\0';

    strncpy(endDate, dateRange + 5, 4);
    endDate[4] = '\0';
}

void appendBookingDate(char **booked_dates, int booked_dates_count, const char *new_date)
{
    // Calculate the new size for the array
    int new_size = booked_dates_count + 1;

    // Reallocate memory for the expanded array
    char **new_booked_dates = (char **)realloc(*booked_dates, new_size * sizeof(char *));

    if (new_booked_dates == NULL)
    {
        // Handle memory allocation error
        printf("Memory allocation failed!\n");
        return;
    }

    // Allocate memory for the new entry and copy the date string
    new_booked_dates[new_size - 1] = (char *)malloc((strlen(new_date) + 1) * sizeof(char));
    strcpy(new_booked_dates[new_size - 1], new_date);

    // Update the booked_dates pointer and count
    booked_dates = new_booked_dates;
    booked_dates_count = new_size;
}

bool bookDate(int id, char *buffer)
{

    char startDate[5];
    char endDate[5];

    splitDateRange(buffer, startDate, endDate);

    int hotel_count = getsize(hotels);
    for (int i = 0; i < hotel_count; i++)
    {
        const Hotel *hotel = hotels[i];

        for (int j = 0; j < hotel->room_count; j++)
        {
            const Room *room = hotel->rooms[j];
            if (room->number == id)
            {
                if (check_availability(startDate, endDate, room->booked_dates, room->booked_dates_count) == 1)
                {
                    appendBookingDate(room->booked_dates,room->booked_dates_count,buffer);
                    return true;
                }
            }
        }
    }
    return false;
}

bool sendString(char *str, int clientSocket)
{
    long str_length = strlen(str) + 1; // Include null terminator

    // Send the size of the current string
    int bytes_sent = send(clientSocket, &str_length, sizeof(long), 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string size");
        close(clientSocket);
        return false;
    }

    // Send the current string
    bytes_sent = send(clientSocket, str, str_length, 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string");
        close(clientSocket);
        return false;
    }
    return true;
}