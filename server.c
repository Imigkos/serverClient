#include "server_functions.c"

void *clientHandler(void *arg)
{
    int clientSocket = *(int *)arg;
    SearchRequest request;
    int menu = 1;
    while (menu)
    {
        // Receive client request
        ssize_t bytesRead = recv(clientSocket, &request, sizeof(SearchRequest), 0);
        if (bytesRead <= 0)
        {
            perror("Error receiving client request");
            close(clientSocket);
            return NULL;
        }
        char **found_hotels;
        // Process client requests based on the received input
        switch (request.choice)
        {
        case 1:
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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    // Get the hotel data from the function
    hotels = getHotelData("hotels.csv");
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    
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
    serverAddr.sin_port = htons(port); // You can choose a different port number if needed

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