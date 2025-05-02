#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void pauseForUser()
{
    printf("\n[Press Enter to continue]");
    getchar();
    getchar();
    system("cls");
}

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
    printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s      |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    Book *current = head;
    while (current) {
        printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f      |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");
}

// Function to display the orders from the queue
void showOrders(OrderQueue *queue) {
    if (!queue->front) {
        printf("No orders available.\n");
        pauseForUser();
        system("cls");
        return;
    }

    int continueConfirming = 1;
    
    while (continueConfirming) {
        system("cls"); // Clear screen for each confirmation cycle
        
        // Display all orders
        printf("\nCurrent Orders:\n");
        printf("------------------------------------------------------------------------------------------------\n");
        printf("| #  | %-20s | %-30s | %-8s | %-10s |\n", "Username", "Book Title", "Quantity", "Status");
        printf("------------------------------------------------------------------------------------------------\n");

        Order *current = queue->front;
        int orderIndex = 1;
        while (current) {
            printf("| %-2d | %-20s | %-30s | %-8d | %-10s |\n", 
                  orderIndex, current->username, current->bookTitle, 
                  current->quantity, current->status);
            current = current->next;
            orderIndex++;
        }
        printf("------------------------------------------------------------------------------------------------\n");

        // Get order number to confirm
        int confirmIndex;
        do {
            printf("\nEnter order number to confirm (1-%d, 0 to exit): ", orderIndex-1);
            
            if (scanf("%d", &confirmIndex) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }

            if (confirmIndex == 0) {
                continueConfirming = 0;
                system("cls");
                printf("Exiting order confirmation.\n");
                pauseForUser();
                return;
            }

            if (confirmIndex < 1 || confirmIndex >= orderIndex) {
                printf("Invalid order number. Please try again.\n");
                continue;
            }

            break; // Valid input
        } while (1);

        // Find the selected order
        current = queue->front;
        for (int i = 1; i < confirmIndex; i++) {
            current = current->next;
        }

        // Confirm action
        printf("\nOrder Details:\n");
        printf("----------------------------------------\n");
        printf("Username: %s\n", current->username);
        printf("Book Title: %s\n", current->bookTitle);
        printf("Quantity: %d\n", current->quantity);
        printf("Current Status: %s\n", current->status);
        printf("----------------------------------------\n");

        char confirm;
        printf("\nConfirm this order? (Y/N): ");
        scanf(" %c", &confirm);
        getchar(); // Clear newline

        if (tolower(confirm) != 'y') {
            printf("Confirmation cancelled.\n");
            pauseForUser();
            continue;
        }

        // Update status
        strcpy(current->status, "Confirmed");

        // Append to history file
        FILE *historyFile = fopen("file/History_Order.csv", "a");
        if (historyFile) {
            fprintf(historyFile, "%s,%s,%d,%s\n", 
                    current->username, current->bookTitle, 
                    current->quantity, current->status);
            fclose(historyFile);
        }

        // Rewrite order file
        FILE *orderFile = fopen("file/Orderlist.csv", "w");
        if (orderFile) {
            Order *temp = queue->front;
            while (temp) {
                fprintf(orderFile, "%s,%s,%d,%s\n", 
                        temp->username, temp->bookTitle, 
                        temp->quantity, temp->status);
                temp = temp->next;
            }
            fclose(orderFile);
        }

        printf("\nOrder #%d confirmed successfully!\n", confirmIndex);
        
        // Ask if admin wants to continue
        char continueChoice;
        printf("\nConfirm another order? (Y/N): ");
        scanf(" %c", &continueChoice);
        getchar(); // Clear newline
        
        if (tolower(continueChoice) != 'y') {
            continueConfirming = 0;
        }
    }
    
    system("cls");
    printf("Returning to main menu.\n");
    pauseForUser();
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

// Function to compare strings case-insensitively
int caseInsensitiveCompare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower(*str1) != tolower(*str2)) {
            return 0;
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
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

        // Always show current stock
        printf("\nCurrent Stock (Sorted by ID):\n");
        showStock(bookList);

        printf("\n----------------------------------------------");
        printf("\n|Edit Stock Book Menu:                       |\n");
        printf("----------------------------------------------\n");
        printf("|1. Edit Book Details                        |\n");
        printf("|2. Delete Book                              |\n");
        printf("|3. Add Stock                                |\n");
        printf("|4. Add New Book                             |\n");
        printf("|0. Exit to Admin Menu                       |\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice (0-4): ");

        if (scanf("%d", &choice) != 1 || choice < 0 || choice > 4) {
            printf("Invalid input. Please enter a number between 0 and 4.\n");
            while (getchar() != '\n');
            freeBookList(bookList);
            system("cls");
            continue;
        }

        system("cls");

        if (choice == 0) {
            freeBookList(bookList);
            return;
        }

        // Common variables for all operations
        int id;
        Book *current = NULL;

        if (choice != 4) { // All options except Add New Book need ID input
            printf("\nCurrent Stock (Sorted by ID):\n");
            showStock(bookList);
            
            printf("\nEnter the ID of the book (or 0 to cancel): ");
            if (scanf("%d", &id) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                freeBookList(bookList);
                system("cls");
                continue;
            }

            if (id == 0) {
                freeBookList(bookList);
                system("cls");
                continue;
            }

            // Find the book
            current = bookList;
            while (current && current->id != id) {
                current = current->next;
            }

            if (!current) {
                printf("Error: Book with ID %d not found.\n", id);
                pauseForUser();
                freeBookList(bookList);
                system("cls");
                continue;
            }
        }

        switch (choice) {
            case 1: { // Edit Book Details
                int editChoice;
                do {
                    printf("\nEditing Book: %s (ID: %d)\n", current->title, current->id);
                    printf("----------------------------------------------\n");
                    printf("1. Edit Title\n");
                    printf("2. Edit Author\n");
                    printf("3. Edit Category\n");
                    printf("4. Edit Quantity\n");
                    printf("5. Edit Price\n");
                    printf("0. Finish Editing\n");
                    printf("----------------------------------------------\n");
                    printf("Enter your choice (0-5): ");
                    
                    if (scanf("%d", &editChoice) != 1 || editChoice < 0 || editChoice > 5) {
                        printf("Invalid input. Please enter a number between 0 and 5.\n");
                        while (getchar() != '\n');
                        continue;
                    }

                    getchar(); // Clear newline
                    system("cls");

                    switch (editChoice) {
                        case 1: // Edit Title
                            printf("Current Title: %s\n", current->title);
                            printf("Enter new title: ");
                            fgets(current->title, sizeof(current->title), stdin);
                            current->title[strcspn(current->title, "\n")] = '\0';
                            printf("Title updated successfully.\n");
                            break;
                        case 2: // Edit Author
                            printf("Current Author: %s\n", current->author);
                            printf("Enter new author: ");
                            fgets(current->author, sizeof(current->author), stdin);
                            current->author[strcspn(current->author, "\n")] = '\0';
                            printf("Author updated successfully.\n");
                            break;
                        case 3: // Edit Category
                            printf("Current Category: %s\n", current->category);
                            printf("Enter new category: ");
                            fgets(current->category, sizeof(current->category), stdin);
                            current->category[strcspn(current->category, "\n")] = '\0';
                            printf("Category updated successfully.\n");
                            break;
                        case 4: { // Edit Quantity
                            int newQuantity;
                            printf("Current Quantity: %d\n", current->quantity);
                            printf("Enter new quantity (must be positive): ");
                            if (scanf("%d", &newQuantity) != 1 || newQuantity <= 0) {
                                printf("Invalid input. Quantity must be a positive number.\n");
                                while (getchar() != '\n');
                                break;
                            }
                            current->quantity = newQuantity;
                            printf("Quantity updated successfully.\n");
                            break;
                        }
                        case 5: { // Edit Price
                            float newPrice;
                            printf("Current Price: $%.2f\n", current->price);
                            printf("Enter new price (must be positive): ");
                            if (scanf("%f", &newPrice) != 1 || newPrice <= 0) {
                                printf("Invalid input. Price must be a positive number.\n");
                                while (getchar() != '\n');
                                break;
                            }
                            current->price = newPrice;
                            printf("Price updated successfully.\n");
                            break;
                        }
                        case 0:
                            printf("Finished editing.\n");
                            break;
                    }
                } while (editChoice != 0);
                break;
            }
            case 2: { // Delete Book
                printf("Are you sure you want to delete '%s' (ID: %d)? (1=Yes, 0=No): ", 
                      current->title, current->id);
                int confirm;
                if (scanf("%d", &confirm) != 1) {
                    printf("Invalid input.\n");
                    while (getchar() != '\n');
                    break;
                }

                if (confirm == 1) {
                    // Find and remove the book from the linked list
                    Book *prev = NULL, *temp = bookList;
                    while (temp && temp != current) {
                        prev = temp;
                        temp = temp->next;
                    }

                    if (temp) {
                        if (prev) {
                            prev->next = temp->next;
                        } else {
                            bookList = temp->next;
                        }
                        free(temp);
                        printf("Book deleted successfully.\n");
                    }
                } else {
                    printf("Deletion cancelled.\n");
                }
                break;
            }
            case 3: { // Add Stock
                int quantityToAdd;
                printf("Current stock of '%s': %d\n", current->title, current->quantity);
                printf("Enter quantity to add (must be positive, 0 to cancel): ");
                if (scanf("%d", &quantityToAdd) != 1) {
                    printf("Invalid input.\n");
                    while (getchar() != '\n');
                    break;
                }

                if (quantityToAdd == 0) {
                    printf("Operation cancelled.\n");
                    break;
                }

                if (quantityToAdd <= 0) {
                    printf("Quantity must be positive.\n");
                    break;
                }

                current->quantity += quantityToAdd;
                printf("Stock updated successfully. New stock: %d\n", current->quantity);
                break;
            }
            case 4: { // Add New Book - Enhanced version with sorted insertion
                Book *newBook = (Book *)malloc(sizeof(Book));
                if (!newBook) {
                    printf("Error: Memory allocation failed.\n");
                    pauseForUser();
                    break;
                }

                // Clear input buffer
                while (getchar() != '\n');

                // Show current stock before adding
                printf("\nCurrent Stock (Sorted by ID):\n");
                showStock(bookList);
                printf("\n----------------------------------------------\n");
                printf("|            ADD NEW BOOK                     |\n");
                printf("----------------------------------------------\n");

                // Get Book ID with validation
                while (1) {
                    printf("Enter book ID (must be positive number, 0 to cancel): ");
                    if (scanf("%d", &newBook->id) != 1) {
                        printf("Invalid input. Please enter a number.\n");
                        while (getchar() != '\n');
                        continue;
                    }

                    if (newBook->id == 0) {
                        free(newBook);
                        printf("Operation cancelled.\n");
                        pauseForUser();
                        system("cls");
                        break;
                    }

                    if (newBook->id < 0) {
                        printf("Error: ID must be a positive number.\n");
                        continue;
                    }

                    // Check if ID already exists
                    int idExists = 0;
                    Book *temp = bookList;
                    while (temp) {
                        if (temp->id == newBook->id) {
                            idExists = 1;
                            break;
                        }
                        temp = temp->next;
                    }

                    if (idExists) {
                        printf("Error: Book with ID %d already exists.\n", newBook->id);
                    } else {
                        break; // Valid ID
                    }
                }
                if (newBook->id == 0) break; // User cancelled

                // Clear input buffer
                while (getchar() != '\n');

                // Get Book Title
                printf("Enter book title: ");
                fgets(newBook->title, sizeof(newBook->title), stdin);
                newBook->title[strcspn(newBook->title, "\n")] = '\0';
                if (strlen(newBook->title) == 0) {
                    printf("Error: Title cannot be empty.\n");
                    free(newBook);
                    pauseForUser();
                    system("cls");
                    continue;
                }

                // Get Book Author
                printf("Enter book author: ");
                fgets(newBook->author, sizeof(newBook->author), stdin);
                newBook->author[strcspn(newBook->author, "\n")] = '\0';
                if (strlen(newBook->author) == 0) {
                    printf("Error: Author cannot be empty.\n");
                    free(newBook);
                    pauseForUser();
                    system("cls");
                    continue;
                }

                // Get Book Category
                printf("Enter book category: ");
                fgets(newBook->category, sizeof(newBook->category), stdin);
                newBook->category[strcspn(newBook->category, "\n")] = '\0';
                if (strlen(newBook->category) == 0) {
                    printf("Error: Category cannot be empty.\n");
                    free(newBook);
                    pauseForUser();
                    system("cls");
                    continue;
                }

                // Get Quantity with validation
                while (1) {
                    printf("Enter book quantity (must be positive number): ");
                    if (scanf("%d", &newBook->quantity) != 1) {
                        printf("Invalid input. Please enter a number.\n");
                        while (getchar() != '\n');
                        continue;
                    }

                    if (newBook->quantity <= 0) {
                        printf("Error: Quantity must be a positive number.\n");
                    } else {
                        break;
                    }
                }

                // Get Price with validation
                while (1) {
                    printf("Enter book price (must be positive number): $");
                    if (scanf("%f", &newBook->price) != 1) {
                        printf("Invalid input. Please enter a number.\n");
                        while (getchar() != '\n');
                        continue;
                    }

                    if (newBook->price <= 0) {
                        printf("Error: Price must be a positive number.\n");
                    } else {
                        break;
                    }
                }

                // Insert the new book in sorted order by ID
                Book *prev = NULL;
                Book *curr = bookList;
                
                while (curr != NULL && curr->id < newBook->id) {
                    prev = curr;
                    curr = curr->next;
                }
                
                if (prev == NULL) {
                    // Insert at beginning
                    newBook->next = bookList;
                    bookList = newBook;
                } else {
                    // Insert in middle or end
                    prev->next = newBook;
                    newBook->next = curr;
                }

                printf("\nNew book added successfully!\n");
                printf("\nUpdated Stock (Sorted by ID):\n");
                showStock(bookList);

                // Save changes to file
                FILE *file = fopen("file/Book_Stock.csv", "w");
                if (!file) {
                    printf("Error: Could not save changes to file.\n");
                    freeBookList(bookList);
                    pauseForUser();
                    system("cls");
                    continue;
                }

                Book *temp = bookList;
                while (temp) {
                    fprintf(file, "%d,%s,%s,%s,%d,%.2f\n", 
                           temp->id, temp->title, temp->author, 
                           temp->category, temp->quantity, temp->price);
                    temp = temp->next;
                }
                fclose(file);

                pauseForUser();
                system("cls");
                break;
            }
        }

        // Save changes to file for all operations except Add New Book (already saved)
        if (choice != 4) {
            FILE *file = fopen("file/Book_Stock.csv", "w");
            if (!file) {
                printf("Error: Could not save changes to file.\n");
                freeBookList(bookList);
                continue;
            }

            Book *temp = bookList;
            while (temp) {
                fprintf(file, "%d,%s,%s,%s,%d,%.2f\n", 
                       temp->id, temp->title, temp->author, 
                       temp->category, temp->quantity, temp->price);
                temp = temp->next;
            }
            fclose(file);
        }

        freeBookList(bookList);

    } while (choice != 0);
}

void manageCoupons() {
    int choice;

    do {
        system("cls"); // Clear screen at start of each loop

        // Always show current coupons first
        printf("\nCurrent Coupons:\n");
        FILE *couponFile = fopen("file/Coupon.csv", "r");
        if (!couponFile) {
            printf("Error: Could not open Coupon.csv\n");
            return;
        }

        printf("---------------------------------------------------\n");
        printf("| %-20s | %-10s |\n", "Coupon Code", "Discount (%)");
        printf("---------------------------------------------------\n");

        char line[256];
        while (fgets(line, sizeof(line), couponFile)) {
            char code[50];
            float discount;
            sscanf(line, "%[^,],%f", code, &discount);
            printf("| %-20s | %-10.2f |\n", code, discount);
        }
        printf("---------------------------------------------------\n");
        fclose(couponFile);

        printf("\n----------------------------------------------");
        printf("\n|Manage Coupons Menu:                        |\n");
        printf("----------------------------------------------\n");
        printf("|1. Add Coupon                               |\n");
        printf("|2. Edit Coupon                              |\n");
        printf("|3. Delete Coupon                            |\n");
        printf("|0. Exit to Admin Menu                       |\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice (0-3): ");

        if (scanf("%d", &choice) != 1 || choice < 0 || choice > 3) {
            printf("Invalid input. Please enter a number between 0 and 3.\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) {
            system("cls");
            return;
        }

        char code[50];
        printf("Enter coupon code: ");
        scanf("%s", code);

        // For Add operation, check if coupon already exists
        if (choice == 1) {
            couponFile = fopen("file/Coupon.csv", "r");
            if (couponFile) {
                int exists = 0;
                while (fgets(line, sizeof(line), couponFile)) {
                    char existingCode[50];
                    float discount;
                    sscanf(line, "%[^,],%f", existingCode, &discount);
                    if (caseInsensitiveCompare(existingCode, code)) {
                        exists = 1;
                        break;
                    }
                }
                fclose(couponFile);
                
                if (exists) {
                    printf("Coupon with code '%s' already exists.\n", code);
                    pauseForUser();
                    continue;
                }
            }
        }

        switch (choice) {
            case 1: { // Add Coupon
                float discount;
                printf("Enter discount percentage (0-100): ");
                if (scanf("%f", &discount) != 1 || discount < 0 || discount > 100) {
                    printf("Invalid input. Discount must be between 0 and 100.\n");
                    while (getchar() != '\n');
                    break;
                }

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
                float newDiscount;
                printf("Enter new discount percentage (0-100): ");
                if (scanf("%f", &newDiscount) != 1 || newDiscount < 0 || newDiscount > 100) {
                    printf("Invalid input. Discount must be between 0 and 100.\n");
                    while (getchar() != '\n');
                    break;
                }

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

                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char existingCode[50];
                    float discount;

                    sscanf(line, "%[^,],%f", existingCode, &discount);

                    if (caseInsensitiveCompare(existingCode, code)) {
                        fprintf(tempFile, "%s,%.2f\n", code, newDiscount);
                        found = 1;
                    } else {
                        fprintf(tempFile, "%s", line);
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Coupon.csv");
                    rename("file/Coupon_temp.csv", "file/Coupon.csv");
                    printf("Coupon updated successfully.\n");
                    
                    // Show updated table
                    printf("\nUpdated Coupons:\n");
                    couponFile = fopen("file/Coupon.csv", "r");
                    printf("---------------------------------------------------\n");
                    printf("| %-20s | %-10s |\n", "Coupon Code", "Discount (%)");
                    printf("---------------------------------------------------\n");
                    while (fgets(line, sizeof(line), couponFile)) {
                        char code[50];
                        float discount;
                        sscanf(line, "%[^,],%f", code, &discount);
                        printf("| %-20s | %-10.2f |\n", code, discount);
                    }
                    printf("---------------------------------------------------\n");
                    fclose(couponFile);
                } else {
                    remove("file/Coupon_temp.csv");
                    printf("Coupon with code '%s' not found.\n", code);
                }
                break;
            }
            case 3: { // Delete Coupon
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

                int found = 0;
                while (fgets(line, sizeof(line), file)) {
                    char existingCode[50];
                    float discount;

                    sscanf(line, "%[^,],%f", existingCode, &discount);

                    if (!caseInsensitiveCompare(existingCode, code)) {
                        fprintf(tempFile, "%s", line);
                    } else {
                        found = 1;
                    }
                }

                fclose(file);
                fclose(tempFile);

                if (found) {
                    remove("file/Coupon.csv");
                    rename("file/Coupon_temp.csv", "file/Coupon.csv");
                    printf("Coupon deleted successfully.\n");
                    
                    // Show updated table
                    printf("\nUpdated Coupons:\n");
                    couponFile = fopen("file/Coupon.csv", "r");
                    printf("---------------------------------------------------\n");
                    printf("| %-20s | %-10s |\n", "Coupon Code", "Discount (%)");
                    printf("---------------------------------------------------\n");
                    while (fgets(line, sizeof(line), couponFile)) {
                        char code[50];
                        float discount;
                        sscanf(line, "%[^,],%f", code, &discount);
                        printf("| %-20s | %-10.2f |\n", code, discount);
                    }
                    printf("---------------------------------------------------\n");
                    fclose(couponFile);
                } else {
                    remove("file/Coupon_temp.csv");
                    printf("Coupon with code '%s' not found.\n", code);
                }
                break;
            }
        }

        pauseForUser();
    } while (choice != 0);
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
            system("cls");
            printf("Access Granted. Welcome, %s!\n", username);

            // Call the admin management menu
            manageAdminMenu();
            return;
        } else {
            attempts--;
            system("cls");
            printf("Invalid credentials. You have %d attempt(s) remaining.\n", attempts);
        }
    }
    system("cls");
    printf("Access Denied. Returning to the main menu.\n");
}

// Function to manage admin menu
void manageAdminMenu() {
    int choice;
    do {
        printf("----------------------------------------------");
        printf("\n|Admin Management Menu:                      |\n");
        printf("----------------------------------------------\n");
        printf("|1. Add Admin                                |\n");
        printf("|2. Edit Admin                               |\n");
        printf("|3. Delete Admin                             |\n");
        printf("|4. Exit to Main Menu                        |\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        system("cls"); 

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
                    system("cls"); 
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
                    system("cls"); 
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
                system("cls"); 
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
                    system("cls"); 
                    printf("Admin password updated successfully.\n");
                } else {
                    remove("file/Admin_Password_temp.csv");
                    system("cls"); 
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
                    system("cls"); 
                    printf("Admin with username '%s' deleted successfully.\n", deleteUsername);
                } else {
                    remove("file/Admin_Password_temp.csv");
                    system("cls"); 
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

void showListBorrowBook() {
    FILE *file = fopen("file/List_borrow_book.csv", "r");
    if (!file) {
        printf("Error: Could not open List_borrow_book.csv\n");
        return;
    }

    printf("\nList of Borrowed Books:\n");
    printf("-------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s |\n", "Username", "Book Title", "Quantity");
    printf("-------------------------------------------------------------\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100];
        int quantity;

        // Parse the line to extract borrow details
        sscanf(line, "%[^,],%[^,],%d", username, bookTitle, &quantity);

        // Display the borrow details
        printf("| %-20s | %-30s | %-10d |\n", username, bookTitle, quantity);
    }

    printf("-------------------------------------------------------------\n");

    fclose(file);
}

void showListReturnBook() {
    FILE *file = fopen("file/list_return.csv", "r");
    if (!file) {
        printf("Error: Could not open list_return.csv\n");
        return;
    }

    printf("\nList of Returned Books:\n");
    printf("-------------------------------------------------------------\n");
    printf("| %-20s | %-30s | %-10s |\n", "Username", "Book Title", "Quantity");
    printf("-------------------------------------------------------------\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char username[50], bookTitle[100];
        int quantity;

        // Parse the line to extract return details
        sscanf(line, "%[^,],%[^,],%d", username, bookTitle, &quantity);

        // Display the return details
        printf("| %-20s | %-30s | %-10d |\n", username, bookTitle, quantity);
    }

    printf("-------------------------------------------------------------\n");

    fclose(file);
}

void manageBorrowedBooks() {
    int choice;

    do {
        system("cls"); // Clear screen at start

        // Load all borrowed books into memory
        FILE *file = fopen("file/List_borrow_book.csv", "r");
        if (!file) {
            printf("Error: Could not open List_borrow_book.csv\n");
            pauseForUser();
            return;
        }

        // Count records and store them
        char records[100][256]; // Assuming max 100 records
        int recordCount = 0;
        while (fgets(records[recordCount], sizeof(records[0]), file) && recordCount < 100) {
            // Skip empty or malformed lines
            if (strlen(records[recordCount]) > 10) { // Minimum valid line length
                recordCount++;
            }
        }
        fclose(file);

        // Filter and display valid records
        char validRecords[100][256];
        char usernames[100][50];
        char bookTitles[100][100];
        int quantities[100];
        int validCount = 0;

        printf("\nCurrent List of Borrowed Books:\n");
        printf("-------------------------------------------------------------\n");
        printf("| #  | %-20s | %-30s | %-10s |\n", "Username", "Book Title", "Quantity");
        printf("-------------------------------------------------------------\n");

        for (int i = 0; i < recordCount; i++) {
            char username[50] = "";
            char bookTitle[100] = "";
            int quantity = 0;
            
            if (sscanf(records[i], "%49[^,],%99[^,],%d", username, bookTitle, &quantity) == 3) {
                if (strlen(username) > 0 && strlen(bookTitle) > 0 && quantity > 0) {
                    strcpy(validRecords[validCount], records[i]);
                    strcpy(usernames[validCount], username);
                    strcpy(bookTitles[validCount], bookTitle);
                    quantities[validCount] = quantity;
                    
                    printf("| %-2d | %-20s | %-30s | %-10d |\n", 
                          validCount+1, username, bookTitle, quantity);
                    validCount++;
                }
            }
        }

        if (validCount == 0) {
            printf("| No valid borrowed books found in records.                |\n");
        }
        printf("-------------------------------------------------------------\n");

        printf("\n----------------------------------------------");
        printf("\n|          Manage Borrowed Books Menu        |");
        printf("\n----------------------------------------------\n");
        printf("|1. Edit Borrowed Book Record                |\n");
        printf("|2. Delete Borrowed Book Record              |\n");
        printf("|0. Exit to Admin Menu                       |\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice (0-2): ");

        if (scanf("%d", &choice) != 1 || choice < 0 || choice > 2) {
            printf("Invalid input. Please enter 0, 1, or 2.\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) {
            system("cls");
            return;
        }

        if (validCount == 0) {
            printf("No valid borrowed books to manage.\n");
            pauseForUser();
            continue;
        }

        int recordNum;
        printf("Enter the record number to %s (1-%d, 0 to cancel): ", 
              (choice == 1) ? "edit" : "delete", validCount);
        if (scanf("%d", &recordNum) != 1 || recordNum < 0 || recordNum > validCount) {
            printf("Invalid record number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (recordNum == 0) {
            system("cls");
            continue;
        }

        // Adjust to 0-based index
        recordNum--;

        if (choice == 1) { // Edit
            int editChoice;
            do {
                system("cls");
                printf("\nEditing Record #%d:\n", recordNum+1);
                printf("1. Username: %s\n", usernames[recordNum]);
                printf("2. Book Title: %s\n", bookTitles[recordNum]);
                printf("3. Quantity: %d\n", quantities[recordNum]);
                printf("0. Finish Editing\n");
                printf("Enter field to edit (1-3) or 0 to finish: ");

                if (scanf("%d", &editChoice) != 1 || editChoice < 0 || editChoice > 3) {
                    printf("Invalid choice.\n");
                    while (getchar() != '\n');
                    continue;
                }

                getchar(); // Clear newline
                switch (editChoice) {
                    case 1: // Edit Username
                        printf("Current username: %s\n", usernames[recordNum]);
                        printf("Enter new username: ");
                        fgets(usernames[recordNum], sizeof(usernames[recordNum]), stdin);
                        usernames[recordNum][strcspn(usernames[recordNum], "\n")] = '\0';
                        break;
                    case 2: // Edit Book Title
                        printf("Current book title: %s\n", bookTitles[recordNum]);
                        printf("Enter new book title: ");
                        fgets(bookTitles[recordNum], sizeof(bookTitles[recordNum]), stdin);
                        bookTitles[recordNum][strcspn(bookTitles[recordNum], "\n")] = '\0';
                        break;
                    case 3: // Edit Quantity
                        printf("Current quantity: %d\n", quantities[recordNum]);
                        printf("Enter new quantity (must be positive): ");
                        if (scanf("%d", &quantities[recordNum]) != 1 || quantities[recordNum] <= 0) {
                            printf("Invalid input. Quantity must be positive.\n");
                            while (getchar() != '\n');
                            quantities[recordNum] = 1; // Default to 1 if invalid
                        }
                        break;
                }

                if (editChoice != 0) {
                    // Update the record in memory
                    sprintf(validRecords[recordNum], "%s,%s,%d", 
                           usernames[recordNum], bookTitles[recordNum], quantities[recordNum]);
                    printf("Field updated successfully.\n");
                    pauseForUser();
                    system("cls");
                }
            } while (editChoice != 0);
        }
        else if (choice == 2) { // Delete
            printf("Are you sure you want to delete record #%d? (1=Yes, 0=No): ", recordNum+1);
            int confirm;
            if (scanf("%d", &confirm) != 1) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
                continue;
            }

            if (confirm == 1) {
                // Shift all records after the deleted one
                for (int i = recordNum; i < validCount-1; i++) {
                    strcpy(validRecords[i], validRecords[i+1]);
                    strcpy(usernames[i], usernames[i+1]);
                    strcpy(bookTitles[i], bookTitles[i+1]);
                    quantities[i] = quantities[i+1];
                }
                validCount--;
                printf("Record deleted successfully.\n");
            } else {
                printf("Deletion cancelled.\n");
            }
            pauseForUser();
        }

        // Save changes back to file
        file = fopen("file/List_borrow_book.csv", "w");
        if (!file) {
            printf("Error: Could not save changes.\n");
            pauseForUser();
            return;
        }

        for (int i = 0; i < validCount; i++) {
            fprintf(file, "%s\n", validRecords[i]);
        }
        fclose(file);

    } while (choice != 0);
}

int main() {
    int choice;

    // Load books from the file into a linked list
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");

    // Load orders from the file into a queue
    OrderQueue *orderQueue = createQueue();
    loadOrdersFromFile("file/Orderlist.csv", orderQueue);

    do {
        printf("\n----------------------------------------------");
        printf("\n|                 Admin Menu                 |");
        printf("\n----------------------------------------------");
        printf("\n|  1. Show Books                             |");
        printf("\n|  2. Show Orders                            |");
        printf("\n|  3. Show History Orders                    |");
        printf("\n|  4. Show Log Stock Book                    |");
        printf("\n|  5. Show List Borrowed Books               |"); 
        printf("\n|  6. Show List Returned Books               |"); 
        printf("\n|  7. Search Book                            |");
        printf("\n|  8. Edit Stock Book                        |");
        printf("\n|  9. Manage Coupons                         |");
        printf("\n| 10. Manage Admins                          |");
        printf("\n| 11. Generate Report                        |");
        printf("\n| 12. Manage Borrowed Books                  |");
        printf("\n| 13. Exit                                   |");
        printf("\n----------------------------------------------");
        printf("\nEnter your choice: ");

        // Validate input
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number between 1 and 13.\n");
            while (getchar() != '\n'); // Clear invalid input from the buffer
            continue;
        }

        system("cls"); 

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
                showListBorrowBook();
                break;
            case 6:
                showListReturnBook();
                break;
            case 7:
                system("searchadmin.exe");
                exit(0);
                break;
            case 8:
                editStockBook();
                break;
            case 9:
                manageCoupons();
                break;
            case 10:
                manageAdmin(); 
                break;
            case 11:
                generateReport();
                break;
            case 12:
                manageBorrowedBooks();
                break;
            case 13:
                printf("Exiting...\n");
                system("login.exe"); 
                exit(0);
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 13.\n");
        }
    } while (choice != 13);

  
    freeBookList(bookList);
    freeOrderQueue(orderQueue);

    return 0;
}