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

    printf("| %-5d | %-30s | %-15s | %-6d | $%-8.2f |\n",
           root->id, root->title, root->category, root->quantity, root->price);

    printAllBooks(root->right);
}

void searchTree(TreeNode *root, char *key, int searchByCategory) {
    if (root == NULL) {
        return;
    }

    searchTree(root->left, key, searchByCategory);

    if ((searchByCategory && strcmp(root->category, key) == 0) || 
        (!searchByCategory && strstr(root->title, key) != NULL)) {
        printf("| %-5d | %-30s | %-15s | %-6d | $%-8.2f |\n",
               root->id, root->title, root->category, root->quantity, root->price);
    }

    searchTree(root->right, key, searchByCategory);
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
    cls();
    printf("+---------------------------------------+\n");
    printf("|          BOOK SEARCH MENU             |\n");
    printf("+---------------------------------------+\n");
    printf("| 1. Search by Category                 |\n");
    printf("| 2. Search by Title                    |\n");
    printf("| 3. Return to Main Menu                |\n");
    printf("+---------------------------------------+\n");
    printf("Enter your choice: ");

    int choice;
    scanf("%d", &choice);
    getchar();

    if (choice == 3) {
        printf("Returning to the Main Menu...\n");
        system("main.exe");
        exit(0);
    }

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    TreeNode *root = loadBooksIntoTree("file/Book_Stock.csv");

    cls();
    printf("CURRENT STOCK:\n");
    printf("+=======+================================+=================+========+==========+\n");
    printf("| %-5s | %-30s | %-15s | %-6s | %-8s |\n", "ID", "Title", "Category", "Stock", "Price");
    printf("+=======+================================+=================+========+==========+\n");

    printAllBooks(root); // SHOW ALL BOOKS BY ID

    printf("+=======+================================+=================+========+==========+\n");

    printf("Enter the %s to search: ", choice == 1 ? "category" : "title");

    char key[100];
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    cls();
    printf("SEARCH RESULT:\n");
    printf("+=======+================================+=================+========+==========+\n");
    printf("| %-5s | %-30s | %-15s | %-6s | %-8s |\n", "ID", "Title", "Category", "Stock", "Price");
    printf("+=======+================================+=================+========+==========+\n");

    searchTree(root, key, choice == 1);

    printf("+=======+================================+=================+========+==========+\n");

    freeTree(root);

    printf("\nPress Enter to continue...");
    getchar();
}

int main() {
    while (1) {
        searchBooks();
    }
    return 0;
}
