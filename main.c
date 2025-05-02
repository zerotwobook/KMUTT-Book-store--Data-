#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void cls()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseForUser()
{
    printf("\n[Press Enter to continue]");
    getchar();
    getchar();
    cls();
}

typedef struct Book
{
    int id;
    char title[100];
    char author[100];
    char category[50];
    int quantity;
    float price;
    struct Book *prev;
    struct Book *next;
} Book;

typedef struct CartItem
{
    char title[100];
    int quantity;
    struct CartItem *next;
} CartItem;

typedef struct Order
{
    char username[50];
    char bookTitle[100];
    int quantity;
    struct Order *next;
} Order;

typedef struct OrderQueue
{
    Order *front;
    Order *rear;
} OrderQueue;

typedef struct Borrow
{
    char username[50];
    char bookTitle[100];
    int quantity;
    struct Borrow *next;
} Borrow;

typedef struct BorrowQueue
{
    Borrow *front;
    Borrow *rear;
} BorrowQueue;

BorrowQueue *createBorrowQueue();
void enqueueBorrow(BorrowQueue *queue, char *username, char *bookTitle, int quantity);
Borrow *dequeueBorrow(BorrowQueue *queue);

BorrowQueue *createBorrowQueue()
{
    BorrowQueue *queue = (BorrowQueue *)malloc(sizeof(BorrowQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

Book *createBookNode(char *title, char *author, int quantity)
{
    Book *newBook = (Book *)malloc(sizeof(Book));
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->quantity = quantity;
    newBook->prev = NULL;
    newBook->next = NULL;
    return newBook;
}

Book *loadBooksFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening %s\n", filename);
        return NULL;
    }

    Book *head = NULL, *tail = NULL;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        Book *newBook = createBookNode(title, author, quantity);
        newBook->id = id;
        strcpy(newBook->category, category);
        newBook->price = price;

        if (!head)
        {
            head = newBook;
            tail = newBook;
        }
        else
        {
            tail->next = newBook;
            newBook->prev = tail;
            tail = newBook;
        }
    }
    fclose(file);
    return head;
}

Book *loadIncomingBooks(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening %s\n", filename);
        return NULL;
    }

    Book *head = NULL, *tail = NULL;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        Book *newBook = createBookNode(title, author, quantity);
        newBook->id = id;
        strcpy(newBook->category, category);
        newBook->price = price;

        if (!head)
        {
            head = newBook;
            tail = newBook;
        }
        else
        {
            tail->next = newBook;
            newBook->prev = tail;
            tail = newBook;
        }
    }
    fclose(file);
    return head;
}

void freeBookList(Book *head);
void reloadBookStock(Book **bookList);
void viewCart(CartItem *cart, Book *bookList);
void editCart(CartItem **cart, Book *bookList);
void removeFromCart(CartItem **cart, const char *title);
void updateCartItem(CartItem *cart, const char *title, int newQuantity, Book *bookList);
void displayTitlesWithNumbers(CartItem *cart);
int titlesMatch(const char *a, const char *b);

// Add these functions with other cart functions
void removeFromCart(CartItem **cart, const char *title)
{
    CartItem *current = *cart;
    CartItem *prev = NULL;

    while (current)
    {
        if (strcmp(current->title, title) == 0)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                *cart = current->next;
            }
            free(current);
            printf("Item '%s' removed from cart.\n", title);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Item '%s' not found in cart.\n", title);
}

void updateCartItem(CartItem *cart, const char *title, int newQuantity, Book *bookList)
{
    CartItem *current = cart;
    while (current)
    {
        if (strcmp(current->title, title) == 0)
        {
            // Check stock availability
            Book *book = bookList;
            while (book)
            {
                if (strcmp(book->title, title) == 0)
                {
                    if (book->quantity >= newQuantity)
                    {
                        current->quantity = newQuantity;
                        printf("Updated quantity for '%s' to %d.\n", title, newQuantity);
                    }
                    else
                    {
                        printf("Not enough stock for '%s'. Only %d available.\n", title, book->quantity);
                    }
                    return;
                }
                book = book->next;
            }
        }
        current = current->next;
    }
    printf("Item '%s' not found in cart.\n", title);
}

void editCart(CartItem **cart, Book *bookList)
{
    cls();
    if (!*cart)
    {
        printf("Your cart is empty.\n");
        pauseForUser();
        return;
    }

    int choice;
    do
    {
        cls(); // Clear screen but then immediately redisplay
        printf("\n=== YOUR CURRENT CART ===\n");
        viewCart(*cart, bookList);

        printf("\nEdit Cart Options:\n");
        printf("1. Remove item\n");
        printf("2. Change quantity\n");
        printf("3. Back to previous menu\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            // Clear input buffer if invalid input
            while (getchar() != '\n')
                ;
            printf("Invalid input. Please enter a number.\n");
            pauseForUser();
            continue;
        }
        getchar(); // Consume newline

        switch (choice)
        {
        case 1:
        {
            displayTitlesWithNumbers(*cart);
            printf("Enter the NUMBER of item to remove (or 0 to cancel): ");
            int itemNum;
            if (scanf("%d", &itemNum) != 1 || itemNum < 0)
            {
                printf("Invalid input.\n");
                while (getchar() != '\n')
                    ;
                pauseForUser();
                break;
            }
            getchar(); // Consume newline

            if (itemNum == 0)
                break;

            // Find the item
            CartItem *current = *cart;
            CartItem *prev = NULL;
            int i = 1;
            while (current && i < itemNum)
            {
                prev = current;
                current = current->next;
                i++;
            }

            if (current)
            {
                char title[100];
                strcpy(title, current->title);
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    *cart = current->next;
                }
                free(current);
                printf("Removed '%s' from cart.\n", title);
            }
            else
            {
                printf("Invalid item number.\n");
            }
            pauseForUser();
            break;
        }
        case 2:
        {
            displayTitlesWithNumbers(*cart);
            printf("Enter the NUMBER of item to update (or 0 to cancel): ");
            int itemNum;
            if (scanf("%d", &itemNum) != 1 || itemNum < 0)
            {
                printf("Invalid input.\n");
                while (getchar() != '\n')
                    ;
                pauseForUser();
                break;
            }
            getchar(); // Consume newline

            if (itemNum == 0)
                break;

            // Find the item
            CartItem *current = *cart;
            int i = 1;
            while (current && i < itemNum)
            {
                current = current->next;
                i++;
            }

            if (current)
            {
                printf("Enter new quantity for '%s' (current: %d): ",
                       current->title, current->quantity);
                int newQty;
                if (scanf("%d", &newQty) != 1 || newQty <= 0)
                {
                    printf("Invalid quantity.\n");
                    while (getchar() != '\n')
                        ;
                    pauseForUser();
                    break;
                }
                getchar(); // Consume newline

                // Check stock
                Book *book = bookList;
                while (book)
                {
                    if (titlesMatch(book->title, current->title))
                    {
                        if (book->quantity >= newQty)
                        {
                            current->quantity = newQty;
                            printf("Quantity updated.\n");
                        }
                        else
                        {
                            printf("Only %d available in stock.\n", book->quantity);
                        }
                        break;
                    }
                    book = book->next;
                }
                if (!book)
                {
                    printf("Error: Book not found in stock.\n");
                }
            }
            else
            {
                printf("Invalid item number.\n");
            }
            pauseForUser();
            break;
        }
        case 3:
            return;
        default:
            printf("Invalid choice. Please try again.\n");
            pauseForUser();
        }
    } while (1);
}

// Add these functions with other order functions
void cancelPreOrder(OrderQueue *queue, const char *username, const char *bookTitle)
{
    Order *current = queue->front;
    Order *prev = NULL;
    int found = 0;

    while (current)
    {
        if (strcmp(current->username, username) == 0 &&
            strcasecmp(current->bookTitle, bookTitle) == 0)
        {
            found = 1;

            // Remove from queue
            if (prev == NULL)
            {
                queue->front = current->next;
            }
            else
            {
                prev->next = current->next;
            }

            if (current == queue->rear)
            {
                queue->rear = prev;
            }

            // Update Orderlist.csv
            FILE *orderFile = fopen("file/Orderlist.csv", "r");
            FILE *tempFile = fopen("file/Orderlist_temp.csv", "w");
            if (orderFile && tempFile)
            {
                char line[256];
                while (fgets(line, sizeof(line), orderFile))
                {
                    char fileUsername[50], fileTitle[100], status[20];
                    int quantity;
                    sscanf(line, "%[^,],%[^,],%d,%[^,\n]", fileUsername, fileTitle, &quantity, status);

                    if (!(strcmp(fileUsername, username) == 0 &&
                          strcasecmp(fileTitle, bookTitle) == 0 &&
                          strcmp(status, "Pending") == 0))
                    {
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(orderFile);
                fclose(tempFile);

                remove("file/Orderlist.csv");
                rename("file/Orderlist_temp.csv", "file/Orderlist.csv");
            }

            // Update History_Order.csv
            FILE *historyFile = fopen("file/History_Order.csv", "a");
            if (historyFile)
            {
                fprintf(historyFile, "%s,%s,%d,Cancelled\n",
                        current->username, current->bookTitle, current->quantity);
                fclose(historyFile);
            }

            free(current);
            printf("Pre-order for '%s' has been cancelled.\n", bookTitle);
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found)
    {
        printf("Pre-order for '%s' not found.\n", bookTitle);
    }
}

void updatePreOrder(OrderQueue *queue, const char *username, const char *bookTitle,
                    int newQuantity, Book *incomingBooks)
{
    Order *current = queue->front;

    while (current)
    {
        if (strcmp(current->username, username) == 0 &&
            titlesMatch(current->bookTitle, bookTitle))
        {
            // ... rest of the function remains the same ...
        }
        current = current->next;
    }
    printf("Pre-order for '%s' not found.\n", bookTitle);
}

void managePreOrders(OrderQueue *queue, Book *incomingBooks)
{
    cls();
    char username[50];
    printf("Enter your username: ");
    if (scanf("%49s", username) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        pauseForUser();
        return;
    }
    getchar(); // Consume newline

    // Collect all user's pre-orders
    Order *userOrders = NULL;
    Order *lastOrder = NULL;
    Order *current = queue->front;
    int orderCount = 0;

    while (current)
    {
        if (strcmp(current->username, username) == 0)
        {
            Order *newOrder = (Order *)malloc(sizeof(Order));
            strcpy(newOrder->username, current->username);
            strcpy(newOrder->bookTitle, current->bookTitle);
            newOrder->quantity = current->quantity;
            newOrder->next = NULL;

            if (!userOrders)
            {
                userOrders = newOrder;
                lastOrder = newOrder;
            }
            else
            {
                lastOrder->next = newOrder;
                lastOrder = newOrder;
            }
            orderCount++;
        }
        current = current->next;
    }

    if (orderCount == 0)
    {
        printf("No pending pre-orders found for %s.\n", username);
        pauseForUser();
        return;
    }

    int choice;
    do
    {
        cls();
        printf("\n=== YOUR PRE-ORDERS ===\n");
        printf("-------------------------------------------------------------\n");
        printf("| %-2s | %-30s | %-8s |\n", "#", "Title", "Quantity");
        printf("-------------------------------------------------------------\n");

        Order *displayOrder = userOrders;
        int i = 1;
        while (displayOrder)
        {
            printf("| %-2d | %-30s | %-8d |\n", i++, displayOrder->bookTitle, displayOrder->quantity);
            displayOrder = displayOrder->next;
        }
        printf("-------------------------------------------------------------\n");

        printf("\nPre-Order Management:\n");
        printf("1. Cancel a pre-order\n");
        printf("2. Back to menu\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid input. Please enter a number.\n");
            pauseForUser();
            continue;
        }
        getchar(); // Consume newline

        switch (choice)
        {
        case 1:
        {
            printf("Enter the NUMBER of pre-order to cancel (or 0 to cancel): ");
            int orderNum;
            if (scanf("%d", &orderNum) != 1 || orderNum < 0)
            {
                printf("Invalid input.\n");
                while (getchar() != '\n')
                    ;
                pauseForUser();
                break;
            }
            getchar(); // Consume newline

            if (orderNum == 0)
                break;
            if (orderNum > orderCount)
            {
                printf("Invalid order number.\n");
                pauseForUser();
                break;
            }

            // Find the order
            Order *toCancel = userOrders;
            Order *prev = NULL;
            int i = 1;
            while (toCancel && i < orderNum)
            {
                prev = toCancel;
                toCancel = toCancel->next;
                i++;
            }

            if (toCancel)
            {
                cancelPreOrder(queue, username, toCancel->bookTitle);

                // Update incoming books quantity
                Book *book = incomingBooks;
                while (book)
                {
                    if (strcasecmp(book->title, toCancel->bookTitle) == 0)
                    {
                        book->quantity += toCancel->quantity;
                        break;
                    }
                    book = book->next;
                }

                // Remove from our local list
                if (prev)
                {
                    prev->next = toCancel->next;
                }
                else
                {
                    userOrders = toCancel->next;
                }
                free(toCancel);
                orderCount--;
            }
            pauseForUser();
            break;
        }
        case 2:
            // Free our local list
            while (userOrders)
            {
                Order *temp = userOrders;
                userOrders = userOrders->next;
                free(temp);
            }
            return;
        default:
            printf("Invalid choice. Please try again.\n");
            pauseForUser();
        }
    } while (1);
}

// Add these near other helper functions
int validateBookTitle(Book *bookList, const char *title)
{
    Book *current = bookList;
    while (current)
    {
        if (strcasecmp(current->title, title) == 0)
        {
            return 1; // Found matching book
        }
        current = current->next;
    }
    return 0; // Book not found
}

int getValidBookTitle(Book *bookList, char *title)
{
    while (1)
    {
        printf("Enter the title of the book (or '0' to return to menu): ");
        fgets(title, 100, stdin);
        title[strcspn(title, "\n")] = '\0';

        if (strcmp(title, "0") == 0)
        {
            return 0; // User wants to return to menu
        }

        if (validateBookTitle(bookList, title))
        {
            return 1; // Valid book title
        }
        printf("Book '%s' not found. Please try again or enter '0' to return to menu.\n", title);
    }
}

int getValidQuantity()
{
    int quantity;
    while (1)
    {
        printf("Enter the quantity: ");
        scanf("%d", &quantity);
        getchar(); // Consume newline

        if (quantity > 0)
        {
            return quantity;
        }
        printf("Quantity must be positive. Please try again.\n");
    }
}

int validatePositiveInt(int value)
{
    if (value <= 0)
    {
        printf("Invalid input. Please enter a positive number.\n");
        return 0;
    }
    return 1;
}

int validatePositiveFloat(float value)
{
    if (value <= 0)
    {
        printf("Invalid input. Please enter a positive number.\n");
        return 0;
    }
    return 1;
}

void reloadBookStock(Book **bookList)
{
    if (*bookList != NULL)
    {
        freeBookList(*bookList);
    }
    *bookList = loadBooksFromFile("file/Book_Stock.csv");
}

void displayBooks(Book *head)
{
    cls();
    if (!head)
    {
        printf("No books available.\n");
        return;
    }

    printf("Books in stock:\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    Book *current = head;
    while (current)
    {
        printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
}

// Add this helper function for consistent title display
void displayTitlesWithNumbers(CartItem *cart)
{
    int i = 1;
    CartItem *current = cart;
    printf("\nItems in your cart:\n");
    printf("-------------------\n");
    while (current)
    {
        printf("%d. %s (Qty: %d)\n", i++, current->title, current->quantity);
        current = current->next;
    }
    printf("-------------------\n");
}

// Case-insensitive string comparison wrapper
int titlesMatch(const char *a, const char *b)
{
    return strcasecmp(a, b) == 0;
}

void displayIncomingBooks(Book *head)
{
    cls();
    if (!head)
    {
        printf("No incoming books available.\n");
        return;
    }

    printf("Incoming Books (Available for Pre-Order):\n");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    Book *current = head;
    while (current)
    {
        printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
        current = current->next;
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
}

void freeBookList(Book *head)
{
    Book *current = head;
    while (current)
    {
        Book *temp = current;
        current = current->next;
        free(temp);
    }
}

void addToCart(CartItem **cart, char *title, int quantity, Book *bookList)
{
    title[strcspn(title, "\n")] = '\0';

    // Validate quantity input
    if (!validatePositiveInt(quantity))
    {
        pauseForUser();
        return;
    }

    Book *currentBook = bookList;
    int found = 0;
    while (currentBook)
    {
        if (strcasecmp(currentBook->title, title) == 0)
        {
            if (currentBook->quantity >= quantity)
            {
                CartItem *newItem = (CartItem *)malloc(sizeof(CartItem));
                strcpy(newItem->title, currentBook->title);
                newItem->quantity = quantity;
                newItem->next = *cart;
                *cart = newItem;

                printf("Added %d of '%s' to the cart.\n", quantity, currentBook->title);
                pauseForUser();
                found = 1;
            }
            else
            {
                printf("Not enough stock for '%s'. Only %d available.\n", currentBook->title, currentBook->quantity);
                pauseForUser();
            }
            break;
        }
        currentBook = currentBook->next;
    }

    if (!found)
    {
        printf("Book '%s' not found in stock. Please check the title and try again.\n", title);
        pauseForUser();
    }
}

void viewCart(CartItem *cart, Book *bookList)
{
    cls();
    if (!cart)
    {
        printf("Your cart is empty...\n");
        pauseForUser();
        return;
    }

    printf("Your cart:\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| %-30s | %-5s | %-10s |\n", "Title", "Qty", "Price");
    printf("-----------------------------------------------------------------------------------------\n");

    float total = 0;
    CartItem *currentCart = cart;
    while (currentCart)
    {
        Book *currentBook = bookList;
        float price = 0;

        while (currentBook)
        {
            if (strcmp(currentCart->title, currentBook->title) == 0)
            {
                price = currentBook->price;
                break;
            }
            currentBook = currentBook->next;
        }

        printf("| %-30s | %-5d | $%-9.2f |\n", currentCart->title, currentCart->quantity, price * currentCart->quantity);
        total += price * currentCart->quantity;

        currentCart = currentCart->next;
    }

    printf("-----------------------------------------------------------------------------------------\n");
    printf("Total: $%.2f\n", total);
    pauseForUser();
}

float applyCoupon(const char *filename, char *couponCode, float total)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening %s\n", filename);
        return total;
    }

    char line[256], code[50];
    float discount;
    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%[^,],%f", code, &discount);
        if (strcmp(code, couponCode) == 0)
        {
            if (!validatePositiveFloat(discount))
            {
                fclose(file);
                return total;
            }
            printf("Coupon applied! You get a %.2f%% discount.\n", discount);
            fclose(file);
            return total * (1 - discount / 100);
        }
    }
    fclose(file);
    printf("Invalid coupon code.\n");
    return total;
}

void checkout(CartItem *cart, Book *bookList)
{
    cls();
    if (!cart)
    {
        printf("Your cart is empty. Nothing to check out.\n");
        pauseForUser();
        cls();
        return;
    }

    char username[50];
    printf("Enter your username: ");
    scanf("%s", username);

    float total = 0;
    CartItem *currentCart = cart;

    // First pass: Check stock availability
    int canCheckout = 1;
    currentCart = cart;
    while (currentCart)
    {
        Book *currentBook = bookList;
        while (currentBook)
        {
            if (strcmp(currentCart->title, currentBook->title) == 0)
            {
                if (currentBook->quantity < currentCart->quantity)
                {
                    printf("Not enough stock for '%s'. Only %d available.\n",
                           currentBook->title, currentBook->quantity);
                    canCheckout = 0;
                }
                break;
            }
            currentBook = currentBook->next;
        }
        currentCart = currentCart->next;
    }

    if (!canCheckout)
    {
        printf("\nCannot proceed with checkout due to insufficient stock.\n");
        printf("Please adjust your cart quantities.\n");
        pauseForUser();
        return;
    }

    // Second pass: Display checkout summary
    cls();
    printf("Checkout Summary for %s:\n", username);
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| %-30s | %-5s | %-10s |\n", "Title", "Qty", "Price");
    printf("-----------------------------------------------------------------------------------------\n");

    currentCart = cart;
    while (currentCart)
    {
        Book *currentBook = bookList;
        float price = 0;

        while (currentBook)
        {
            if (strcmp(currentCart->title, currentBook->title) == 0)
            {
                price = currentBook->price;
                break;
            }
            currentBook = currentBook->next;
        }

        printf("| %-30s | %-5d | $%-9.2f |\n", currentCart->title, currentCart->quantity, price * currentCart->quantity);
        total += price * currentCart->quantity;

        currentCart = currentCart->next;
    }

    printf("-----------------------------------------------------------------------------------------\n");
    printf("Subtotal: $%.2f\n", total);

    char confirm[10];
    while (1)
    {
        printf("Do you want to proceed with the checkout? (yes or no): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0)
        {
            break;
        }
        else if (strcmp(confirm, "no") == 0)
        {
            printf("Checkout canceled. Returning to the previous menu...\n");
            return;
        }
        else
        {
            printf("Invalid input. Please enter 'yes' or 'no'.\n");
        }
    }

    // Third pass: Update stock and complete checkout
    FILE *logFile = fopen("file/Log_Stockbook.csv", "a");
    if (!logFile)
    {
        printf("Error opening file/Log_Stockbook.csv\n");
        return;
    }

    currentCart = cart;
    while (currentCart)
    {
        Book *currentBook = bookList;
        float price = 0;

        while (currentBook)
        {
            if (strcmp(currentCart->title, currentBook->title) == 0)
            {
                price = currentBook->price;
                currentBook->quantity -= currentCart->quantity;
                fprintf(logFile, "%s,%s,%d,%.2f\n", username, currentCart->title, currentCart->quantity, price * currentCart->quantity);
                break;
            }
            currentBook = currentBook->next;
        }
        currentCart = currentCart->next;
    }

    fclose(logFile);

    // Update Book_Stock.csv
    FILE *stockFile = fopen("file/Book_Stock.csv", "r");
    FILE *tempStockFile = fopen("file/Book_Stock_temp.csv", "w");
    if (!stockFile || !tempStockFile)
    {
        printf("Error opening Book_Stock.csv or creating temporary file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), stockFile))
    {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        Book *currentBook = bookList;
        while (currentBook)
        {
            if (strcmp(currentBook->title, title) == 0)
            {
                quantity = currentBook->quantity;
                break;
            }
            currentBook = currentBook->next;
        }

        fprintf(tempStockFile, "%d,%s,%s,%s,%d,%.2f\n", id, title, author, category, quantity, price);
    }

    fclose(stockFile);
    fclose(tempStockFile);

    remove("file/Book_Stock.csv");
    rename("file/Book_Stock_temp.csv", "file/Book_Stock.csv");

    // Coupon code handling
    char couponCode[50];
    printf("Enter a coupon code (or press Enter to skip): ");
    getchar();
    fgets(couponCode, sizeof(couponCode), stdin);
    couponCode[strcspn(couponCode, "\n")] = '\0';

    if (strlen(couponCode) > 0)
    {
        total = applyCoupon("file/Coupon.csv", couponCode, total);
    }

    cls();
    printf("\n");
    printf("=========================================\n");
    printf("          PURCHASE COMPLETE\n");
    printf("=========================================\n\n");
    printf("        Thank you for your order!\n\n");
    printf("        Final amount: $%.2f\n\n", total);
    printf("        Your receipt has been saved.\n");
    printf("=============================================\n");
    printf("    Need help? Punge@bookstore.kmutt.ac.th\n");
    printf("=============================================\n");
    reloadBookStock(&bookList);
    pauseForUser();
}

void freeCart(CartItem *cart)
{
    CartItem *current = cart;
    while (current)
    {
        CartItem *temp = current;
        current = current->next;
        free(temp);
    }
}

OrderQueue *createQueue()
{
    OrderQueue *queue = (OrderQueue *)malloc(sizeof(OrderQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(OrderQueue *queue, char *username, char *bookTitle, int quantity)
{
    Order *newOrder = (Order *)malloc(sizeof(Order));
    strcpy(newOrder->username, username);
    strcpy(newOrder->bookTitle, bookTitle);
    newOrder->quantity = quantity;
    newOrder->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newOrder;
        return;
    }

    queue->rear->next = newOrder;
    queue->rear = newOrder;
}

void processOrder(OrderQueue *queue)
{
    cls();
    char username[50];
    printf("Enter username: ");
    scanf("%s", username);

    printf("\nOrders for %s:\n", username);
    printf("-------------------------------------------------------------\n");
    printf("| %-30s | %-10s | %-10s |\n", "Book Title", "Quantity", "Status");
    printf("-------------------------------------------------------------\n");

    FILE *file = fopen("file/Orderlist.csv", "r");
    if (!file)
    {
        printf("Error opening file/Orderlist.csv\n");
        return;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char fileUsername[50], bookTitle[100], status[20];
        int quantity;
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", fileUsername, bookTitle, &quantity, status);

        if (strcmp(fileUsername, username) == 0)
        {
            printf("| %-30s | %-10d | %-10s |\n", bookTitle, quantity, status);
            found = 1;
        }
    }
    fclose(file);

    if (!found)
    {
        printf("No orders found for %s.\n", username);
        return;
    }

    printf("-------------------------------------------------------------\n");

    char confirm[10];
    printf("Do you want to process these orders? (yes or no): ");
    scanf("%s", confirm);

    if (strcmp(confirm, "yes") != 0)
    {
        printf("Orders not processed.\n");
        return;
    }

    FILE *tempFile = fopen("file/Orderlist_temp.csv", "w");
    if (!tempFile)
    {
        printf("Error creating temporary file.\n");
        return;
    }

    file = fopen("file/Orderlist.csv", "r");
    while (fgets(line, sizeof(line), file))
    {
        char fileUsername[50], bookTitle[100], status[20];
        int quantity;
        sscanf(line, "%[^,],%[^,],%d,%[^,\n]", fileUsername, bookTitle, &quantity, status);

        if (strcmp(fileUsername, username) == 0 && strcmp(status, "Pending") == 0)
        {
            fprintf(tempFile, "%s,%s,%d,Processed\n", fileUsername, bookTitle, quantity);
            printf("Order processed: %s ordered %d of '%s'\n", fileUsername, quantity, bookTitle);
        }
        else
        {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("file/Orderlist.csv");
    rename("file/Orderlist_temp.csv", "file/Orderlist.csv");

    Order *current = queue->front;
    Order *prev = NULL;

    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (prev == NULL)
            {
                queue->front = current->next;
            }
            else
            {
                prev->next = current->next;
            }

            if (current == queue->rear)
            {
                queue->rear = prev;
            }

            Order *temp = current;
            current = current->next;
            free(temp);
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    cls();
    printf("All orders for %s have been processed successfully!\n", username);
    pauseForUser();
}

void preOrderBook(OrderQueue *queue, Book *incomingBooks)
{
    cls();
    if (!incomingBooks)
    {
        printf("No incoming books available for pre-order.\n");
        pauseForUser();
        return;
    }

    displayIncomingBooks(incomingBooks);

    char bookTitle[100];
    int quantity;

    printf("\nEnter the title of the book to pre-order (or '0' to return to menu): ");
    getchar();
    fgets(bookTitle, sizeof(bookTitle), stdin);
    bookTitle[strcspn(bookTitle, "\n")] = '\0';

    if (bookTitle[0] == '0')
    {
        printf("Returning to menu...\n");
        pauseForUser();
        return;
    }

    // Validate quantity input
    while (1)
    {
        printf("Enter the quantity: ");
        if (scanf("%d", &quantity) != 1)
        {
            printf("Invalid input. Please enter a positive number.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }
        getchar(); // Consume newline

        if (quantity <= 0)
        {
            printf("Quantity must be positive. Please try again.\n");
        }
        else
        {
            break;
        }
    }

    Book *current = incomingBooks;
    int found = 0;

    while (current)
    {
        if (strcasecmp(current->title, bookTitle) == 0)
        {
            if (current->quantity >= quantity)
            {
                found = 1;
                break;
            }
            else
            {
                printf("Not enough available for pre-order. Only %d coming in stock.\n", current->quantity);
                pauseForUser();
                return;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printf("Book '%s' not found in incoming books.\n", bookTitle);
        pauseForUser();
        return;
    }

    // Get username after validating everything
    char username[50];
    printf("Enter your name for the pre-order: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    enqueue(queue, username, bookTitle, quantity);

    // Update the incoming books quantity
    current = incomingBooks;
    while (current)
    {
        if (strcasecmp(current->title, bookTitle) == 0)
        {
            current->quantity -= quantity;
            break;
        }
        current = current->next;
    }

    // Write to Orderlist.csv
    FILE *orderFile = fopen("file/Orderlist.csv", "a");
    if (!orderFile)
    {
        printf("Error opening file/Orderlist.csv\n");
        return;
    }
    fprintf(orderFile, "%s,%s,%d,Pending\n", username, bookTitle, quantity);
    fclose(orderFile);

    // Write to History_Order.csv
    current = incomingBooks;
    char author[100];
    while (current)
    {
        if (strcasecmp(current->title, bookTitle) == 0)
        {
            strcpy(author, current->author);
            break;
        }
        current = current->next;
    }

    FILE *historyFile = fopen("file/History_Order.csv", "a");
    if (!historyFile)
    {
        printf("Error opening file/History_Order.csv\n");
        return;
    }
    fprintf(historyFile, "%s,%s,%d,Submitted\n", author, bookTitle, quantity);
    fclose(historyFile);

    // Update Incoming_Books.csv
    FILE *incomingFile = fopen("file/Incoming_Books.csv", "r");
    FILE *tempFile = fopen("file/Incoming_Books_temp.csv", "w");
    if (!incomingFile || !tempFile)
    {
        printf("Error updating Incoming_Books.csv\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), incomingFile))
    {
        int id, stockQuantity;
        char fileTitle[100], fileAuthor[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, fileTitle, fileAuthor, category, &stockQuantity, &price);

        if (strcasecmp(fileTitle, bookTitle) == 0)
        {
            stockQuantity -= quantity;
        }

        fprintf(tempFile, "%d,%s,%s,%s,%d,%.2f\n", id, fileTitle, fileAuthor, category, stockQuantity, price);
    }

    fclose(incomingFile);
    fclose(tempFile);

    remove("file/Incoming_Books.csv");
    rename("file/Incoming_Books_temp.csv", "file/Incoming_Books.csv");

    cls();
    printf("Pre-Order successfully placed!\n");
    printf("-------------------------\n");
    printf("Name: %s\n", username);
    printf("Book Title: %s\n", bookTitle);
    printf("Quantity: %d\n", quantity);
    printf("\nStatus: Submitted (will be processed by staff)\n");
    pauseForUser();
}

void enqueueBorrow(BorrowQueue *queue, char *username, char *bookTitle, int quantity)
{
    Borrow *newBorrow = (Borrow *)malloc(sizeof(Borrow));
    strcpy(newBorrow->username, username);
    strcpy(newBorrow->bookTitle, bookTitle);
    newBorrow->quantity = quantity;
    newBorrow->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newBorrow;
        return;
    }

    queue->rear->next = newBorrow;
    queue->rear = newBorrow;
}

Borrow *dequeueBorrow(BorrowQueue *queue)
{
    if (queue->front == NULL)
    {
        return NULL;
    }

    Borrow *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    return temp;
}

void displayBooksWithCartOptions(Book *head, CartItem **cart, Book *bookList)
{
    cls();
    if (!head)
    {
        printf("No books available.\n");
        return;
    }

    int choice;
    do
    {
        printf("Books in stock:\n");
        printf("-------------------------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-30s | %-20s | %-15s | %-5s | %-10s |\n", "ID", "Title", "Author", "Category", "Qty", "Price");
        printf("-------------------------------------------------------------------------------------------------------------\n");
        Book *current = head;
        while (current)
        {
            printf("| %-5d | %-30s | %-20s | %-15s | %-5d | $%-9.2f |\n", current->id, current->title, current->author, current->category, current->quantity, current->price);
            current = current->next;
        }
        printf("-------------------------------------------------------------------------------------------------------------\n");

        printf("\nOptions:\n");
        printf("1. Add to Cart\n");
        printf("2. View Cart\n");
        printf("3. Edit Cart\n");
        printf("4. Checkout\n");
        printf("5. Exit to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            char title[100];
            int quantity;
            printf("Enter the title of the book: ");
            getchar();
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = '\0';
            printf("Enter the quantity: ");
            scanf("%d", &quantity);
            addToCart(cart, title, quantity, bookList);
            break;
        }
        case 2:
            viewCart(*cart, bookList);
            break;
        case 3:
            editCart(cart, bookList);
            break;
        case 4:
            checkout(*cart, bookList);
            freeCart(*cart);
            *cart = NULL;
            head = bookList;
            break;
        case 5:
            printf("Returning to main menu...\n");
            break;
        default:
            printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}

void borrowBook(BorrowQueue *borrowQueue, Book **bookListPtr)
{
    cls();
    if (!*bookListPtr)
    {
        printf("No books available to borrow.\n");
        pauseForUser();
        return;
    }

    char username[50];
    printf("Enter your username: ");
    scanf("%s", username);
    getchar(); // Consume newline

    printf("\nAvailable Books:\n");
    displayBooks(*bookListPtr);

    char title[100];
    if (!getValidBookTitle(*bookListPtr, title))
    {
        printf("Returning to main menu...\n");
        pauseForUser();
        return;
    }

    printf("Enter the quantity (or '0' to cancel): ");
    int quantity;
    scanf("%d", &quantity);
    getchar(); // Consume newline

    if (quantity == 0)
    {
        printf("Borrow canceled. Returning to menu...\n");
        pauseForUser();
        return;
    }

    if (quantity <= 0)
    {
        printf("Quantity must be positive. Operation canceled.\n");
        pauseForUser();
        return;
    }

    Book *current = *bookListPtr;
    int found = 0;

    while (current)
    {
        if (strcasecmp(current->title, title) == 0)
        {
            if (current->quantity >= quantity)
            {
                current->quantity -= quantity;
                enqueueBorrow(borrowQueue, username, current->title, quantity);

                FILE *file = fopen("file/List_borrow_book.csv", "a");
                if (file)
                {
                    fprintf(file, "%s,%s,%d\n", username, current->title, quantity);
                    fclose(file);
                }

                printf("You have successfully borrowed %d of '%s'.\n", quantity, current->title);

                // Update Book_Stock.csv
                FILE *stockFile = fopen("file/Book_Stock.csv", "r");
                FILE *tempStockFile = fopen("file/Book_Stock_temp.csv", "w");
                if (stockFile && tempStockFile)
                {
                    char line[256];
                    while (fgets(line, sizeof(line), stockFile))
                    {
                        int id, stockQuantity;
                        char stockTitle[100], author[100], category[50];
                        float price;
                        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f",
                               &id, stockTitle, author, category, &stockQuantity, &price);

                        if (strcasecmp(stockTitle, title) == 0)
                        {
                            stockQuantity = current->quantity;
                        }
                        fprintf(tempStockFile, "%d,%s,%s,%s,%d,%.2f\n",
                                id, stockTitle, author, category, stockQuantity, price);
                    }
                    fclose(stockFile);
                    fclose(tempStockFile);

                    remove("file/Book_Stock.csv");
                    rename("file/Book_Stock_temp.csv", "file/Book_Stock.csv");
                }

                // Reload book list to reflect changes
                reloadBookStock(bookListPtr);
                found = 1;
                break;
            }
            else
            {
                printf("Not enough stock for '%s'. Only %d available.\n", current->title, current->quantity);
                found = 1;
                break;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printf("Error processing your borrow request.\n");
    }

    pauseForUser();
}

void returnBook(BorrowQueue *borrowQueue, Book **bookListPtr)
{
    cls();
    if (!borrowQueue->front)
    {
        printf("No borrowed books to return.\n");
        pauseForUser();
        return;
    }

    char username[50];
    printf("Enter your username: ");
    scanf("%s", username);
    getchar(); // Consume newline

    // Display borrowed books
    Borrow *current = borrowQueue->front;
    int found = 0;

    printf("\nBooks borrowed by %s:\n", username);
    printf("-------------------------------------------------------------\n");
    printf("| %-30s | %-10s |\n", "Book Title", "Quantity");
    printf("-------------------------------------------------------------\n");

    while (current)
    {
        if (strcmp(current->username, username) == 0)
        {
            printf("| %-30s | %-10d |\n", current->bookTitle, current->quantity);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("No books borrowed by %s.\n", username);
        pauseForUser();
        return;
    }

    printf("-------------------------------------------------------------\n");

    char title[100];
    if (!getValidBookTitle(*bookListPtr, title))
    {
        printf("Returning to main menu...\n");
        pauseForUser();
        return;
    }

    current = borrowQueue->front;
    Borrow *prev = NULL;

    while (current)
    {
        if (strcmp(current->username, username) == 0 &&
            strcasecmp(current->bookTitle, title) == 0)
        {

            // Update book quantity
            Book *book = *bookListPtr;
            while (book)
            {
                if (strcasecmp(book->title, title) == 0)
                {
                    book->quantity += current->quantity;
                    break;
                }
                book = book->next;
            }

            // Update Book_Stock.csv
            FILE *stockFile = fopen("file/Book_Stock.csv", "r");
            FILE *tempStockFile = fopen("file/Book_Stock_temp.csv", "w");
            if (stockFile && tempStockFile)
            {
                char line[256];
                while (fgets(line, sizeof(line), stockFile))
                {
                    int id, stockQuantity;
                    char stockTitle[100], author[100], category[50];
                    float price;
                    sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f",
                           &id, stockTitle, author, category, &stockQuantity, &price);

                    if (strcasecmp(stockTitle, title) == 0)
                    {
                        stockQuantity = book->quantity;
                    }
                    fprintf(tempStockFile, "%d,%s,%s,%s,%d,%.2f\n",
                            id, stockTitle, author, category, stockQuantity, price);
                }
                fclose(stockFile);
                fclose(tempStockFile);

                remove("file/Book_Stock.csv");
                rename("file/Book_Stock_temp.csv", "file/Book_Stock.csv");
            }

            // Update return log
            FILE *returnFile = fopen("file/list_return.csv", "a");
            if (returnFile)
            {
                fprintf(returnFile, "%s,%s,%d\n", username, current->bookTitle, current->quantity);
                fclose(returnFile);
            }

            // Update borrowed books list
            FILE *borrowFile = fopen("file/List_borrow_book.csv", "r");
            FILE *tempBorrowFile = fopen("file/List_borrow_book_temp.csv", "w");
            if (borrowFile && tempBorrowFile)
            {
                char line[256];
                while (fgets(line, sizeof(line), borrowFile))
                {
                    char fileUsername[50], fileTitle[100];
                    int fileQuantity;
                    sscanf(line, "%[^,],%[^,],%d", fileUsername, fileTitle, &fileQuantity);

                    if (!(strcmp(fileUsername, username) == 0 &&
                          strcasecmp(fileTitle, title) == 0 &&
                          fileQuantity == current->quantity))
                    {
                        fprintf(tempBorrowFile, "%s", line);
                    }
                }
                fclose(borrowFile);
                fclose(tempBorrowFile);

                remove("file/List_borrow_book.csv");
                rename("file/List_borrow_book_temp.csv", "file/List_borrow_book.csv");
            }

            // Remove from queue
            if (prev == NULL)
            {
                borrowQueue->front = current->next;
            }
            else
            {
                prev->next = current->next;
            }

            if (current == borrowQueue->rear)
            {
                borrowQueue->rear = prev;
            }

            free(current);

            // Reload book list to reflect changes
            reloadBookStock(bookListPtr);

            printf("Book '%s' returned successfully.\n", title);
            pauseForUser();
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Book '%s' not found in your borrowed list.\n", title);
    pauseForUser();
}

int main()
{
    Book *bookList = loadBooksFromFile("file/Book_Stock.csv");
    Book *incomingBooks = loadIncomingBooks("file/Incoming_Books.csv");
    CartItem *cart = NULL;
    OrderQueue *orderQueue = createQueue();
    BorrowQueue *borrowQueue = createBorrowQueue();

    int choice;
    do
    {
        cls();
        printf("\n----------------------------------------------");
        printf("\n|      WELCOME TO THE KMUTT BOOK STORE       |");
        printf("\n----------------------------------------------\n");
        printf("| 1. View Books                              |\n");
        printf("| 2. Search Books                            |\n");
        printf("| 3. Pre-Order Book                          |\n");
        printf("| 4. Manage Pre-Orders                       |\n");
        printf("| 5. Borrow Book                             |\n");
        printf("| 6. Return Book                             |\n");
        printf("| 7. Process Order                           |\n");
        printf("| 8. Exit                                    |\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            displayBooksWithCartOptions(bookList, &cart, bookList);
            break;
        case 2:
            cls();
            system("search.exe");
            exit(0);
            break;
        case 3:
            preOrderBook(orderQueue, incomingBooks);
            break;
        case 4:
            managePreOrders(orderQueue, incomingBooks);
            break;
        case 5:
            borrowBook(borrowQueue, &bookList);
            break;
        case 6:
            returnBook(borrowQueue, &bookList);
            break;
        case 7:
            processOrder(orderQueue);
            break;
        case 8:
            freeBookList(bookList);
            freeBookList(incomingBooks);
            freeCart(cart);
            cls();
            system("login.exe");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
            pauseForUser();
        }
    } while (choice != 8);

    freeBookList(bookList);
    freeBookList(incomingBooks);
    freeCart(cart);
    return 0;
}