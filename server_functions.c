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

Hotel **RoomsByBedCount(Hotel **hotels, int target_beds)
{
    Hotel **found_hotels = NULL;
    int hotel_count = getsize(hotels);

    // Allocate memory for the new array of hotels
    Hotel **new_hotels = malloc(hotel_count * sizeof(Hotel *));
    int new_hotel_index = 0;

    // Create the new hotels with valid rooms
    for (int i = 0; i < hotel_count; i++)
    {
        Hotel *hotel = hotels[i];
        int room_count = hotel->room_count;
        Room **rooms = hotel->rooms;

        // Create a new hotel with valid rooms
        Hotel *new_hotel = malloc(sizeof(Hotel));
        new_hotel->name = hotel->name;
        new_hotel->location = hotel->location;
        new_hotel->description = hotel->description;
        new_hotel->room_count = 0;
        new_hotel->rooms = malloc(room_count * sizeof(Room *));

        for (int j = 0; j < room_count; j++)
        {
            Room *room = rooms[j];

            if (room->beds == target_beds)
            {
                // Create a new room with the same values
                Room *new_room = malloc(sizeof(Room));
                new_room->price = room->price;
                new_room->description = room->description;
                new_room->booked_dates_count = room->booked_dates_count;
                new_room->booked_dates = room->booked_dates;
                new_room->beds = room->beds;
                new_room->number = room->number;

                // Add the new room to the new hotel
                new_hotel->rooms[new_hotel->room_count] = new_room;
                new_hotel->room_count++;
            }
            else
            {
                free(room->description);
                free(room);
            }
        }

        if (new_hotel->room_count > 0)
        {
            // Add the new hotel to the new array
            new_hotels[new_hotel_index] = new_hotel;
            new_hotel_index++;
        }
        else
        {
            free(new_hotel);
        }
    }
    new_hotels[new_hotel_index] = NULL;
    return new_hotels;
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

char *roomToString(Room *room)
{
    // Calculate the length of the string representation
    int totalLength = snprintf(NULL, 0, "Price: %d\nDescription: %s\nBooked Dates: ", room->price, room->description);

    for (int i = 0; i < room->booked_dates_count; i++)
    {
        totalLength += snprintf(NULL, 0, "%s, ", room->booked_dates[i]);
    }

    totalLength += snprintf(NULL, 0, "\nBeds: %d\nNumber: %d\n", room->beds, room->number);

    // Create the string buffer
    char *str = (char *)malloc((totalLength + 1) * sizeof(char));

    // Copy the room information into the string buffer
    snprintf(str, totalLength + 1, "Price: %d\nDescription: %s\nBooked Dates: ", room->price, room->description);

    for (int i = 0; i < room->booked_dates_count; i++)
    {
        snprintf(str + strlen(str), totalLength + 1 - strlen(str), "%s, ", room->booked_dates[i]);
    }

    snprintf(str + strlen(str), totalLength + 1 - strlen(str), "\nBeds: %d\nNumber: %d\n", room->beds, room->number);

    return str;
}

char *hotelToString(Hotel *hotel)
{
    // Calculate the length of the string representation
    int totalLength = snprintf(NULL, 0, "Hotel Name: %s\nLocation: %s\nDescription: %s\nRoom Count: %d\n\n",
                               hotel->name, hotel->location, hotel->description, hotel->room_count);

    for (int i = 0; i < hotel->room_count; i++)
    {
        totalLength += snprintf(NULL, 0, "%s\n", roomToString(hotel->rooms[i]));
    }

    // Create the string buffer
    char *str = (char *)malloc((totalLength + 1) * sizeof(char));

    // Copy the hotel and room information into the string buffer
    snprintf(str, totalLength + 1, "Hotel Name: %s\nLocation: %s\nDescription: %s\nRoom Count: %d\n\n",
             hotel->name, hotel->location, hotel->description, hotel->room_count);

    for (int i = 0; i < hotel->room_count; i++)
    {
        snprintf(str + strlen(str), totalLength + 1 - strlen(str), "%s\n", roomToString(hotel->rooms[i]));
    }

    int length = strlen(str);
    str[length] = '\0';
    return str;
}

char **hotelArrString(Hotel **hotels)
{
    int size = getsize(hotels);

    // Allocate memory for the array of strings
    char **finalStrings = malloc(size * sizeof(char *));

    for (int i = 0; i < size; i++)
    {
        // Convert each hotel to a string
        char *hotelString = hotelToString(hotels[i]);

        // Allocate memory for the current hotel string
        finalStrings[i] = malloc(strlen(hotelString) + 15);

        // Copy the hotel string into the current array entry
        strcpy(finalStrings[i], hotelString);

        // Add separator to the current array entry
        strcat(finalStrings[i], "===============\n");

        // Free memory allocated by hotelString
        free(hotelString);
    }

    return finalStrings;
}
