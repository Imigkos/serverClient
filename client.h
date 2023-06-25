#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct
{
    int choice;
    char query[BUFFER_SIZE];
} SearchRequest;

void sendMenuChoice(char *buffer, int choice);
void handleSignal(int signal);
char *convertDate(const char *date);
bool isValidDate(const char *date);
char *receiveString();
int doRequest(int choice);