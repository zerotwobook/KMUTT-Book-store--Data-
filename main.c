#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a doubly linked list node
typedef struct Book {
    int id; // Book ID
    char title[100];
    char author[100];
    char category[50]; // Add category field
    int quantity;
    float price; // Add price field
    struct Book *prev;
    struct Book *next;
} Book;

// Define the structure for a cart item
typedef struct CartItem {
    char title[100];
    int quantity;
    struct CartItem *next;
} CartItem;

// Define the structure for a queue node
typedef struct Order {
    char username[50];
    char bookTitle[100];
    int quantity;
    struct Order *next;
} Order;

// Define the queue
typedef struct OrderQueue {
    Order *front;
    Order *rear;
} OrderQueue;

// Function to create a new book node
Book* createBookNode(char *title, char *author, int quantity) {
    Book *newBook = (Book *)malloc(sizeof(Book));
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->quantity = quantity;
    newBook->prev = NULL;
    newBook->next = NULL;
    return newBook;
}

// Function to load books from Book_Stock.csv into a doubly linked list
Book* loadBooksFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening %s\n", filename);
        return NULL;
    }

    Book *head = NULL, *tail = NULL;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        Book *newBook = createBookNode(title, author, quantity);
        newBook->id = id;
        strcpy(newBook->category, category);
        newBook->price = price; // Set the price

        if (!head) {
            head = newBook;
            tail = newBook;
        } else {
            tail->next = newBook;
            newBook->prev = tail;
            tail = newBook;
        }
    }
    fclose(file);
    return head;
}

// Function to display the books in the doubly linked list
void displayBooks(Book *head) {
    if (!head) {
        printf("No books available.\n");
        return;
    }

    printf("Books in stock:\n");
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

// Function to free the memory allocated for the doubly linked list
void freeBookList(Book *head) {
    Book *current = head;
    while (current) {
        Book *temp = current;
        current = current->next;
        free(temp);
    }
}

// Function to add a book to the cart
void addToCart(CartItem **cart, char *title, int quantity, Book *bookList) {
    // Trim trailing newline character from the title
    title[strcspn(title, "\n")] = '\0';

    // Check if the book exists in the stock
    Book *currentBook = bookList;
    int found = 0;
    while (currentBook) {
        if (strcasecmp(currentBook->title, title) == 0) { // Case-insensitive comparison
            if (currentBook->quantity >= quantity) {
                // Add the book to the cart
                CartItem *newItem = (CartItem *)malloc(sizeof(CartItem));
                strcpy(newItem->title, title);
                newItem->quantity = quantity;
                newItem->next = *cart;
                *cart = newItem;

                printf("Added %d of '%s' to the cart.\n", quantity, title);
                found = 1;
            } else {
                printf("Not enough stock for '%s'. Only %d available.\n", title, currentBook->quantity);
            }
            break;
        }
        currentBook = currentBook->next;
    }

    if (!found) {
        printf("Book '%s' not found in stock.\n", title);
    }
}

// Function to view the cart
void viewCart(CartItem *cart, Book *bookList) {
    if (!cart) {
        printf("Your cart is empty.\n");
        return;
    }

    printf("Your cart:\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| %-30s | %-5s | %-10s |\n", "Title", "Qty", "Price");
    printf("-----------------------------------------------------------------------------------------\n");

    float total = 0;
    CartItem *currentCart = cart;
    while (currentCart) {
        Book *currentBook = bookList;
        float price = 0;

        // Find the book in the book list to get its price
        while (currentBook) {
            if (strcmp(currentCart->title, currentBook->title) == 0) {
                price = currentBook->price;
                break;
            }
            currentBook = currentBook->next;
        }

        // Display the cart item with its price
        printf("| %-30s | %-5d | $%-9.2f |\n", currentCart->title, currentCart->quantity, price * currentCart->quantity);
        total += price * currentCart->quantity;

        currentCart = currentCart->next;
    }

    printf("-----------------------------------------------------------------------------------------\n");
    printf("Total: $%.2f\n", total);
}

// Function to apply a coupon
float applyCoupon(const char *filename, char *couponCode, float total) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening %s\n", filename);
        return total;
    }

    char line[256], code[50];
    float discount;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%f", code, &discount);
        if (strcmp(code, couponCode) == 0) {
            printf("Coupon applied! You get a %.2f%% discount.\n", discount);
            fclose(file);
            return total * (1 - discount / 100);
        }
    }
    fclose(file);
    printf("Invalid coupon code.\n");
    return total;
}

// Function to check out
void checkout(CartItem *cart, Book *bookList) {
    if (!cart) {
        printf("Your cart is empty. Nothing to check out.\n");
        return;
    }

    char username[50];
    printf("Enter your username: ");
    scanf("%s", username);

    float total = 0;
    CartItem *currentCart = cart;

    printf("Checkout Summary:\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| %-30s | %-5s | %-10s |\n", "Title", "Qty", "Price");
    printf("-----------------------------------------------------------------------------------------\n");

    // Calculate the total and display the cart items
    while (currentCart) {
        Book *currentBook = bookList;
        float price = 0;

        // Find the book in the book list to get its price
        while (currentBook) {
            if (strcmp(currentCart->title, currentBook->title) == 0) {
                if (currentBook->quantity >= currentCart->quantity) {
                    price = currentBook->price;
                } else {
                    printf("Not enough stock for '%s'. Only %d available.\n", currentCart->title, currentBook->quantity);
                    price = currentBook->price;
                    currentCart->quantity = currentBook->quantity; // Adjust quantity to available stock
                }
                break;
            }
            currentBook = currentBook->next;
        }

        // Display the cart item with its price
        printf("| %-30s | %-5d | $%-9.2f |\n", currentCart->title, currentCart->quantity, price * currentCart->quantity);
        total += price * currentCart->quantity;

        currentCart = currentCart->next;
    }

    printf("-----------------------------------------------------------------------------------------\n");
    printf("Subtotal: $%.2f\n", total);

    // Ask for confirmation before proceeding with checkout
    char confirm[10];
    while (1) {
        printf("Do you want to proceed with the checkout? (yes or no): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0) {
            break; // Proceed with checkout
        } else if (strcmp(confirm, "no") == 0) {
            printf("Checkout canceled. Returning to the previous menu...\n");
            return; // Exit the checkout function without recording any data
        } else {
            printf("Invalid input. Please enter 'yes' or 'no'.\n");
        }
    }

    // Open Log_Stockbook.csv to log the checkout details
    FILE *logFile = fopen("file/Log_Stockbook.csv", "a");
    if (!logFile) {
        printf("Error opening file/Log_Stockbook.csv\n");
        return;
    }

    // Deduct stock and log the checkout details
    currentCart = cart;
    while (currentCart) {
        Book *currentBook = bookList;
        float price = 0;

        // Find the book in the book list to get its price and deduct stock
        while (currentBook) {
            if (strcmp(currentCart->title, currentBook->title) == 0) {
                if (currentBook->quantity >= currentCart->quantity) {
                    price = currentBook->price;
                    currentBook->quantity -= currentCart->quantity; // Deduct stock
                } else {
                    price = currentBook->price;
                    currentCart->quantity = currentBook->quantity; // Adjust quantity to available stock
                    currentBook->quantity = 0;
                }

                // Log the checkout details to Log_Stockbook.csv
                fprintf(logFile, "%s,%s,%d,%.2f\n", username, currentCart->title, currentCart->quantity, price * currentCart->quantity);
                break;
            }
            currentBook = currentBook->next;
        }

        currentCart = currentCart->next;
    }

    fclose(logFile);

    // Update Book_Stock.csv with the new stock quantities
    FILE *stockFile = fopen("file/Book_Stock.csv", "r");
    FILE *tempStockFile = fopen("file/Book_Stock_temp.csv", "w");
    if (!stockFile || !tempStockFile) {
        printf("Error opening Book_Stock.csv or creating temporary file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), stockFile)) {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        // Find the book in the book list to get the updated quantity
        Book *currentBook = bookList;
        while (currentBook) {
            if (strcmp(currentBook->title, title) == 0) {
                quantity = currentBook->quantity; // Update the quantity
                break;
            }
            currentBook = currentBook->next;
        }

        // Write the updated stock to the temporary file
        fprintf(tempStockFile, "%d,%s,%s,%s,%d,%.2f\n", id, title, author, category, quantity, price);
    }

    fclose(stockFile);
    fclose(tempStockFile);

    // Replace the original Book_Stock.csv with the updated file
    remove("file/Book_Stock.csv");
    rename("file/Book_Stock_temp.csv", "file/Book_Stock.csv");

    // Apply coupon if available
    char couponCode[50];
    printf("Enter a coupon code (or press Enter to skip): ");
    getchar(); // Clear newline from buffer
    fgets(couponCode, sizeof(couponCode), stdin);
    couponCode[strcspn(couponCode, "\n")] = '\0'; // Remove newline

    if (strlen(couponCode) > 0) {
        total = applyCoupon("file/Coupon.csv", couponCode, total);
    }

    printf("Final total after discounts: $%.2f\n", total);
    printf("Thank you for your purchase! Your order has been logged.\n");
}

// Function to free the memory allocated for the cart
void freeCart(CartItem *cart) {
    CartItem *current = cart;
    while (current) {
        CartItem *temp = current;
        current = current->next;
        free(temp);
    }
}

// Initialize the queue
OrderQueue* createQueue() {
    OrderQueue *queue = (OrderQueue *)malloc(sizeof(OrderQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Enqueue an order
void enqueue(OrderQueue *queue, char *username, char *bookTitle, int quantity) {
    Order *newOrder = (Order *)malloc(sizeof(Order));
    strcpy(newOrder->username, username);
    strcpy(newOrder->bookTitle, bookTitle);
    newOrder->quantity = quantity;
    newOrder->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newOrder;
        return;
    }

    queue->rear->next = newOrder;
    queue->rear = newOrder;
}

// Dequeue an order and save it to Orderlist.csv
void processOrder(OrderQueue *queue) {
    char username[50];
    printf("Enter the username to process orders for: ");
    scanf("%s", username);

    // Display all orders for the entered username
    printf("\nOrders for %s:\n", username);
    printf("-------------------------------------------------------------\n");
    printf("| %-30s | %-10s | %-10s |\n", "Book Title", "Quantity", "Status");
    printf("-------------------------------------------------------------\n");

    FILE *file = fopen("file/Orderlist.csv", "r");
    if (!file) {
        printf("Error opening file/Orderlist.csv\n");
        return;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char fileUsername[50], bookTitle[100], status[20];
        int quantity;
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", fileUsername, bookTitle, &quantity, status);

        if (strcmp(fileUsername, username) == 0) {
            printf("| %-30s | %-10d | %-10s |\n", bookTitle, quantity, status);
            found = 1;
        }
    }
    fclose(file);

    if (!found) {
        printf("No orders found for %s.\n", username);
        return;
    }

    printf("-------------------------------------------------------------\n");

    // Ask if the user wants to process the orders
    char confirm[10];
    printf("Do you want to process these orders? (yes or no): ");
    scanf("%s", confirm);

    if (strcmp(confirm, "yes") != 0) {
        printf("Orders not processed.\n");
        return;
    }

    // Temporary file to store updated orders
    FILE *tempFile = fopen("file/Orderlist_temp.csv", "w");
    if (!tempFile) {
        printf("Error creating temporary file.\n");
        return;
    }

    file = fopen("file/Orderlist.csv", "r");
    while (fgets(line, sizeof(line), file)) {
        char fileUsername[50], bookTitle[100], status[20];
        int quantity;
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", fileUsername, bookTitle, &quantity, status);

        if (strcmp(fileUsername, username) == 0 && strcmp(status, "Pending") == 0) {
            fprintf(tempFile, "%s,%s,%d,Processed\n", fileUsername, bookTitle, quantity);
            printf("Order processed: %s ordered %d of '%s'\n", fileUsername, quantity, bookTitle);
        } else {
            fprintf(tempFile, "%s", line); // Copy the line as is
        }
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated file
    remove("file/Orderlist.csv");
    rename("file/Orderlist_temp.csv", "file/Orderlist.csv");

    // Dequeue the processed orders
    Order *current = queue->front;
    Order *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }

            if (current == queue->rear) {
                queue->rear = prev;
            }

            Order *temp = current;
            current = current->next;
            free(temp);
        } else {
            prev = current;
            current = current->next;
        }
    }

    if (queue->front == NULL) {
        queue->rear = NULL;
    }
}

// Function to order a book
void orderBook(OrderQueue *queue, char *username) {
    char bookTitle[100];
    int quantity;

    printf("Enter the title of the book to order: ");
    getchar(); // Clear newline from buffer
    fgets(bookTitle, sizeof(bookTitle), stdin);
    bookTitle[strcspn(bookTitle, "\n")] = '\0'; // Remove newline

    printf("Enter the quantity: ");
    scanf("%d", &quantity);

    // Enqueue the order
    enqueue(queue, username, bookTitle, quantity);

    // Append the order details to Orderlist.csv
    FILE *file = fopen("file/Orderlist.csv", "a");
    if (!file) {
        printf("Error opening file/Orderlist.csv\n");
        return;
    }
    fprintf(file, "%s,%s,%d,Pending\n", username, bookTitle, quantity); // Add "Pending" as the initial order status
    fclose(file);

    printf("Order added to the queue and saved to Orderlist.csv: %s ordered %d of '%s'\n", username, quantity, bookTitle);
}

void displayBooksWithCartOptions(Book *head, CartItem **cart, Book *bookList) {
    if (!head) {
        printf("No books available.\n");
        return;
    }

    int choice;
    do {
        printf("Books in stock:\n");
        printf("-------------------------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
        printf("-------------------------------------------------------------------------------------------------------------\n");
        Book *current = head;
        while (current) {
            printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
            current = current->next;
        }
        printf("-------------------------------------------------------------------------------------------------------------\n");

        printf("\nOptions:\n");
        printf("1. Add to Cart\n");
        printf("2. View Cart\n");
        printf("3. Checkout\n");
        printf("4. Exit to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char title[100];
                int quantity;
                printf("Enter the title of the book: ");
                getchar(); // Clear newline from buffer
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0'; // Remove newline
                printf("Enter the quantity: ");
                scanf("%d", &quantity);
                addToCart(cart, title, quantity, bookList); // Pass bookList here
                break;
            }
            case 2:
                viewCart(*cart, bookList);
                break;
            case 3:
                checkout(*cart, bookList);
                freeCart(*cart);
                *cart = NULL; // Reset cart after checkout
                break;
            case 4:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

int main() {
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");
    CartItem *cart = NULL;
    OrderQueue *orderQueue = createQueue(); // Initialize the order queue

    int choice;
    do {
        printf("\nWelcome to KMUTT Book Store!\n");
        printf("\n1. View Books\n2. Search Books\n3. Order Book\n4. Process Order\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayBooksWithCartOptions(bookList, &cart, bookList);
                break;
            case 2:
                // Call the search program
                system("search.exe"); // Assuming search.c is compiled into search.exe
                exit(0); // Exit the current program
                break;
            case 3: {
                char username[50];
                printf("Enter your username: ");
                scanf("%s", username);
                orderBook(orderQueue, username);
                break;
            }
            case 4:
                processOrder(orderQueue);
                break;
            case 5:
                freeBookList(bookList);
                freeCart(cart);
                system("login.exe"); // Call the login program
                return 0; // Exit the current main function after returning to login
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);

    freeBookList(bookList);
    freeCart(cart);

    return 0;
}