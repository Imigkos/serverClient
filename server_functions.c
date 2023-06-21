#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <json-c/json.h>
#define MAX_ROOMS 100
#define MAX_BOOKED_DATES 100

void remove_newline_character(char *string)
{
    int length = strlen(string);
    if (length > 0 && string[length - 1] == '\n')
    {
        string[length - 1] = '\0';
    }
}

Hotel **getHotelData(char *filename)
{
    // Open the file for reading
    FILE *file = fopen(filename, "r");
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

            token = strtok(NULL, "#");
            room->price = atof(token);

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

            remove_newline_character(token);
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
    hotels[hotel_count] = NULL;
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
    printf("Price : %d\n", room->price);
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

    printf("\nHotel Name: %s\n", hotel->name);
    printf("Location: %s\n", hotel->location);
    printf("Description: %s\n", hotel->description);

    printf("These rooms are available\n");
    for (int j = 0; j < hotel->room_count; j++)
    {
        printf("%d)", j + 1);
        Room *room = hotel->rooms[j];
        printRoomData(room);
    }
    printf("=========================================\n");
}

Hotel **searchHotelByLocation(Hotel **hotels, const char *search_location)
{
    // Create a dynamic array to store the found hotels
    Hotel **found_hotels = NULL;
    int count = 0;

    for (int i = 0; i < getsize(hotels); i++)
    {
        if (strcasecmp(hotels[i]->location, search_location) == 0)
        {
            // Reallocate memory for the dynamic array to accommodate the new hotel
            found_hotels = realloc(found_hotels, (count + 1) * sizeof(Hotel *));
            found_hotels[count] = hotels[i];
            count++;
        }
    }

    return found_hotels;
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

void saveHotelData(Hotel **hotels, int hotel_count)
{
    // Open the file for writing in overwrite mode ("w+")
    FILE *file = fopen("sendHotels.csv", "w+");
    if (file == NULL)
    {
        printf("Failed to open the file for writing.\n");
        return;
    }

    // Write the hotel data to the file
    for (int i = 0; i < hotel_count; i++)
    {
        const Hotel *hotel = hotels[i];

        // Write hotel information
        fprintf(file, "%s#%s#%s#", hotel->name, hotel->location, hotel->description);

        // Write room information for the hotel
        for (int j = 0; j < hotel->room_count; j++)
        {
            const Room *room = hotel->rooms[j];

            // Write room details
            fprintf(file, "%d#%d", room->beds, room->number);

            fprintf(file, "#%d#", room->price);

            // Write booked dates
            for (int k = 0; k < room->booked_dates_count; k++)
            {
                fprintf(file, "%s", room->booked_dates[k]);
                if (k < room->booked_dates_count - 1)
                {
                    fprintf(file, ",");
                }
            }
            fprintf(file, "#");

            // Write room description
            if (j + 1 == hotel->room_count)
            {
                fprintf(file, "%s", room->description);
            }
            else
            {
                fprintf(file, "%s#", room->description);
            }
        }
        fprintf(file, "\n");
    }

    // Close the file
    fclose(file);

    printf("Hotel data saved successfully.\n");
}
