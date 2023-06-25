#include "client.h"

int clientSocket;

void sendMenuChoice(char *buffer, int choice)
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

char *convertDate(const char *date)
{
    char converted_date[11];
    sprintf(converted_date, "%.2s%.2s-%.2s%.2s", date + 3, date, date + 9, date + 6);

    // Allocate memory for the converted date string
    char *result = (char *)malloc((strlen(converted_date) + 1) * sizeof(char));
    strcpy(result, converted_date);

    return result;
}


bool isValidDate(const char *date)
{
    // Check the length of the date string
    if (strlen(date) != 11)
        return false;

    // Extract day and month values
    int day1 = atoi(date);
    int month1 = atoi(date + 3);
    int day2 = atoi(date + 6);
    int month2 = atoi(date + 9);

    // Perform validity checks
    if (day1 < 1 || day1 > 31 || month1 < 1 || month1 > 12 ||
        day2 < 1 || day2 > 31 || month2 < 1 || month2 > 12)
        return false;

    return true;
}

char *receiveString()
{
    // Receive the size of the current string
    long str_size;
    int bytes_received = recv(clientSocket, &str_size, sizeof(long), 0);
    if (bytes_received == -1)
    {
        perror("Failed to receive string size");
        close(clientSocket);
        return NULL;
    }

    // Create a buffer to receive the string
    char *rbuffer = malloc(str_size);

    // Receive the string itself
    bytes_received = recv(clientSocket, rbuffer, str_size, 0);
    if (bytes_received == -1)
    {
        perror("Failed to receive string");
        close(clientSocket);
        return NULL;
    }

    // Null-terminate the received string
    rbuffer[str_size - 1] = '\0';
    return rbuffer;
}

int doRequest(int choice)
{
    char buffer[BUFFER_SIZE];

    // Prompt the user to enter the location query
    printf("Location example:'athens'\nBed Number example:'2'\nPrice Number example:'150-250'\n");
    printf("Enter search query:");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    sendMenuChoice(buffer, choice);

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

    if (str_count == 0)
    {
        return 0;
    }

    // Receive each string in the array
    for (int i = 0; i < str_count; i++)
    {
        // Store the received string in the array
        receivedStrings[i] = receiveString();
    }

    // Print the received strings
    for (int i = 0; i < str_count; i++)
    {
        printf("===================\n");
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

