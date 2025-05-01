#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void cls() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

typedef struct TreeNode {
    int id;
    char title[100];
    char author[100];
    char category[50];
    int quantity;
    float price;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct PriceSearchResult {
    TreeNode *book;
    struct PriceSearchResult *next;
} PriceSearchResult;

TreeNode* createTreeNode(int id, char *title, char *author, char *category, int quantity, float price) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->id = id;
    strcpy(newNode->title, title);
    strcpy(newNode->author, author);
    strcpy(newNode->category, category);
    newNode->quantity = quantity;
    newNode->price = price;
    newNode->left = newNode->right = NULL;
    return newNode;
}

TreeNode* insertTreeNode(TreeNode *root, int id, char *title, char *author, char *category, int quantity, float price) {
    if (root == NULL) {
        return createTreeNode(id, title, author, category, quantity, price);
    }

    if (id < root->id) {
        root->left = insertTreeNode(root->left, id, title, author, category, quantity, price);
    } else {
        root->right = insertTreeNode(root->right, id, title, author, category, quantity, price);
    }

    return root;
}

void printAllBooks(TreeNode *root) {
    if (root == NULL) {
        return;
    }

    printAllBooks(root->left);

    printf("| %-5d | %-30s | %-20s | %-15s | %-6d | $%-8.2f |\n",
           root->id, root->title, root->author, root->category, root->quantity, root->price);

    printAllBooks(root->right);
}

int startsWithCaseInsensitive(const char *str, const char *prefix) {
    while (*prefix) {
        if (tolower((unsigned char)*str) != tolower((unsigned char)*prefix)) {
            return 0;
        }
        str++;
        prefix++;
    }
    return 1;
}

void searchTree(TreeNode *root, char *key, int searchType) {
    if (root == NULL) {
        return;
    }

    searchTree(root->left, key, searchType);

    if ((searchType == 1 && startsWithCaseInsensitive(root->category, key)) || 
        (searchType == 2 && startsWithCaseInsensitive(root->title, key)) ||
        (searchType == 3 && startsWithCaseInsensitive(root->author, key))) {
        printf("| %-5d | %-30s | %-20s | %-15s | %-6d | $%-8.2f |\n",
               root->id, root->title, root->author, root->category, root->quantity, root->price);
    }

    searchTree(root->right, key, searchType);
}

void searchByPriceRange(TreeNode *root, float minPrice, float maxPrice, PriceSearchResult **results) {
    if (root == NULL) {
        return;
    }

    searchByPriceRange(root->left, minPrice, maxPrice, results);

    if (root->price >= minPrice && root->price <= maxPrice) {
        PriceSearchResult *newResult = (PriceSearchResult *)malloc(sizeof(PriceSearchResult));
        newResult->book = root;
        newResult->next = *results;
        *results = newResult;
    }

    searchByPriceRange(root->right, minPrice, maxPrice, results);
}

void printPriceResults(PriceSearchResult *results) {
    printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");
    printf("| %-5s | %-30s | %-20s | %-15s | %-6s | %-9s |\n", "ID", "Title", "Author", "Category", "Stock", "Price");
    printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");

    PriceSearchResult *current = results;
    while (current != NULL) {
        TreeNode *book = current->book;
        printf("| %-5d | %-30s | %-20s | %-15s | %-6d | $%-8.2f |\n",
               book->id, book->title, book->author, book->category, book->quantity, book->price);
        current = current->next;
    }

    printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");
}

void freePriceResults(PriceSearchResult *results) {
    while (results != NULL) {
        PriceSearchResult *temp = results;
        results = results->next;
        free(temp);
    }
}

PriceSearchResult* sortResults(PriceSearchResult *results, int ascending) {
    if (results == NULL || results->next == NULL) {
        return results;
    }

    PriceSearchResult *sorted = NULL;
    PriceSearchResult *current = results;

    while (current != NULL) {
        PriceSearchResult *next = current->next;
        
        if (sorted == NULL || 
            (ascending && current->book->price < sorted->book->price) ||
            (!ascending && current->book->price > sorted->book->price)) {
            current->next = sorted;
            sorted = current;
        } else {
            PriceSearchResult *temp = sorted;
            while (temp->next != NULL && 
                   ((ascending && temp->next->book->price <= current->book->price) ||
                    (!ascending && temp->next->book->price >= current->book->price))) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        
        current = next;
    }

    return sorted;
}

void priceRangeSearch(TreeNode *root) {
    cls();
    printf("Enter minimum price: ");
    float minPrice;
    if (scanf("%f", &minPrice) != 1) {
        printf("Invalid input for minimum price.\n");
        printf("Press Enter to continue...");
        getchar(); getchar();
        return;
    }

    if (minPrice<0) {
        printf("Minimum price cannot be negative number.\n");
        printf("Press Enter to continue...");
        getchar(); getchar();
        return;
    }
    getchar(); // Consume newline
    
    printf("Enter maximum price: ");
    float maxPrice;
    if (scanf("%f", &maxPrice) != 1) {
        printf("Invalid input for maximum price.\n");
        printf("Press Enter to continue...");
        getchar(); getchar();
        return;
    }
    if (maxPrice<0) {
        printf("Maximum price cannot be negative number.\n");
        printf("Press Enter to continue...");
        getchar(); getchar();
        return;
    }
    getchar(); // Consume newline

    if (minPrice > maxPrice) {
        printf("Minimum price cannot be greater than maximum price.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    PriceSearchResult *results = NULL;
    searchByPriceRange(root, minPrice, maxPrice, &results);
    results = sortResults(results, 1); // Sort ascending by default

    if (results == NULL) {
        printf("No books found in price range $%.2f to $%.2f\n", minPrice, maxPrice);
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    int choice;
    do {
        cls();
        printf("Books in price range $%.2f to $%.2f (sorted by price ascending):\n", minPrice, maxPrice);
        printPriceResults(results);

        printf("\nOptions:\n");
        printf("1. Sort by price (descending)\n");
        printf("2. Sort by price (ascending)\n");
        printf("3. Return to search menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            results = sortResults(results, 0); // Descending
        } else if (choice == 2) {
            results = sortResults(results, 1); // Ascending
        }
    } while (choice != 3);

    freePriceResults(results);
}

TreeNode* loadBooksIntoTree(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening %s\n", filename);
        return NULL;
    }

    TreeNode *root = NULL;
    char line[256];
    fgets(line, sizeof(line), file); // skip header
    while (fgets(line, sizeof(line), file)) {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        root = insertTreeNode(root, id, title, author, category, quantity, price);
    }

    fclose(file);
    return root;
}

void freeTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void searchBooks() {
    TreeNode *root = loadBooksIntoTree("file/Book_Stock.csv");
    if (root == NULL) {
        printf("Failed to load book data.\n");
        return;
    }

    int choice;
    do {
        cls();
        printf("+---------------------------------------+\n");
        printf("|          BOOK SEARCH MENU             |\n");
        printf("+---------------------------------------+\n");
        printf("| 1. Search by Category                 |\n");
        printf("| 2. Search by Title                    |\n");
        printf("| 3. Search by Author                   |\n");
        printf("| 4. Search by Price Range              |\n");
        printf("| 5. Return to Main Menu                |\n");
        printf("+---------------------------------------+\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
            case 2:
            case 3: {
                cls();
                printf("CURRENT STOCK:\n");
                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");
                printf("| %-5s | %-30s | %-20s | %-15s | %-6s | %-9s |\n", "ID", "Title", "Author", "Category", "Stock", "Price");
                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");
                printAllBooks(root);
                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");

                const char *searchTypes[] = {"category", "title", "author"};
                printf("Enter the %s to search (beginning with): ", searchTypes[choice-1]);

                char key[100];
                fgets(key, sizeof(key), stdin);
                key[strcspn(key, "\n")] = '\0';

                cls();
                printf("SEARCH RESULT (%s starting with '%s' - case insensitive):\n", searchTypes[choice-1], key);
                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");
                printf("| %-5s | %-30s | %-20s | %-15s | %-6s | %-9s |\n", "ID", "Title", "Author", "Category", "Stock", "Price");
                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");

                searchTree(root, key, choice);

                printf("+-------+--------------------------------+----------------------+-----------------+--------+-----------+\n");

                printf("\nPress Enter to continue...");
                getchar();
                break;
            }
            case 4:
                priceRangeSearch(root);
                break;
            case 5:
                printf("Returning to the Main Menu...\n");
                break;
            default:
                printf("Invalid choice.\n");
                printf("Press Enter to continue...");
                getchar();
        }
    } while (choice != 5);

    freeTree(root);
}

int main() {
    while (1) {
        searchBooks();
        system("main.exe");
        exit(0);
    }
    return 0;
}