#include "client_functions.c"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
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
    serverAddr.sin_port = htons(port); // Same port number used by the server
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
        printf("1)Location\n2)Number of Beds\n3)Price Range\n4)Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character from the input buffer

        if (choice <= 3 && choice >= 1)
        {
            if (doRequest(choice))
            {
                int roomID = 0;
                char book_date[10];
                printf("\nEnter the id of the room you want to book. Otherwise enter 0 to go back to the menu: ");
                scanf("%d", &roomID);
                getchar();

                if (roomID == 0)
                {
                    continue;
                }

                printf("Enter the Date you want to book: ");
                scanf("%s", book_date);
                getchar();
                if (isValidDate(book_date) == 0)
                {
                    printf("\nPlease enter a valid date. Going back to start menu\n");
                    continue;
                }
                char *book_date_conv = convertDate(book_date);
                sendMenuChoice(book_date_conv, roomID);
                char *book_answer = receiveString();
                printf("%s", book_answer);
            }
            else // incase something goes wrong while doing request
            {
                continue;
            }
        }
        else if (choice == 4)
        {
            menu = 0;
            sendMenuChoice("", choice);
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    // Close the client socket
    close(clientSocket);
    exit(1);
}
