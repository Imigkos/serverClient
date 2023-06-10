#define BUFFER_SIZE 1024

// hotel room structure
typedef struct {
    double price[4];
    char *description;
    int booked_dates_count;
    char **booked_dates;
    int beds;
    int number;
} Room;

// hotel structure
typedef struct {
    char *name;
    char *location;
    char *description;
    int room_count;
    Room **rooms;
} Hotel;

typedef struct {
    int choice;
    char query[BUFFER_SIZE];
} SearchRequest;


Hotel **getHotelData();
int getsize(Hotel **hotels);
void printRoomData(Room *room);
void printHotelData(Hotel *hotel);
Hotel *searchHotelByLocation(Hotel **hotels, const char *search_location);
Room *searchRoomByBedNumber(Hotel *hotels, int hotel_count, int search_bed_number);
char *hotelToJson(const Hotel *hotel);
Hotel *jsonToHotel(const char *json_str);