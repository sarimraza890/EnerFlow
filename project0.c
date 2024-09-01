#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

// user structure
typedef struct
{
    char username[50];
    char password[50];
} User;

// energy consumption structure
typedef struct
{
    float consumption[24]; // array to store consumption for each hour of the day
} EnergyConsumption;

int AuthenticateUser(User *users, int numUsers, char *username, char *password);
void GenerateReport(char *username, EnergyConsumption *consumption);
void SaveReportToFile(char *username, EnergyConsumption *consumption);
void PrintSuggestions(float *consumption);

int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, 2); // set the color to green

    User users[10];
    strcpy(users[0].username, "user1234");
    strcpy(users[0].password, "pass1234");
    int numUsers = 1;

    int choice;
    char username[50];
    char password[50];

    EnergyConsumption userConsumption;
    memset(userConsumption.consumption, 0, sizeof(userConsumption.consumption));

    // current time
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);

    // current hour
    int current_hour = current_time->tm_hour;

    // next hour to input consumption
    int next_input_hour = (current_hour + 1) % 24;

    do
    {
        // menu
        printf("\n-----------------------\n");
        printf("\tEnerFlow\t");
        printf("\n-----------------------\n");
        printf("1. Authenticate User\n");
        printf("2. Log Energy Consumption\n");
        printf("3. Generate Report\n");
        printf("4. Save Report\n");
        printf("5. Exit\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // authenticate user
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);

            if (AuthenticateUser(users, numUsers, username, password) != -1)
            {
                printf("Authentication successful\n");
            }
            else
            {
                printf("Authentication failed\n");
            }
            break;

        case 2:
            // log energy consumption
            if (AuthenticateUser(users, numUsers, username, password) != -1)
            {
                float consumption;
                printf("Enter energy consumption for hour %d (current hour is %d): ", next_input_hour, current_hour);
                scanf("%f", &consumption);

                if (consumption >= 0)
                {
                    userConsumption.consumption[next_input_hour] = consumption;
                    printf("Energy consumption for hour %d logged successfully\n", next_input_hour);

                    // Update the next input hour
                    next_input_hour = (next_input_hour + 1) % 24;
                }
                else
                {
                    printf("Please enter a non negative number\n");
                }
            }
            else
            {
                printf("Authentication required before logging consumption\n");
            }
            break;

        case 3:
            // generate report
            if (AuthenticateUser(users, numUsers, username, password) != -1)
            {
                GenerateReport(username, &userConsumption);
                PrintSuggestions(userConsumption.consumption);
            }
            else
            {
                printf("Authentication required before generating report\n");
            }
            break;

        case 4:
            SaveReportToFile(username, &userConsumption);
            printf("Report saved successfully\n");
            break;

        case 5:
            // exit
            printf("Exiting\n");
            break;

        default:
            printf("Invalid choice\n");
        }
    } while (choice != 5);

    SetConsoleTextAttribute(hConsole, 7); // reset color

    return 0;
}

int AuthenticateUser(User *users, int numUsers, char *username, char *password)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            return i; // return user id if authenticated
        }
    }
    return -1; // return -1 if authentication fails
}

void GenerateReport(char *username, EnergyConsumption *consumption)
{
    printf(" Hour of the Day  | Energy Consumption   \n");
    printf("--------------------------------------------\n");

    float totalConsumption = 0.0;
    float peakConsumption = 0.0;
    int peakHour = 0;

    for (int i = 0; i < 24; i++)
    {
        printf(" %02d:00 - %02d:59    | %.2f kWh             \n", i, i, consumption->consumption[i]);
        totalConsumption += consumption->consumption[i];

        if (consumption->consumption[i] > peakConsumption)
        {
            peakConsumption = consumption->consumption[i];
            peakHour = i;
        }
    }

    printf("--------------------------------------------\n");
    printf(" Total            | %.2f kWh             \n", totalConsumption);
    printf("\nPeak Consumption Hour: %02d:00 - %02d:59 with %.2f kWh\n", peakHour, peakHour, peakConsumption);
}

void SaveReportToFile(char *username, EnergyConsumption *consumption)
{
    FILE *file;
    file = fopen("report.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fprintf(file, " Hour of the Day  | Energy Consumption   \n");
    fprintf(file, "--------------------------------------------\n");

    float totalConsumption = 0.0;
    float peakConsumption = 0.0;
    int peakHour = 0;

    for (int i = 0; i < 24; i++)
    {
        fprintf(file, " %02d:00 - %02d:59    | %.2f kWh             \n", i, i, consumption->consumption[i]);
        totalConsumption += consumption->consumption[i];

        if (consumption->consumption[i] > peakConsumption)
        {
            peakConsumption = consumption->consumption[i];
            peakHour = i;
        }
    }

    fprintf(file, "--------------------------------------------\n");
    fprintf(file, " Total            | %.2f kWh             \n", totalConsumption);
    fprintf(file, "\nPeak Consumption Hour: %02d:00 - %02d:59 with %.2f kWh\n", peakHour, peakHour, peakConsumption);

    fclose(file);
}

void PrintSuggestions(float *consumption)
{
    printf("\nSuggestions:\n");

    float totalConsumption = 0.0;
    float peakConsumption = 0.0;
    int peakHour = 0;

    for (int i = 0; i < 24; i++)
    {
        totalConsumption += consumption[i];

        if (consumption[i] > peakConsumption)
        {
            peakConsumption = consumption[i];
            peakHour = i;
        }
    }

    float averageConsumption = totalConsumption / 24;

    printf("- Maintain an average consumption level: %.2f kWh/hour\n", averageConsumption);

    // Analyze distribution of consumption
    int highConsumptionCount = 0;
    for (int i = 0; i < 24; i++)
    {
        if (consumption[i] > averageConsumption)
        {
            highConsumptionCount++;
        }
    }

    if (highConsumptionCount >= 18) // High consumption in more than 75% of hours
    {
        printf("- Consider redistributing energy usage to off-peak hours for better efficiency.\n");
    }

    // Suggestions for specific hours
    for (int i = 0; i < 24; i++)
    {
        if (consumption[i] > averageConsumption)
        {
            printf("- %02d:00 - %02d:59: Use appliances during this time to reduce consumption.\n", i, i);
        }
    }

    printf("\n");
}
