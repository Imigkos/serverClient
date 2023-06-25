#include "server.h"

Hotel **hotels;

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

int getsize(Hotel **fhotels)
{
    int length = 0;

    // Iterate until a NULL pointer is encountered
    while (fhotels[length] != NULL)
    {
        length++;
    }

    return length;
}

char **searchHotelByLocation(Hotel **hotels, const char *search_location)
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
    found_hotels[count] = NULL;
    if (count > 0)
    {
        char **str = hotelArrString(found_hotels);
        free(found_hotels);
        return str;
    }
    free(found_hotels);
    return NULL;
}

char **RoomsByBedCount(Hotel **hotels, int target_beds)
{
    char **roomStrings = NULL;
    int roomCount = 0;

    // Count the number of rooms with the target number of beds
    for (int i = 0; hotels[i] != NULL; i++)
    {
        Hotel *hotel = hotels[i];
        int hotelRoomCount = hotel->room_count;
        int hasRoomWithTargetBeds = 0;

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];

            if (room->beds == target_beds)
            {
                roomCount++;
                hasRoomWithTargetBeds = 1;
            }
        }

        if (hasRoomWithTargetBeds)
        {
            roomCount++; // Count the hotel name as well
        }
    }

    // Allocate memory for the array of room strings
    roomStrings = malloc((roomCount + 1) * sizeof(char *)); // +1 for the terminating NULL

    int roomIndex = 0;

    // Create the strings for the rooms with the target number of beds
    for (int i = 0; hotels[i] != NULL; i++)
    {
        Hotel *hotel = hotels[i];
        int hotelRoomCount = hotel->room_count;
        int hasRoomWithTargetBeds = 0;

        // Calculate the total length of the room strings for the current hotel
        int totalLength = 0;

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];
            if (room->beds == target_beds)
            {
                totalLength += strlen(roomToString(room));
                hasRoomWithTargetBeds = 1;
            }
        }

        if (hasRoomWithTargetBeds)
        {
            totalLength += strlen(hotel->name) + 1; // +1 for the line break
        }
        else
        {
            continue; // Skip hotels without rooms with target beds
        }

        // Create the string buffer for the current hotel
        char *hotelString = malloc((totalLength + 1) * sizeof(char));

        // Append the hotel name and room strings for the current hotel
        if (hasRoomWithTargetBeds)
        {
            snprintf(hotelString, totalLength + 1, "%s\n", hotel->name);
        }

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];
            if (room->beds == target_beds)
            {
                char *roomString = roomToString(room);
                strcat(hotelString, roomString);
                free(roomString);
            }
        }

        roomStrings[roomIndex] = hotelString;
        roomIndex++;
    }

    // Set the terminating NULL
    roomStrings[roomIndex] = NULL;

    if (roomCount > 0)
    {
        return roomStrings;
    }
    return NULL;
}

char **RoomsByPrice(Hotel **hotels, char *price_str)
{
    char **roomStrings = NULL;
    int roomCount = 0;
    int minPrice, maxPrice;

    // parsing price range
    sscanf(price_str, "%d-%d", &minPrice, &maxPrice);

    // Count the number of rooms with the target number of beds
    for (int i = 0; hotels[i] != NULL; i++)
    {
        Hotel *hotel = hotels[i];
        int hotelRoomCount = hotel->room_count;
        int hasRoomWithTargetPrice = 0;

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];

            if (room->price <= maxPrice && room->price >= minPrice)
            {
                roomCount++;
                hasRoomWithTargetPrice = 1;
            }
        }

        if (hasRoomWithTargetPrice)
        {
            roomCount++; // Count the hotel name as well
        }
    }

    // Allocate memory for the array of room strings
    roomStrings = malloc((roomCount + 1) * sizeof(char *)); // +1 for the terminating NULL

    int roomIndex = 0;

    // Create the strings for the rooms with the target number of beds
    for (int i = 0; hotels[i] != NULL; i++)
    {
        Hotel *hotel = hotels[i];
        int hotelRoomCount = hotel->room_count;
        int hasRoomWithTargetPrice = 0;

        // Calculate the total length of the room strings for the current hotel
        int totalLength = 0;

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];
            if (room->price <= maxPrice && room->price >= minPrice)
            {
                totalLength += strlen(roomToString(room));
                hasRoomWithTargetPrice = 1;
            }
        }

        if (hasRoomWithTargetPrice)
        {
            totalLength += strlen(hotel->name) + 1; // +1 for the line break
        }
        else
        {
            continue; // Skip hotels without rooms with target beds
        }

        // Create the string buffer for the current hotel
        char *hotelString = malloc((totalLength + 1) * sizeof(char));

        // Append the hotel name and room strings for the current hotel
        if (hasRoomWithTargetPrice)
        {
            snprintf(hotelString, totalLength + 1, "%s\n", hotel->name);
        }

        for (int j = 0; j < hotelRoomCount; j++)
        {
            Room *room = hotel->rooms[j];
            if (room->price <= maxPrice && room->price >= minPrice)
            {
                char *roomString = roomToString(room);
                strcat(hotelString, roomString);
                free(roomString);
            }
        }

        roomStrings[roomIndex] = hotelString;
        roomIndex++;
    }

    // Set the terminating NULL
    roomStrings[roomIndex] = NULL;

    if (roomCount > 0)
    {
        return roomStrings;
    }
    return NULL;
}

char *roomToString(Room *room)
{
    // Calculate the length of the string representation
    int totalLength = snprintf(NULL, 0, "\nPrice: %d\nDescription: %s\nBooked Dates: ", room->price, room->description);

    for (int i = 0; i < room->booked_dates_count; i++)
    {
        totalLength += snprintf(NULL, 0, "%s, ", room->booked_dates[i]);
    }

    totalLength += snprintf(NULL, 0, "\nBeds: %d\nNumber: %d\n", room->beds, room->number);

    // Create the string buffer
    char *str = (char *)malloc((totalLength + 1) * sizeof(char));

    // Copy the room information into the string buffer
    snprintf(str, totalLength + 1, "\nPrice: %d\nDescription: %s\nBooked Dates: ", room->price, room->description);

    for (int i = 0; i < room->booked_dates_count; i++)
    {
        const char *mmdd = room->booked_dates[i];
        char d1[3] = {mmdd[2], mmdd[3], '\0'};
        char d2[3] = {mmdd[7], mmdd[8], '\0'};
        char m1[3] = {mmdd[0], mmdd[1], '\0'};
        char m2[3] = {mmdd[5], mmdd[6], '\0'};

        snprintf(str + strlen(str), totalLength + 1 - strlen(str), "%s/%s-%s/%s,", d1, m1, d2, m2);
    }

    snprintf(str + strlen(str), totalLength + 4 - strlen(str), "\nBeds: %d\nID: %d\n", room->beds, room->number);

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

char **hotelArrString(Hotel **found_hotels)
{
    int size = getsize(found_hotels);

    // Allocate memory for the array of strings
    char **finalStrings = malloc(size * sizeof(char *));

    for (int i = 0; i < size; i++)
    {
        // Convert each hotel to a string
        char *hotelString = hotelToString(found_hotels[i]);

        // Allocate memory for the current hotel string
        finalStrings[i] = malloc(strlen(hotelString) + 15);

        // Copy the hotel string into the current array entry
        strcpy(finalStrings[i], hotelString);

        // Add separator to the current array entry
        strcat(finalStrings[i], "===============\n");

        // Free memory allocated by hotelString
        free(hotelString);
    }
    finalStrings[size] = NULL;
    return finalStrings;
}

bool check_availability(char *new_date_start, char *new_date_end, char **booked_dates, int booked_dates_count)
{
    for (int i = 0; i < booked_dates_count; i++)
    {
        char booked_date[10];
        strcpy(booked_date, booked_dates[i]);
        char *start_date = strtok(booked_date, "-");
        char *end_date = strtok(NULL, "-");

        // Extract day and month from start and end dates
        int booked_start = atoi(start_date);
        int booked_end = atoi(end_date);

        // Extract day and month from new date range
        int new_start = atoi(new_date_start);
        int new_end = atoi(new_date_end);

        if (new_start <= booked_end && new_end >= booked_start)
        {

            // free(booked_date_copy); // Free the memory allocated for the copy

            return false;
        }
        // free(booked_date_copy); // Free the memory allocated for the copy
    }

    return true;
}

int getStringArraySize(char **stringArray)
{
    int size = 0;
    if (stringArray != NULL)
    {
        while (stringArray[size] != NULL)
        {
            size++;
        }
    }
    return size;
}

// Helper function to free the memory allocated for the string array
void freeStringArray(char **strings, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(strings[i]);
    }
    free(strings);
}

void splitDateRange(const char *dateRange, char *startDate, char *endDate)
{
    strncpy(startDate, dateRange, 4);
    startDate[4] = '\0';

    strncpy(endDate, dateRange + 5, 4);
    endDate[4] = '\0';
}

void appendBookingDate(int hind, int rind, char *new_date)
{
    // Reallocate memory for the new array
    char **new_booked_dates = realloc(hotels[hind]->rooms[rind]->booked_dates, (hotels[hind]->rooms[rind]->booked_dates_count + 1) * sizeof(char *));
    hotels[hind]->rooms[rind]->booked_dates = new_booked_dates;

    // Allocate memory for the new date and copy the value
    hotels[hind]->rooms[rind]->booked_dates[hotels[hind]->rooms[rind]->booked_dates_count] = malloc(strlen(new_date) + 1);
    strcpy(hotels[hind]->rooms[rind]->booked_dates[hotels[hind]->rooms[rind]->booked_dates_count], new_date);

    hotels[hind]->rooms[rind]->booked_dates_count++;
}

bool bookDate(int id, char *buffer)
{

    char startDate[5];
    char endDate[5];

    splitDateRange(buffer, startDate, endDate);

    int hotel_count = getsize(hotels);
    for (int i = 0; i < hotel_count; i++)
    {
        const Hotel *hotel = hotels[i];

        for (int j = 0; j < hotel->room_count; j++)
        {
            const Room *room = hotel->rooms[j];
            if (room->number == id)
            {
                if (check_availability(startDate, endDate, room->booked_dates, room->booked_dates_count) == 1)
                {
                    appendBookingDate(i, j, buffer);
                    return true;
                }
            }
        }
    }
    return false;
}

bool sendString(char *str, int clientSocket)
{
    long str_length = strlen(str) + 1; // Include null terminator

    // Send the size of the current string
    int bytes_sent = send(clientSocket, &str_length, sizeof(long), 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string size");
        close(clientSocket);
        return false;
    }

    // Send the current string
    bytes_sent = send(clientSocket, str, str_length, 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string");
        close(clientSocket);
        return false;
    }
    return true;
}

void receiveRequest(int clientSocket, char **found_hotels)
{
    int num_strings = getStringArraySize(found_hotels);
    long str_size = num_strings; // Number of strings

    // Send the number of strings to the client
    int bytes_sent = send(clientSocket, &str_size, sizeof(long), 0);
    if (bytes_sent == -1)
    {
        perror("Failed to send string count");
        close(clientSocket);
        freeStringArray(found_hotels, num_strings); // Free the string array
        return;
    }

    // Send each string in the array to the client
    for (int i = 0; i < num_strings; i++)
    {
        if (sendString(found_hotels[i], clientSocket) == 0)
        {
            freeStringArray(found_hotels, num_strings);
            return;
        }
    }
    freeStringArray(found_hotels, num_strings); // Free the string array
}