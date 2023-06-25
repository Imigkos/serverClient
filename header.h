#define BUFFER_SIZE 1024
#include <stdbool.h>

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

Hotel **getHotelData(char *filename);
int getsize(Hotel **fhotels);
void printRoomData(Room *room);
void printHotelData(Hotel *hotel);
char **searchHotelByLocation(Hotel **hotels, const char *search_location);
Room *searchRoomByBedNumber(Hotel *hotels, int hotel_count, int search_bed_number);
void saveHotelData(Hotel **hotels, int hotel_count);
void freeStringArray(char **strings, int size);
void receiveRequest(int clientSocket, char **found_hotels);
char *roomToString(Room *room);
char **hotelArrString(Hotel **foound_hotels);
bool sendString(char *str, int clientSocket);
bool bookDate(int id, char *buffer);