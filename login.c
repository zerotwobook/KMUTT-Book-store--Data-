#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

typedef struct Book {
    int id;
    char title[100];
    char author[100];
    char category[50];
    int quantity;
    float price;
    struct Book *next;
} Book;

void createAccount() {
    char username[50], password[50];
    char line[MAX_LINE];

    printf("Enter username: ");
    scanf("%s", username);

    // Check if the username already exists in Admin_Password.csv
    FILE *adminFile = fopen("file/Admin_Password.csv", "r");
    if (!adminFile) {
        printf("Error opening file/Admin_Password.csv\n");
        return;
    }

    while (fgets(line, MAX_LINE, adminFile)) {
        char adminUser[50], adminPass[50];
        sscanf(line, "%[^,],%s", adminUser, adminPass);
        if (strcmp(username, adminUser) == 0) {
            printf("Username '%s' already exists. Please choose a different username.\n", username);
            fclose(adminFile);
            return;
        }
    }
    fclose(adminFile);

    // Check if the username already exists in Account.csv
    FILE *userFile = fopen("file/Account.csv", "r");
    if (!userFile) {
        printf("Error opening file/Account.csv\n");
        return;
    }

    while (fgets(line, MAX_LINE, userFile)) {
        char user[50], pass[50];
        sscanf(line, "%[^,],%s", user, pass);
        if (strcmp(username, user) == 0) {
            printf("Username '%s' already exists. Please choose a different username.\n", username);
            fclose(userFile);
            return;
        }
    }
    fclose(userFile);

    // If the username is unique, proceed to create the account
    printf("Enter password: ");
    scanf("%s", password);

    FILE *file = fopen("file/Account.csv", "a");
    if (!file) {
        printf("Error opening file/Account.csv\n");
        return;
    }

    fprintf(file, "%s,%s\n", username, password);
    printf("Account created successfully!\n");
    fclose(file);
}

void login() {
    char username[50], password[50];
    char line[MAX_LINE];
    int isAdmin = 0;

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Check Admin_Password.csv for admin credentials
    FILE *adminFile = fopen("file/Admin_Password.csv", "r"); // Updated path
    if (!adminFile) {
        printf("Error opening file/Admin_Password.csv\n");
        return;
    }

    while (fgets(line, MAX_LINE, adminFile)) {
        char adminUser[50], adminPass[50];
        sscanf(line, "%[^,],%s", adminUser, adminPass);
        if (strcmp(username, adminUser) == 0 && strcmp(password, adminPass) == 0) {
            isAdmin = 1;
            break;
        }
    }
    fclose(adminFile);

    if (isAdmin) {
        // Redirect to admin.c
        system("admin.exe");
        exit(0); // Close the login system
    }

    // Check Account.csv for user credentials
    FILE *userFile = fopen("file/Account.csv", "r"); // Updated path
    if (!userFile) {
        printf("Error opening file/Account.csv\n");
        return;
    }

    int isUser = 0;
    while (fgets(line, MAX_LINE, userFile)) {
        char user[50], pass[50];
        sscanf(line, "%[^,],%s", user, pass);
        if (strcmp(username, user) == 0 && strcmp(password, pass) == 0) {
            isUser = 1;
            break;
        }
    }
    fclose(userFile);

    if (isUser) {
        // Redirect to main.c
        system("main.exe");
        exit(0); // Close the login system
    } else {
        printf("Invalid username or password.\n");
    }
}

Book* loadBooksFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return NULL;
    }

    Book *head = NULL, *tail = NULL;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Book *newBook = (Book *)malloc(sizeof(Book));
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &newBook->id, newBook->title, newBook->author, newBook->category, &newBook->quantity, &newBook->price);
        newBook->next = NULL;

        if (!head) {
            head = newBook;
            tail = newBook;
        } else {
            tail->next = newBook;
            tail = newBook;
        }
    }

    fclose(file);
    return head;
}

void showBooks() {
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");
    if (!bookList) {
        return;
    }

    printf("\nBooks in Stock:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    Book *current = bookList;
    while (current) {
        printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");

    // Free the linked list
    while (bookList) {
        Book *temp = bookList;
        bookList = bookList->next;
        free(temp);
    }
}

int main() {
    int choice;
    do {
        printf("\n1. Login\n2. Create Account\n3. Show Books\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                createAccount();
                break;
            case 3:
                showBooks(); 
                break;
            case 4:
                printf("Exiting\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}
