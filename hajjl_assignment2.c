#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the movie struct
struct movie {
    char* title;
    int year;
    char languages[5][20];
    float rating;
    struct movie* next;
};

int main(int argc, char* argv[]) {
    // Check if the filename is provided by the user
    if (argc < 2) {
        printf("Give me the name of the file to continue\n");
        return 1;
    }

    // Trying to open the file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Could not open the file %s\n", argv[1]);
        return 1;
    }

    char line[1024];  // Buffer for reading lines from the file

    // Skip the header line
    fgets(line, sizeof(line), file);

    // Linked list setup
    struct movie* head = NULL;
    struct movie* tail = NULL;
    int movieCount = 0;

    // Read each line and build the linked list
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        struct movie* newMovie = malloc(sizeof(struct movie));
        newMovie->next = NULL;

        char* token;
        char* rest = line;

        // Parse movie TITLE
        token = strtok_r(rest, ",", &rest);
        if (!token) {
            free(newMovie);
            continue;
        }
        newMovie->title = malloc(strlen(token) + 1);
        strcpy(newMovie->title, token);

        // Parse release YEAR
        token = strtok_r(NULL, ",", &rest);
        if (!token) {
            free(newMovie->title);
            free(newMovie);
            continue;
        }
        newMovie->year = atoi(token);

        // Parse and split LANGUAGES
        token = strtok_r(NULL, ",", &rest);
        if (!token || token[0] != '[') {
            free(newMovie->title);
            free(newMovie);
            continue;
        }

        char languagesStr[100];
        strncpy(languagesStr, token + 1, sizeof(languagesStr) - 1);
        languagesStr[sizeof(languagesStr) - 1] = '\0';
        char* closingBracket = strchr(languagesStr, ']');
        if (closingBracket) *closingBracket = '\0';

        int langIndex = 0;
        char* langToken = strtok(languagesStr, ";");
        while (langToken != NULL && langIndex < 5) {
            strncpy(newMovie->languages[langIndex], langToken, 19);
            newMovie->languages[langIndex][19] = '\0';
            langToken = strtok(NULL, ";");
            langIndex++;
        }

        // Parse rating
        token = strtok_r(NULL, ",", &rest);
        if (!token) {
            free(newMovie->title);
            free(newMovie);
            continue;
        }
        newMovie->rating = strtof(token, NULL);

        // Add to the linked list
        if (head == NULL) {
            head = newMovie;
            tail = newMovie;
        } else {
            tail->next = newMovie;
            tail = newMovie;
        }

        movieCount++;
    }

    // Print file processing summary
    printf("Processed file %s and parsed data for %d movies\n", argv[1], movieCount);

    // MENU system for user interaction
    int choice;

    while (1) {
        printf("\n");
        printf("1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n");
        printf("\nEnter a choice from 1 to 4: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int year;
            int found = 0;
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &year);

            struct movie* current = head;
            while (current != NULL) {
                if (current->year == year) {
                    printf("%s\n", current->title);
                    found = 1;
                }
                current = current->next;
            }

            if (!found) {
                printf("No data about movies released in the year %d\n", year);
            }

        } else if (choice == 2) {
            struct movie* current = head;
            int printedYears[3000] = {0};

            while (current != NULL) {
                int year = current->year;

                if (!printedYears[year]) {
                    struct movie* temp = head;
                    struct movie* best = NULL;
                    float maxRating = -1.0;

                    while (temp != NULL) {
                        if (temp->year == year && temp->rating > maxRating) {
                            best = temp;
                            maxRating = temp->rating;
                        }
                        temp = temp->next;
                    }

                    if (best != NULL) {
                        printf("%d %.1f %s\n", best->year, best->rating, best->title);
                        printedYears[year] = 1;
                    }
                }
                current = current->next;
            }

        } else if (choice == 3) {
            char language[21];
            int found = 0;

            printf("Enter the language for which you want to see movies: ");
            scanf("%20s", language);

            struct movie* current = head;
            while (current != NULL) {
                for (int i = 0; i < 5; i++) {
                    if (strcmp(current->languages[i], language) == 0) {
                        printf("%d %s\n", current->year, current->title);
                        found = 1;
                        break;
                    }
                }
                current = current->next;
            }

            if (!found) {
                printf("No data about movies released in %s\n", language);
            }

        } else if (choice == 4) {
            break;
        } else {
            printf("You entered an incorrect choice. Try again.\n");
        }
    }

    // Free all allocated memory
    struct movie* current = head;
    while (current != NULL) {
        struct movie* temp = current;
        current = current->next;
        free(temp->title);
        free(temp);
    }

    // Close the file
    fclose(file);
    return 0;
}
