
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_ROOMS 100
#define MAX_BOOKED_DATES 100
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

// hotel room structure
typedef struct
{
    int price;
    char *description;
    int booked_dates_count;
    char **booked_dates;
    int beds;
    int number;
} Room;

// hotel structure
typedef struct
{
    char *name;
    char *location;
    char *description;
    int room_count;
    Room **rooms;
} Hotel;

typedef struct
{
    int choice;
    char query[BUFFER_SIZE];
} SearchRequest;

void remove_newline_character(char *string);
Hotel **getHotelData(char *filename);
int getsize(Hotel **fhotels);
char **searchHotelByLocation(Hotel **hotels, const char *search_location);
char **RoomsByBedCount(Hotel **hotels, int target_beds);
char **RoomsByPrice(Hotel **hotels, char *price_str);
char *roomToString(Room *room);
char *hotelToString(Hotel *hotel);
char **hotelArrString(Hotel **found_hotels);
bool check_availability(char *new_date_start, char *new_date_end, char **booked_dates, int booked_dates_count);
int getStringArraySize(char **stringArray);
void freeStringArray(char **strings, int size);
void splitDateRange(const char *dateRange, char *startDate, char *endDate);
void appendBookingDate(int hind, int rind, char *new_date);
bool bookDate(int id, char *buffer);
bool sendString(char *str, int clientSocket);
void receiveRequest(int clientSocket, char **found_hotels);
