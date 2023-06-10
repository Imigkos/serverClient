#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <json-c/json.h>
#define MAX_ROOMS 100
#define MAX_BOOKED_DATES 100

Hotel **getHotelData()
{
    // Open the file for reading
    FILE *file = fopen("hotels.csv", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return NULL;
    }

    // Allocate memory for the hotel array
    Hotel **hotels = malloc(MAX_ROOMS * sizeof(Hotel *));
    int hotel_count = 0;

    // Read the file line by line
    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        // Split the line into tokens using comma as the delimiter
        char *token;
        token = strtok(line, "#");

        // Allocate memory for a new hotel structure
        Hotel *hotel = malloc(sizeof(Hotel));
        hotel->name = strdup(token);

        token = strtok(NULL, "#");
        hotel->location = strdup(token);

        token = strtok(NULL, "#");
        hotel->description = strdup(token);

        // Initialize the room count
        hotel->room_count = 0;

        // Read room data for the hotel
        while ((token = strtok(NULL, "#")))
        {
            Room *room = malloc(sizeof(Room));

            room->beds = atof(token);

            token = strtok(NULL, "#");
            room->number = atof(token);

            for (int i = 0; i < 4; i++)
            {
                token = strtok(NULL, "#");
                room->price[i] = atof(token);
            }

            token = strtok(NULL, ",");
            room->booked_dates_count = 0;

            // Allocate memory for the booked_dates array
            room->booked_dates = malloc(MAX_BOOKED_DATES * sizeof(char *));

            // Split the booked dates using the comma (,) as the delimiter
            while (token != NULL && strstr(token, "-") != NULL)
            {
                if (room->booked_dates_count >= MAX_BOOKED_DATES)
                {
                    printf("Reached the maximum number of booked dates for a room.\n");
                    break;
                }

                room->booked_dates[room->booked_dates_count] = strdup(token);
                room->booked_dates_count++;

                token = strtok(NULL, ",#");
            }

            room->description = strdup(token);

            // Add the room to the hotel
            if (hotel->room_count >= MAX_ROOMS)
            {
                printf("Reached the maximum number of rooms for a hotel.\n");
                break;
            }

            if (hotel->room_count == 0)
            {
                hotel->rooms = malloc(MAX_ROOMS * sizeof(Room *));
            }

            hotel->rooms[hotel->room_count] = room;
            hotel->room_count++;
        }

        // Add the hotel to the array
        hotels[hotel_count] = hotel;
        hotel_count++;
    }

    // Close the file
    fclose(file);

    return hotels;
}

int getsize(Hotel **hotels)
{
    int length = 0;

    // Iterate until a NULL pointer is encountered
    while (hotels[length] != NULL)
    {
        length++;
    }

    return length;
}


void printRoomData(Room *room)
{
    printf("Room Type: %d bed\n", room->beds);
    printf("Room Number: %d\n", room->number);
    printf("Price (Spring): %.2lf\n", room->price[0]);
    printf("Price (Autumn): %.2lf\n", room->price[1]);
    printf("Price (Winter): %.2lf\n", room->price[2]);
    printf("Price (Summer): %.2lf\n", room->price[3]);
    printf("Booked Dates: ");
    for (int k = 0; k < room->booked_dates_count; k++)
    {
        printf("%s ", room->booked_dates[k]);
    }
    printf("\n");

    printf("Room Description: %s\n", room->description);

    printf("\n");
}

void printHotelData(Hotel *hotel)
{

    printf("Hotel Name: %s\n", hotel->name);
    printf("Location: %s\n", hotel->location);
    printf("Description: %s\n", hotel->description);

    for (int j = 0; j < hotel->room_count; j++)
    {
        Room *room = hotel->rooms[j];
        printRoomData(room);
    }

    printf("=========================================\n");
}

Hotel *searchHotelByLocation(Hotel **hotels, const char *search_location)
{
    
    for (int i = 0; i < getsize(hotels); i++)
    {
        if (strcasecmp(hotels[i]->location, search_location) == 0)
        {
            return hotels[i];
        }
    }
    return NULL;
}

Room *searchRoomByBedNumber(Hotel *hotels, int hotel_count, int search_bed_number)
{
    for (int i = 0; i < hotel_count; i++)
    {
        Hotel *current_hotel = &hotels[i];
        for (int j = 0; j < current_hotel->room_count; j++)
        {
            Room *current_room = current_hotel->rooms[j];
            if (current_room->beds == search_bed_number)
            {
                return current_room;
            }
        }
    }
    return NULL; // If no matching room is found
}

char* hotelToJson(const Hotel* hotel) {
    json_object *jhotel = json_object_new_object();
    json_object_object_add(jhotel, "name", json_object_new_string(hotel->name));
    json_object_object_add(jhotel, "location", json_object_new_string(hotel->location));

    json_object *jrooms = json_object_new_array();
    for (int i = 0; i < hotel->room_count; i++) {
        Room *room = hotel->rooms[i];
        json_object *jroom = json_object_new_object();
        json_object_object_add(jroom, "price", json_object_new_double(room->price[0]));
        // Add more room fields as needed
        json_object_array_add(jrooms, jroom);
    }
    json_object_object_add(jhotel, "rooms", jrooms);

    const char *json_str = json_object_to_json_string(jhotel);

    char *result = strdup(json_str); // Allocate memory for the result

    json_object_put(jhotel); // Release the JSON object and its contents

    return result;
}

Hotel* jsonToHotel(const char* json_str) {
    json_object *jhotel = json_tokener_parse(json_str);

    json_object *jname, *jlocation, *jrooms;
    if (!json_object_object_get_ex(jhotel, "name", &jname) ||
        !json_object_object_get_ex(jhotel, "location", &jlocation) ||
        !json_object_object_get_ex(jhotel, "rooms", &jrooms) ||
        !json_object_is_type(jrooms, json_type_array)) {
        // Invalid JSON format, handle the error
        json_object_put(jhotel);
        return NULL;
    }

    Hotel* hotel = malloc(sizeof(Hotel));
    hotel->name = strdup(json_object_get_string(jname));
    hotel->location = strdup(json_object_get_string(jlocation));

    int room_count = json_object_array_length(jrooms);
    hotel->room_count = room_count;
    hotel->rooms = malloc(room_count * sizeof(Room*));

    for (int i = 0; i < room_count; i++) {
        json_object *jroom = json_object_array_get_idx(jrooms, i);

        Room *room = malloc(sizeof(Room));
        room->price[0] = json_object_get_double(json_object_object_get(jroom, "price"));
        // Extract more room fields as needed
        // Remember to handle memory allocation and error checking

        hotel->rooms[i] = room;
    }

    json_object_put(jhotel);

    return hotel;
}