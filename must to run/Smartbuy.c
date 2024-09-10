#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#define NUM_ITEMS 5
#define FILENAME "products.txt"
#define ADMIN_PASSWORD "1222"

// Global variables
int inStock[NUM_ITEMS];
char foodName[NUM_ITEMS][50];
double price[NUM_ITEMS];

// Function prototypes
void loadProducts();
void saveProducts();
void printFoodTable();
void displayMenu();
void displayAdminMenu();
void printReceipt(int numItems, int f[], int q[], double total);
void logTransaction(int numItems, int f[], int q[], double total);
void viewTransactions();
void editProduct();

int main() {
    loadProducts();
    int choice;
    
    while (1) {
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Error: invalid input.\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }

        switch (choice) {
            case 1: {
                printf("You have selected Order Products.\n");
                printFoodTable();
                printf("Enter the number of different items you want to order: ");
                int numItems;
                if (scanf("%d", &numItems) != 1 || numItems <= 0) {
                    fprintf(stderr, "Error: invalid number of items.\n");
                    while (getchar() != '\n'); // Clear the input buffer
                    continue;
                }

                int *f = (int*)malloc(numItems * sizeof(int));
                int *q = (int*)malloc(numItems * sizeof(int));
                if (f == NULL || q == NULL) {
                    fprintf(stderr, "Error: memory allocation failed.\n");
                    return 1;
                }

                double total = 0.0;

                for (int i = 0; i < numItems; ++i) {
                    printf("Enter the item no. for item %d: ", i + 1);
                    if (scanf("%d", &f[i]) != 1 || f[i] < 1 || f[i] > NUM_ITEMS) {
                        fprintf(stderr, "Error: invalid item number.\n");
                        while (getchar() != '\n'); // Clear the input buffer
                        --i;
                        continue;
                    }
                    printf("Enter the quantity for item %d: ", i + 1);
                    if (scanf("%d", &q[i]) != 1 || q[i] < 1) {
                        fprintf(stderr, "Error: invalid quantity.\n");
                        while (getchar() != '\n'); // Clear the input buffer
                        --i;
                        continue;
                    }
                    if (q[i] > inStock[f[i] - 1]) {
                        fprintf(stderr, "Error: insufficient stock for %s.\n", foodName[f[i] - 1]);
                        --i;
                        continue;
                    }
                    total += price[f[i] - 1] * q[i];
                    inStock[f[i] - 1] -= q[i];
                    printf("You have ordered %d %s. Total cost so far: %.2lf\n", q[i], foodName[f[i] - 1], total);
                }
                printReceipt(numItems, f, q, total);
                logTransaction(numItems, f, q, total);
                saveProducts(); // Save changes to the products file

                free(f);
                free(q);
                break;
            }
            case 2:
                displayAdminMenu();
                break;
            case 3:
                printf("Thanks for visiting.\n");
                return 0;
            default:
                fprintf(stderr, "Error: invalid choice.\n");
                break;
        }
    }

    return 0;
}

// Function to load products from the file
void loadProducts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open %s.\n", FILENAME);
        exit(1);
    }

    for (int i = 0; i < NUM_ITEMS; ++i) {
        fscanf(file, "%[^,],%lf,%d\n", foodName[i], &price[i], &inStock[i]);
    }

    fclose(file);
}

// Function to save products to the file
void saveProducts() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open %s.\n", FILENAME);
        exit(1);
    }

    for (int i = 0; i < NUM_ITEMS; ++i) {
        fprintf(file, "%s,%.2lf,%d\n", foodName[i], price[i], inStock[i]);
    }

    fclose(file);
}

// Function to print the food table
void printFoodTable() {
    printf("______________________________________________________\n");
    printf("|  Item No.  |  Item Name   |  Price    |  In Stock   |\n");
    printf("------------------------------------------------------\n");

    for (int i = 0; i < NUM_ITEMS; ++i) {
        printf("|    %2d      | %-12s | %6.2lf    |     %3d     |\n", i + 1, foodName[i], price[i], inStock[i]);
        if (i != NUM_ITEMS - 1) {
            printf("+----------------------------------------------------+\n");
        }
    }
    printf("+-----------------------------------------------------+\n");
}

// Function to display the main menu
void displayMenu() {
    printf("\nWelcome to our shop. How may I help you?\n");
    printf("1. Order Products\n");
    printf("2. Admin Panel\n");
    printf("3. Exit\n");
}

// Function to display the admin menu
void displayAdminMenu() {
    char password[50];
    printf("\nEnter admin password: ");
    scanf("%s", password);
    if (strcmp(password, ADMIN_PASSWORD) == 0) {
        int choice;
        printf("\nAdmin Panel:\n");
        printf("1. View Transactions\n");
        printf("2. Edit Product\n");
        printf("3. Return to Main Menu\n");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Error: invalid input.\n");
            while (getchar() != '\n'); // Clear the input buffer
            return;
        }

        switch (choice) {
            case 1:
                viewTransactions();
                break;
            case 2:
                editProduct();
                saveProducts(); // Save changes to the products file
                break;
            case 3:
                return;
            default:
                fprintf(stderr, "Error: invalid choice.\n");
                break;
        }
    } else {
        printf("Error: incorrect password.\n");
    }
}

void printReceipt(int numItems, int f[], int q[], double total) {
    FILE *billFile = fopen("bill.txt", "w");
    if (billFile != NULL) {
        time_t t;
        time(&t);
        fprintf(billFile, "------------------------------------------\n");
        fprintf(billFile, "SmartBuy: A Product order Management System\n");
        fprintf(billFile, "------------------------------------------\n");
        fprintf(billFile, "Date: %s", ctime(&t));
        fprintf(billFile, "------------------------------------------\n");
        fprintf(billFile, "Item      Quantity    Price    Total\n");
        fprintf(billFile, "------------------------------------------\n");

        for (int i = 0; i < numItems; ++i) {
            fprintf(billFile, "%-12s %3d  %8.2lf  %8.2lf\n", foodName[f[i] - 1], q[i], price[f[i] - 1], price[f[i] - 1] * q[i]);
        }

        fprintf(billFile, "------------------------------------------\n");
        fprintf(billFile, "TOTAL                        %8.2lf\n", total);
        fprintf(billFile, "------------------------------------------\n");
        fprintf(billFile, " Thanks for choosing us!           \n");
        fprintf(billFile, "------------------------------------------\n");
        fclose(billFile);
        printf("Bill has been saved to bill.txt\n");
    } else {
        printf("Error creating bill file.\n");
    }
}

// Function to log the transaction
void logTransaction(int numItems, int f[], int q[], double total) {
    FILE *logFile = fopen("transactions.log", "a");
    if (logFile != NULL) {
        time_t t;
        time(&t);
        fprintf(logFile, "------------------------------------------\n");
        fprintf(logFile, "Date: %s", ctime(&t));
        fprintf(logFile, "------------------------------------------\n");
        fprintf(logFile, "Item          Qty    Price    Total\n");
        fprintf(logFile, "------------------------------------------\n");

        for (int i = 0; i < numItems; ++i) {
        fprintf(logFile, "%-12s %3d   %8.2lf %8.2lf\n", foodName[f[i] - 1], q[i], price[f[i] - 1], price[f[i] - 1] * q[i]);
        }

        fprintf(logFile, "------------------------------------------\n");
        fprintf(logFile, "TOTAL                       %8.2lf\n", total);
        fprintf(logFile, "------------------------------------------\n");
        fclose(logFile);
    } else {
        printf("Error creating transaction log file.\n");
    }
}

// Function to view transactions
void viewTransactions() {
    FILE *logFile = fopen("transactions.log", "r");
    if (logFile != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), logFile)) {
            printf("%s", line);
        }
        fclose(logFile);
    } else {
        printf("Error opening transaction log file.\n");
    }
}

// Function to edit a product
void editProduct() {
    int choice, newPrice, newStock;
    printFoodTable();
    printf("Enter the product no. to edit: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > NUM_ITEMS) {
        fprintf(stderr, "Error: invalid Item number.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return;
    }

    printf("Enter the new price: ");
    if (scanf("%d", &newPrice) != 1 || newPrice < 0) {
        fprintf(stderr, "Error: invalid price.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return;
    }

    printf("Enter the new stock quantity: ");
    if (scanf("%d", &newStock) != 1 || newStock < 0) {
        fprintf(stderr, "Error: invalid stock quantity.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return;
    }

    price[choice - 1] = newPrice;
    inStock[choice - 1] = newStock;
    printf("Product updated successfully.\n");
}
