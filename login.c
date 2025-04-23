#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

void createAccount() {
    char username[50], password[50];
    FILE *file = fopen("file/Account.csv", "a"); // Updated path
    if (!file) {
        printf("Error opening file/Account.csv\n");
        return;
    }

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Prevent creating an admin account
    if (strcmp(username, "admin") == 0) {
        printf("Cannot create an admin account here.\n");
        fclose(file);
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

int main() {
    int choice;
    do {
        printf("\n1. Login\n2. Create Account\n3. Exit\n");
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
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 3);

    return 0;
}