#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a doubly linked list node
typedef struct Book {
    char title[100];
    char author[100];
    int quantity;
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
        char title[100], author[100];
        int quantity;
        sscanf(line, "%[^,],%[^,],%d", title, author, &quantity);

        Book *newBook = createBookNode(title, author, quantity);
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
    printf("-------------------------------------------------\n");
    printf("| %-30s | %-20s | %-5s |\n", "Title", "Author", "Qty");
    printf("-------------------------------------------------\n");
    Book *current = head;
    while (current) {
        printf("| %-30s | %-20s | %-5d |\n", current->title, current->author, current->quantity);
        current = current->next;
    }
    printf("-------------------------------------------------\n");
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
void addToCart(CartItem **cart, char *title, int quantity) {
    CartItem *newItem = (CartItem *)malloc(sizeof(CartItem));
    strcpy(newItem->title, title);
    newItem->quantity = quantity;
    newItem->next = *cart;
    *cart = newItem;
    printf("Added %d of '%s' to the cart.\n", quantity, title);
}

// Function to view the cart
void viewCart(CartItem *cart) {
    if (!cart) {
        printf("Your cart is empty.\n");
        return;
    }

    printf("Your cart:\n");
    printf("---------------------------------\n");
    printf("| %-30s | %-5s |\n", "Title", "Qty");
    printf("---------------------------------\n");
    CartItem *current = cart;
    while (current) {
        printf("| %-30s | %-5d |\n", current->title, current->quantity);
        current = current->next;
    }
    printf("---------------------------------\n");
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

    float total = 0;
    CartItem *currentCart = cart;
    while (currentCart) {
        Book *currentBook = bookList;
        while (currentBook) {
            if (strcmp(currentCart->title, currentBook->title) == 0) {
                if (currentBook->quantity >= currentCart->quantity) {
                    total += currentCart->quantity * 10; // Assume each book costs $10
                    currentBook->quantity -= currentCart->quantity;
                } else {
                    printf("Not enough stock for '%s'.\n", currentCart->title);
                }
                break;
            }
            currentBook = currentBook->next;
        }
        currentCart = currentCart->next;
    }

    printf("Your total is: $%.2f\n", total);

    char couponCode[50];
    printf("Enter a coupon code (or press Enter to skip): ");
    getchar(); // Clear newline from buffer
    fgets(couponCode, sizeof(couponCode), stdin);
    couponCode[strcspn(couponCode, "\n")] = '\0'; // Remove newline

    if (strlen(couponCode) > 0) {
        total = applyCoupon("file/Coupon.csv", couponCode, total);
    }

    printf("Final total after discounts: $%.2f\n", total);
    printf("Thank you for your purchase!\n");
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
    if (queue->front == NULL) {
        printf("No orders to process.\n");
        return;
    }

    char username[50];
    printf("Enter the username to process orders for: ");
    scanf("%s", username);

    FILE *file = fopen("file/Orderlist.csv", "a");
    if (!file) {
        printf("Error opening file/Orderlist.csv\n");
        return;
    }

    Order *current = queue->front;
    Order *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            fprintf(file, "%s,%s,%d\n", current->username, current->bookTitle, current->quantity);
            printf("Order processed: %s ordered %d of '%s'\n", current->username, current->quantity, current->bookTitle);

            // Remove the current order from the queue
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

    fclose(file);

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

    enqueue(queue, username, bookTitle, quantity);
    printf("Order added to the queue: %s ordered %d of '%s'\n", username, quantity, bookTitle);
}

void displayBooksWithCartOptions(Book *head, CartItem **cart, Book *bookList) {
    if (!head) {
        printf("No books available.\n");
        return;
    }

    int choice;
    do {
        printf("Books in stock:\n");
        printf("-------------------------------------------------\n");
        printf("| %-30s | %-20s | %-5s |\n", "Title", "Author", "Qty");
        printf("-------------------------------------------------\n");
        Book *current = head;
        while (current) {
            printf("| %-30s | %-20s | %-5d |\n", current->title, current->author, current->quantity);
            current = current->next;
        }
        printf("-------------------------------------------------\n");
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
                addToCart(cart, title, quantity);
                break;
            }
            case 2:
                viewCart(*cart);
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
        printf("\n1. View Books\n2. Order Book\n3. Process Order\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayBooksWithCartOptions(bookList, &cart, bookList);
                break;
            case 2: {
                char username[50];
                printf("Enter your username: ");
                scanf("%s", username);
                orderBook(orderQueue, username);
                break;
            }
            case 3:
                processOrder(orderQueue);
                break;
            case 4:
                freeBookList(bookList);
                freeCart(cart);
                system("login.exe"); // Call the login program
                return 0; // Exit the current main function after returning to login
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    freeBookList(bookList);
    freeCart(cart);

    return 0;
}