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

// Function to initialize the queue
OrderQueue* createQueue() {
    OrderQueue *queue = (OrderQueue *)malloc(sizeof(OrderQueue));
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

    printf("Order for '%s' by '%s' has been confirmed and updated in Orderlist.csv.\n", current->bookTitle, current->username);
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

void editStockBook() {
    int choice;
    printf("\nEdit Stock Book Menu:\n");
    printf("1. Edit Book Details\n");
    printf("2. Delete Book\n");
    printf("3. Add Stock\n");
    printf("4. Exit to Admin Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    FILE *file = fopen("file/Book_Stock.csv", "r");
    if (!file) {
        printf("Error: Could not open Book_Stock.csv\n");
        return;
    }

    // Load books into a linked list
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");
    fclose(file);

    switch (choice) {
        case 1: { // Edit Book Details
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
            int id, quantityToAdd;
            printf("Enter the ID of the book to add stock: ");
            scanf("%d", &id);

            Book *current = bookList;
            while (current) {
                if (current->id == id) {
                    printf("Current stock for '%s': %d\n", current->title, current->quantity);
                    printf("Enter quantity to add: ");
                    scanf("%d", &quantityToAdd);
                    current->quantity += quantityToAdd;
                    printf("Stock updated successfully. New stock: %d\n", current->quantity);
                    break;
                }
                current = current->next;
            }

            if (!current) {
                printf("Book with ID %d not found.\n", id);
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
    FILE *tempFile = fopen("file/Book_Stock.csv", "w");
    if (!tempFile) {
        printf("Error: Could not open Book_Stock.csv for writing.\n");
        freeBookList(bookList);
        return;
    }

    Book *current = bookList;
    while (current) {
        fprintf(tempFile, "%d,%s,%s,%s,%d,%.2f\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }

    fclose(tempFile);
    freeBookList(bookList);

    printf("Changes saved successfully.\n");
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
        printf("7. Exit\n");
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
                editStockBook(); // Call the editStockBook function
                break;
            case 7:
                printf("Exiting...\n");
                system("login.exe"); // Call the login program
                exit(0); // Exit the current program
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    // Free the linked list and the queue
    freeBookList(bookList);
    freeOrderQueue(orderQueue);

    return 0;
}