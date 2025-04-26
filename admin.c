#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a book
typedef struct Book {
    int id;
    char title[100];
    char author[100];
    char category[50];
    int quantity;
    float price;
    struct Book *next;
} Book;

// Define the structure for an order
typedef struct Order {
    char username[50];
    char bookTitle[100];
    int quantity;
    char status[20];
    struct Order *next;
} Order;

// Define the structure for a queue
typedef struct OrderQueue {
    Order *front;
    Order *rear;
} OrderQueue;

typedef struct History {
    char username[50];
    char bookTitle[100];
    int quantity;
    char status[20];
    struct History *next;
} History;

typedef struct HistoryQueue {
    History *front;
    History *rear;
} HistoryQueue;

typedef struct Log {
    char username[50];
    char bookTitle[100];
    int quantity;
    float totalPrice;
    struct Log *next;
} Log;

typedef struct LogQueue {
    Log *front;
    Log *rear;
} LogQueue;

// Function prototypes
void manageAdminMenu(); // Add this prototype to fix the warning

// Function to initialize the queue
OrderQueue* createQueue() {
    OrderQueue *queue = (OrderQueue *)malloc(sizeof(OrderQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

HistoryQueue* createHistoryQueue() {
    HistoryQueue *queue = (HistoryQueue *)malloc(sizeof(HistoryQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

LogQueue* createLogQueue() {
    LogQueue *queue = (LogQueue *)malloc(sizeof(LogQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to enqueue an order
void enqueue(OrderQueue *queue, char *username, char *bookTitle, int quantity, char *status) {
    Order *newOrder = (Order *)malloc(sizeof(Order));
    strcpy(newOrder->username, username);
    strcpy(newOrder->bookTitle, bookTitle);
    newOrder->quantity = quantity;
    strcpy(newOrder->status, status);
    newOrder->next = NULL;

    if (!queue->rear) {
        queue->front = queue->rear = newOrder;
        return;
    }

    queue->rear->next = newOrder;
    queue->rear = newOrder;
}

void enqueueHistory(HistoryQueue *queue, char *username, char *bookTitle, int quantity, char *status) {
    History *newNode = (History *)malloc(sizeof(History));
    strcpy(newNode->username, username);
    strcpy(newNode->bookTitle, bookTitle);
    newNode->quantity = quantity;
    strcpy(newNode->status, status);
    newNode->next = NULL;

    if (!queue->rear) {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

void enqueueLog(LogQueue *queue, char *username, char *bookTitle, int quantity, float totalPrice) {
    Log *newNode = (Log *)malloc(sizeof(Log));
    strcpy(newNode->username, username);
    strcpy(newNode->bookTitle, bookTitle);
    newNode->quantity = quantity;
    newNode->totalPrice = totalPrice;
    newNode->next = NULL;

    if (!queue->rear) {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to load orders from Orderlist.csv into the queue
void loadOrdersFromFile(const char *filename, OrderQueue *queue) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100], status[20];
        int quantity;

        // Parse the line to extract order details
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", username, bookTitle, &quantity, status);

        // Enqueue the order
        enqueue(queue, username, bookTitle, quantity, status);
    }

    fclose(file);
}

// Function to load books from Book_Stock.csv into a linked list
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

// Function to display the stock from the linked list
void showStock(Book *head) {
    if (!head) {
        printf("No books available in stock.\n");
        return;
    }

    printf("Books in Stock:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    Book *current = head;
    while (current) {
        printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");
}

// Function to display the orders from the queue
void showOrders(OrderQueue *queue) {
    if (!queue->front) {
        printf("No orders available.\n");
        return;
    }

    printf("Orders:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s | %-10s |\n", "Username", "Book Title", "Quantity", "Status");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    Order *current = queue->front;
    int orderIndex = 1;

    // Display orders with an index
    while (current) {
        printf("| %-2d | %-20s | %-30s | %-10d | %-10s |\n", orderIndex, current->username, current->bookTitle, current->quantity, current->status);
        current = current->next;
        orderIndex++;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");

    // Ask admin to confirm and update status
    int confirmIndex;
    printf("Enter the order number to confirm and update status (or 0 to cancel): ");
    scanf("%d", &confirmIndex);

    if (confirmIndex == 0) {
        printf("No order was submitted.\n");
        return;
    }

    // Find the selected order
    current = queue->front;
    Order *prev = NULL;
    orderIndex = 1;

    while (current && orderIndex < confirmIndex) {
        prev = current;
        current = current->next;
        orderIndex++;
    }

    if (!current) {
        printf("Invalid order number.\n");
        return;
    }

    // Update the status of the selected order
    strcpy(current->status, "Confirmed");

    // Append the confirmed order to History_Order.csv
    FILE *historyFile = fopen("file/History_Order.csv", "a");
    if (!historyFile) {
        printf("Error: Could not open History_Order.csv\n");
        return;
    }

    fprintf(historyFile, "%s,%s,%d,%s\n", current->username, current->bookTitle, current->quantity, current->status);
    fclose(historyFile);

    // Rewrite the Orderlist.csv file with the updated status
    FILE *file = fopen("file/Orderlist.csv", "w");
    if (!file) {
        printf("Error: Could not open Orderlist.csv\n");
        return;
    }

    Order *temp = queue->front;
    while (temp) {
        fprintf(file, "%s,%s,%d,%s\n", temp->username, temp->bookTitle, temp->quantity, temp->status);
        temp = temp->next;
    }

    fclose(file);

    printf("Order for '%s' by '%s' has been confirmed and added to History_Order.csv.\n", current->bookTitle, current->username);
}

// Function to display the history of orders from History_Order.csv
void showHistoryOrders() {
    FILE *file = fopen("file/History_Order.csv", "r");
    if (!file) {
        printf("Error: Could not open History_Order.csv\n");
        return;
    }

    printf("History of Orders:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s | %-10s |\n", "Username", "Book Title", "Quantity", "Status");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100], status[20];
        int quantity;

        // Parse the line to extract order details
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", username, bookTitle, &quantity, status);

        // Display the order
        printf("| %-20s | %-30s | %-10d | %-10s |\n", username, bookTitle, quantity, status);
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");

    fclose(file);
}

// Function to display the log of stock transactions from Log_Stockbook.csv
void showLogStockBook() {
    FILE *file = fopen("file/Log_Stockbook.csv", "r");
    if (!file) {
        printf("Error: Could not open Log_Stockbook.csv\n");
        return;
    }

    printf("Log of Stock Transactions:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s | %-10s |\n", "Username", "Book Title", "Quantity", "Total Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100];
        int quantity;
        float totalPrice;

        // Parse the line to extract log details
        sscanf(line, "%[^,],%[^,],%d,%f", username, bookTitle, &quantity, &totalPrice);

        // Display the log
        printf("| %-20s | %-30s | %-10d | $%-9.2f |\n", username, bookTitle, quantity, totalPrice);
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");

    fclose(file);
}

// Function to free the linked list
void freeBookList(Book *head) {
    Book *current = head;
    while (current) {
        Book *temp = current;
        current = current->next;
        free(temp);
    }
}

// Function to free the queue
void freeOrderQueue(OrderQueue *queue) {
    Order *current = queue->front;
    while (current) {
        Order *temp = current;
        current = current->next;
        free(temp);
    }
    free(queue);
}

void freeHistoryQueue(HistoryQueue *queue) {
    History *current = queue->front;
    while (current) {
        History *temp = current;
        current = current->next;
        free(temp);
    }
    free(queue);
}

void freeLogQueue(LogQueue *queue) {
    Log *current = queue->front;
    while (current) {
        Log *temp = current;
        current = current->next;
        free(temp);
    }
    free(queue);
}

void editStockBook() {
    int choice;

    do {
        // Load books into a linked list
        Book *bookList = loadBooksFromFile("file/Book_Stock.csv");
        if (!bookList) {
            printf("Error: Could not load books from Book_Stock.csv\n");
            return;
        }

        printf("\nEdit Stock Book Menu:\n");
        printf("1. Edit Book Details\n");
        printf("2. Delete Book\n");
        printf("3. Add Stock\n");
        printf("4. Exit to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: { // Edit Book Details
                // Show the current stock before asking for the book ID
                printf("\nCurrent Stock:\n");
                showStock(bookList);

                int id;
                printf("Enter the ID of the book to edit: ");
                scanf("%d", &id);

                Book *current = bookList;
                while (current) {
                    if (current->id == id) {
                        printf("Editing book: %s\n", current->title);
                        printf("Enter new title: ");
                        getchar(); // Clear newline
                        fgets(current->title, sizeof(current->title), stdin);
                        current->title[strcspn(current->title, "\n")] = '\0'; // Remove newline

                        printf("Enter new author: ");
                        fgets(current->author, sizeof(current->author), stdin);
                        current->author[strcspn(current->author, "\n")] = '\0'; // Remove newline

                        printf("Enter new category: ");
                        fgets(current->category, sizeof(current->category), stdin);
                        current->category[strcspn(current->category, "\n")] = '\0'; // Remove newline

                        printf("Enter new quantity: ");
                        scanf("%d", &current->quantity);

                        printf("Enter new price: ");
                        scanf("%f", &current->price);

                        printf("Book details updated successfully.\n");
                        break;
                    }
                    current = current->next;
                }

                if (!current) {
                    printf("Book with ID %d not found.\n", id);
                }
                break;
            }
            case 2: { // Delete Book
                // Show the current stock before asking for the book ID
                printf("\nCurrent Stock:\n");
                showStock(bookList);

                int id;
                printf("Enter the ID of the book to delete: ");
                scanf("%d", &id);

                Book *current = bookList, *prev = NULL;
                while (current) {
                    if (current->id == id) {
                        if (prev) {
                            prev->next = current->next;
                        } else {
                            bookList = current->next;
                        }
                        free(current);
                        printf("Book with ID %d deleted successfully.\n", id);
                        break;
                    }
                    prev = current;
                    current = current->next;
                }

                if (!current) {
                    printf("Book with ID %d not found.\n", id);
                }
                break;
            }
            case 3: { // Add Stock
                // Show the current stock before asking for the book ID
                printf("\nCurrent Stock:\n");
                showStock(bookList);

                int id, quantityToAdd;
                printf("Enter the ID of the book to add stock: ");
                scanf("%d", &id);

                Book *current = bookList;
                int found = 0;

                while (current) {
                    if (current->id == id) {
                        printf("Current stock for '%s': %d\n", current->title, current->quantity);
                        printf("Enter quantity to add: ");
                        scanf("%d", &quantityToAdd);
                        current->quantity += quantityToAdd;
                        printf("Stock updated successfully. New stock: %d\n", current->quantity);
                        found = 1;
                        break;
                    }
                    current = current->next;
                }

                if (!found) {
                    // If the book ID is not found, add a new book
                    printf("Book with ID %d not found. Adding a new book.\n", id);

                    Book *newBook = (Book *)malloc(sizeof(Book));
                    newBook->id = id;

                    printf("Enter title: ");
                    getchar(); // Clear newline
                    fgets(newBook->title, sizeof(newBook->title), stdin);
                    newBook->title[strcspn(newBook->title, "\n")] = '\0'; // Remove newline

                    printf("Enter author: ");
                    fgets(newBook->author, sizeof(newBook->author), stdin);
                    newBook->author[strcspn(newBook->author, "\n")] = '\0'; // Remove newline

                    printf("Enter category: ");
                    fgets(newBook->category, sizeof(newBook->category), stdin);
                    newBook->category[strcspn(newBook->category, "\n")] = '\0'; // Remove newline

                    printf("Enter quantity: ");
                    scanf("%d", &newBook->quantity);

                    printf("Enter price: ");
                    scanf("%f", &newBook->price);

                    newBook->next = bookList;
                    bookList = newBook;

                    printf("New book added successfully.\n");
                }
                break;
            }
            case 4: // Exit to Admin Menu
                printf("Returning to Admin Menu...\n");
                freeBookList(bookList);
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

        // Save the updated book list back to the file
        FILE *file = fopen("file/Book_Stock.csv", "w");
        if (!file) {
            printf("Error: Could not open Book_Stock.csv for writing.\n");
            freeBookList(bookList);
            return;
        }

        Book *current = bookList;
        while (current) {
            fprintf(file, "%d,%s,%s,%s,%d,%.2f\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
            current = current->next;
        }

        fclose(file);

        // Show the updated stock
        printf("\nUpdated Stock:\n");
        showStock(bookList);

        freeBookList(bookList);

        printf("Changes saved successfully.\n");

    } while (choice != 4);

    printf("Returning to Admin Menu...\n");
}

void manageCoupons() {
    int choice;
    do {
        printf("\nManage Coupons Menu:\n");
        printf("1. Add Coupon\n");
        printf("2. Edit Coupon\n");
        printf("3. Delete Coupon\n");
        printf("4. Show All Coupons\n");
        printf("5. Exit to Admin Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: { // Add Coupon
                char code[50];
                float discount;
                printf("Enter coupon code: ");
                scanf("%s", code);
                printf("Enter discount percentage (e.g., 10 for 10%%): ");
                scanf("%f", &discount);

                FILE *file = fopen("file/Coupon.csv", "a");
                if (!file) {
                    printf("Error: Could not open Coupon.csv\n");
                    return;
                }

                fprintf(file, "%s,%.2f\n", code, discount);
                fclose(file);

                printf("Coupon added successfully.\n");
                break;
            }
            case 2: { // Edit Coupon
                char code[50];
                float newDiscount;
                printf("Enter the coupon code to edit: ");
                scanf("%s", code);

                FILE *file = fopen("file/Coupon.csv", "r");
                if (!file) {
                    printf("Error: Could not open Coupon.csv\n");
                    return;
                }

                FILE *tempFile = fopen("file/Coupon_temp.csv", "w");
                if (!tempFile) {
                    printf("Error: Could not create temporary file.\n");
                    fclose(file);
                    return;
                }

                char line[256];
                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char existingCode[50];
                    float discount;

                    sscanf(line, "%[^,],%f", existingCode, &discount);

                    if (strcmp(existingCode, code) == 0) {
                        printf("Enter new discount percentage for '%s': ", code);
                        scanf("%f", &newDiscount);
                        fprintf(tempFile, "%s,%.2f\n", code, newDiscount);
                        found = 1;
                    } else {
                        fprintf(tempFile, "%s,%.2f\n", existingCode, discount);
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Coupon.csv");
                    rename("file/Coupon_temp.csv", "file/Coupon.csv");
                    printf("Coupon updated successfully.\n");
                } else {
                    remove("file/Coupon_temp.csv");
                    printf("Coupon with code '%s' not found.\n", code);
                }
                break;
            }
            case 3: { // Delete Coupon
                char code[50];
                printf("Enter the coupon code to delete: ");
                scanf("%s", code);

                FILE *file = fopen("file/Coupon.csv", "r");
                if (!file) {
                    printf("Error: Could not open Coupon.csv\n");
                    return;
                }

                FILE *tempFile = fopen("file/Coupon_temp.csv", "w");
                if (!tempFile) {
                    printf("Error: Could not create temporary file.\n");
                    fclose(file);
                    return;
                }

                char line[256];
                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char existingCode[50];
                    float discount;

                    sscanf(line, "%[^,],%f", existingCode, &discount);

                    if (strcmp(existingCode, code) == 0) {
                        found = 1;
                    } else {
                        fprintf(tempFile, "%s,%.2f\n", existingCode, discount);
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Coupon.csv");
                    rename("file/Coupon_temp.csv", "file/Coupon.csv");
                    printf("Coupon deleted successfully.\n");
                } else {
                    remove("file/Coupon_temp.csv");
                    printf("Coupon with code '%s' not found.\n", code);
                }
                break;
            }
            case 4: { // Show All Coupons
                FILE *file = fopen("file/Coupon.csv", "r");
                if (!file) {
                    printf("Error: Could not open Coupon.csv\n");
                    return;
                }

                printf("Available Coupons:\n");
                printf("---------------------------------------------------\n");
                printf("| %-20s | %-10s |\n", "Coupon Code", "Discount (%)");
                printf("---------------------------------------------------\n");

                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    char code[50];
                    float discount;

                    // Parse the line to extract coupon details
                    sscanf(line, "%[^,],%f", code, &discount);

                    // Display the coupon
                    printf("| %-20s | %-10.2f |\n", code, discount);
                }

                printf("---------------------------------------------------\n");

                fclose(file);
                break;
            }
            case 5: // Exit to Admin Menu
                printf("Returning to Admin Menu...\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 5);
}

// Function to validate admin credentials
int validateAdminCredentials(const char *username, const char *password) {
    FILE *file = fopen("file/Admin_Password.csv", "r");
    if (!file) {
        printf("Error: Could not open Admin_Password.csv\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char storedUsername[50], storedPassword[50];

        // Parse the line to extract username and password
        sscanf(line, "%[^,],%s", storedUsername, storedPassword);

        // Check if the entered username and password match
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            fclose(file);
            return 1; // Credentials are valid
        }
    }

    fclose(file);
    return 0; // Credentials are invalid
}

// Function to manage admin access
void manageAdmin() {
    char username[50], password[50];
    int attempts = 3;

    while (attempts > 0) {
        printf("Enter Admin Username: ");
        scanf("%s", username);
        printf("Enter Admin Password: ");
        scanf("%s", password);

        if (validateAdminCredentials(username, password)) {
            printf("Access Granted. Welcome, %s!\n", username);

            // Call the admin management menu
            manageAdminMenu();
            return;
        } else {
            attempts--;
            printf("Invalid credentials. You have %d attempt(s) remaining.\n", attempts);
        }
    }

    printf("Access Denied. Returning to the main menu.\n");
}

// Function to manage admin menu
void manageAdminMenu() {
    int choice;
    do {
        printf("\nAdmin Management Menu:\n");
        printf("1. Add Admin\n");
        printf("2. Edit Admin\n");
        printf("3. Delete Admin\n");
        printf("4. Exit to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: { // Add Admin
                char newUsername[50], newPassword[50];
                char line[256];

                printf("Enter new admin username: ");
                scanf("%s", newUsername);

                // Check if the username already exists in Admin_Password.csv
                FILE *adminFile = fopen("file/Admin_Password.csv", "r");
                if (!adminFile) {
                    printf("Error: Could not open Admin_Password.csv\n");
                    return;
                }

                int existsInAdmin = 0;
                while (fgets(line, sizeof(line), adminFile)) {
                    char adminUser[50], adminPass[50];
                    sscanf(line, "%[^,],%s", adminUser, adminPass);
                    if (strcmp(newUsername, adminUser) == 0) {
                        existsInAdmin = 1;
                        break;
                    }
                }
                fclose(adminFile);

                if (existsInAdmin) {
                    printf("Username '%s' already exists. Please choose a different username.\n", newUsername);
                    break;
                }

                // Check if the username already exists in Account.csv
                FILE *userFile = fopen("file/Account.csv", "r");
                if (!userFile) {
                    printf("Error: Could not open Account.csv\n");
                    return;
                }

                int existsInAccount = 0;
                while (fgets(line, sizeof(line), userFile)) {
                    char user[50], pass[50];
                    sscanf(line, "%[^,],%s", user, pass);
                    if (strcmp(newUsername, user) == 0) {
                        existsInAccount = 1;
                        break;
                    }
                }
                fclose(userFile);

                if (existsInAccount) {
                    printf("Username '%s' already exists. Please choose a different username.\n", newUsername);
                    break;
                }

                // If the username is unique, proceed to add the admin
                printf("Enter new admin password: ");
                scanf("%s", newPassword);

                FILE *file = fopen("file/Admin_Password.csv", "a");
                if (!file) {
                    printf("Error: Could not open Admin_Password.csv\n");
                    return;
                }

                fprintf(file, "%s,%s\n", newUsername, newPassword);
                fclose(file);

                printf("New admin added successfully.\n");
                break;
            }
            case 2: { // Edit Admin
                char editUsername[50], newPassword[50];
                printf("Enter the username of the admin to edit: ");
                scanf("%s", editUsername);

                FILE *file = fopen("file/Admin_Password.csv", "r");
                if (!file) {
                    printf("Error: Could not open Admin_Password.csv\n");
                    return;
                }

                FILE *tempFile = fopen("file/Admin_Password_temp.csv", "w");
                if (!tempFile) {
                    printf("Error: Could not create temporary file.\n");
                    fclose(file);
                    return;
                }

                char line[256];
                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char storedUsername[50], storedPassword[50];
                    sscanf(line, "%[^,],%s", storedUsername, storedPassword);

                    if (strcmp(storedUsername, editUsername) == 0) {
                        printf("Enter new password for '%s': ", editUsername);
                        scanf("%s", newPassword);
                        fprintf(tempFile, "%s,%s\n", storedUsername, newPassword);
                        found = 1;
                    } else {
                        fprintf(tempFile, "%s,%s\n", storedUsername, storedPassword);
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Admin_Password.csv");
                    rename("file/Admin_Password_temp.csv", "file/Admin_Password.csv");
                    printf("Admin password updated successfully.\n");
                } else {
                    remove("file/Admin_Password_temp.csv");
                    printf("Admin with username '%s' not found.\n", editUsername);
                }
                break;
            }
            case 3: { // Delete Admin
                char deleteUsername[50];
                printf("Enter the username of the admin to delete: ");
                scanf("%s", deleteUsername);

                FILE *file = fopen("file/Admin_Password.csv", "r");
                if (!file) {
                    printf("Error: Could not open Admin_Password.csv\n");
                    return;
                }

                FILE *tempFile = fopen("file/Admin_Password_temp.csv", "w");
                if (!tempFile) {
                    printf("Error: Could not create temporary file.\n");
                    fclose(file);
                    return;
                }

                char line[256];
                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char storedUsername[50], storedPassword[50];
                    sscanf(line, "%[^,],%s", storedUsername, storedPassword);

                    if (strcmp(storedUsername, deleteUsername) == 0) {
                        found = 1;
                    } else {
                        fprintf(tempFile, "%s,%s\n", storedUsername, storedPassword);
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Admin_Password.csv");
                    rename("file/Admin_Password_temp.csv", "file/Admin_Password.csv");
                    printf("Admin with username '%s' deleted successfully.\n", deleteUsername);
                } else {
                    remove("file/Admin_Password_temp.csv");
                    printf("Admin with username '%s' not found.\n", deleteUsername);
                }
                break;
            }
            case 4:
                printf("Returning to Main Menu...\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);
}

void loadHistoryQueue(const char *filename, HistoryQueue *queue) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100], status[20];
        int quantity;

        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", username, bookTitle, &quantity, status);
        enqueueHistory(queue, username, bookTitle, quantity, status);
    }

    fclose(file);
}

void loadLogQueue(const char *filename, LogQueue *queue) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100];
        int quantity;
        float totalPrice;

        sscanf(line, "%[^,],%[^,],%d,%f", username, bookTitle, &quantity, &totalPrice);
        enqueueLog(queue, username, bookTitle, quantity, totalPrice);
    }

    fclose(file);
}

void generateReport() {
    HistoryQueue *historyQueue = createHistoryQueue();
    LogQueue *logQueue = createLogQueue();

    // Load data into queues
    loadHistoryQueue("file/History_Order.csv", historyQueue);
    loadLogQueue("file/Log_Stockbook.csv", logQueue);

    int totalOrders = 0, totalBooksSold = 0;
    float totalRevenue = 0.0;

    // Display History of Orders
    printf("\nHistory of Orders:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s | %-10s |\n", "Username", "Book Title", "Quantity", "Status");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    History *currentHistory = historyQueue->front;
    while (currentHistory) {
        printf("| %-20s | %-30s | %-10d | %-10s |\n", currentHistory->username, currentHistory->bookTitle, currentHistory->quantity, currentHistory->status);
        totalOrders++;
        totalBooksSold += currentHistory->quantity;
        currentHistory = currentHistory->next;
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");

    // Display Log of Stock Transactions
    printf("\nLog of Stock Transactions:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s | %-10s |\n", "Username", "Book Title", "Quantity", "Total Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    Log *currentLog = logQueue->front;
    while (currentLog) {
        printf("| %-20s | %-30s | %-10d | $%-9.2f |\n", currentLog->username, currentLog->bookTitle, currentLog->quantity, currentLog->totalPrice);
        totalRevenue += currentLog->totalPrice;
        currentLog = currentLog->next;
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");

    // Display Summary
    printf("\nSummary:\n");
    printf("Total Orders: %d\n", totalOrders);
    printf("Total Books Sold: %d\n", totalBooksSold);
    printf("Total Revenue: $%.2f\n", totalRevenue);

    // Free the queues
    freeHistoryQueue(historyQueue);
    freeLogQueue(logQueue);
}

int main() {
    int choice;

    // Load books from the file into a linked list
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");

    // Load orders from the file into a queue
    OrderQueue *orderQueue = createQueue();
    loadOrdersFromFile("file/Orderlist.csv", orderQueue);

    do {
        printf("\nAdmin Menu:\n");
        printf("1. Show Books\n");
        printf("2. Show Orders\n");
        printf("3. Show History Orders\n");
        printf("4. Show Log Stock Book\n");
        printf("5. Search Book\n");
        printf("6. Edit Stock Book\n");
        printf("7. Manage Coupons\n");
        printf("8. Manage Admins\n");
        printf("9. Generate Report\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                showStock(bookList);
                break;
            case 2:
                showOrders(orderQueue);
                break;
            case 3:
                showHistoryOrders();
                break;
            case 4:
                showLogStockBook();
                break;
            case 5:
                system("searchadmin.exe");
                exit(0);
                break;
            case 6:
                editStockBook();
                break;
            case 7:
                manageCoupons();
                break;
            case 8:
                manageAdmin(); // Call the manageAdmin function
                break;
            case 9:
                generateReport();
                break;
            case 10:
                printf("Exiting...\n");
                system("login.exe"); // Call the login program
                exit(0); // Exit the current program
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 10);

    // Free the linked list and the queue
    freeBookList(bookList);
    freeOrderQueue(orderQueue);

    return 0;
}