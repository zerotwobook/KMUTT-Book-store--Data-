#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the structure for a tree node
typedef struct TreeNode {
    char key[100]; // Key for searching (can be category or title)
    int id;
    char title[100];
    char author[100];
    char category[50];
    int quantity;
    float price;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Function to create a new tree node
TreeNode* createTreeNode(char *key, int id, char *title, char *author, char *category, int quantity, float price) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    strcpy(newNode->key, key);
    newNode->id = id;
    strcpy(newNode->title, title);
    strcpy(newNode->author, author);
    strcpy(newNode->category, category);
    newNode->quantity = quantity;
    newNode->price = price;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Function to insert a node into the tree
TreeNode* insertTreeNode(TreeNode *root, char *key, int id, char *title, char *author, char *category, int quantity, float price) {
    if (root == NULL) {
        return createTreeNode(key, id, title, author, category, quantity, price);
    }

    if (strcmp(key, root->key) < 0) {
        root->left = insertTreeNode(root->left, key, id, title, author, category, quantity, price);
    } else {
        root->right = insertTreeNode(root->right, key, id, title, author, category, quantity, price);
    }

    return root;
}

// Function to search for a node in the tree
void searchTree(TreeNode *root, char *key, int searchByCategory) {
    if (root == NULL) {
        return;
    }

    // Traverse the left subtree
    searchTree(root->left, key, searchByCategory);

    // Check if the current node matches the search key
    if ((searchByCategory && strcmp(root->category, key) == 0) || (!searchByCategory && strcmp(root->key, key) == 0)) {
        printf("Book Found:\n");
        printf("ID: %d\nTitle: %s\nAuthor: %s\nCategory: %s\nQuantity: %d\nPrice: $%.2f\n\n",
               root->id, root->title, root->author, root->category, root->quantity, root->price);
    }

    // Traverse the right subtree
    searchTree(root->right, key, searchByCategory);
}

// Function to load books from Book_Stock.csv into the tree
TreeNode* loadBooksIntoTree(const char *filename, int searchByCategory) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening %s\n", filename);
        return NULL;
    }

    TreeNode *root = NULL;
    char line[256];
    fgets(line, sizeof(line), file); // Skip header line
    while (fgets(line, sizeof(line), file)) {
        int id, quantity;
        char title[100], author[100], category[50];
        float price;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%d,%f", &id, title, author, category, &quantity, &price);

        // Use category or title as the key based on the user's choice
        char *key = searchByCategory ? category : title;
        root = insertTreeNode(root, key, id, title, author, category, quantity, price);
    }

    fclose(file);
    return root;
}

// Function to free the tree
void freeTree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Function to search books by category or title
void searchBooks() {
    int choice;
    printf("Search Books by:\n");
    printf("1. Category\n");
    printf("2. Title\n");
    printf("3. Exit to Main Menu\n"); // Add an option to exit
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 3) {
        printf("Returning to the Main Menu...\n");
        system("main.exe"); // Assuming main.c is compiled into main.exe
        exit(0); // Exit the current program
    }

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return;
    }

    // Load books into the tree based on the user's choice
    TreeNode *root = loadBooksIntoTree("file/Book_Stock.csv", choice == 1);

    char key[100];
    printf("Enter the %s to search (or type 'exit' to return to the Main Menu): ", choice == 1 ? "category" : "title");
    getchar(); // Clear newline from buffer
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0'; // Remove newline

    // Check if the user wants to exit
    if (strcmp(key, "exit") == 0) {
        printf("Returning to the Main Menu...\n");
        system("main.exe"); // Assuming main.c is compiled into main.exe
        exit(0); // Exit the current program
    }

    // Search for the book(s)
    printf("\nSearch Results:\n");
    searchTree(root, key, choice == 1);

    // Free the tree after searching
    freeTree(root);
}

int main() {
    printf("Welcome to the Book Store!\n");

    char userInput[10]; // Buffer to store user input
    int exitFlag = 0;   // Flag to control the loop

    while (!exitFlag) {
        searchBooks(); // Call the searchBooks function

        printf("\nDo you want to search again? (yes or no): ");
        scanf("%s", userInput); // Read user input

        // Convert input to lowercase for case-insensitive comparison
        for (int i = 0; userInput[i]; i++) {
            userInput[i] = tolower(userInput[i]);
        }

        // Check if the user entered "no"
        if (strcmp(userInput, "no") == 0) {
            exitFlag = 1; // Exit the loop
        }
    }

    printf("Returning to the Main Menu...\n");
    system("main.exe"); // Assuming main.c is compiled into main.exe
    return 0;
}