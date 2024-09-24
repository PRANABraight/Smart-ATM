#include <stdio.h>
#include <string.h>


#define MAX_ACCOUNTS 100

// Define a structure for user accounts
struct Account {
    int accountNumber;
    char name[50];
    int pin;
    double balance;
    double lastTransactions[5];
    int transactionCount;
};

// Define a structure for transactions (to be saved in the binary file)
struct Transaction {
    int accountNumber;
    char type[20];
    double amount;
    double newBalance;
};

// Function prototypes
void createAccount(struct Account accounts[], int *totalAccounts);
void deposit(struct Account accounts[], int index, double amount, FILE *file);
void withdraw(struct Account accounts[], int index, double amount, FILE *file);
void checkBalance(struct Account accounts[], int index);
void miniStatement(struct Account accounts[], int index);
int authenticate(struct Account accounts[], int totalAccounts, int accountNumber, int pin);
void transferFunds(struct Account accounts[], int senderIndex, int receiverIndex, double amount, FILE *file);
void saveTransaction(FILE *file, struct Account accounts[], int index, double amount, const char *type);
void saveAccounts(struct Account accounts[], int totalAccounts);
void loadAccounts(struct Account accounts[], int *totalAccounts);

int main() {
    struct Account accounts[MAX_ACCOUNTS];
    int totalAccounts = 0;
    int accountNumber, pin, authenticatedAccountIndex;
    int choice;
    double amount;

    // Load accounts from binary file
    loadAccounts(accounts, &totalAccounts);

    FILE *transactionFile = fopen("transactions.bin", "ab+");
    if (transactionFile == NULL) {
        printf("Error opening transaction file.\n");
        return 1;
    }

    do {
        // Main menu
        printf("\n--- Smart ATM System ---\n");
        printf("1. Create New Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Check Balance\n");
        printf("5. Mini Statement\n");
        printf("6. Transfer Funds\n");
        printf("7. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createAccount(accounts, &totalAccounts);
                saveAccounts(accounts, totalAccounts); // Save accounts after creation
                break;
            case 2:
                printf("Enter Account Number: ");
                scanf("%d", &accountNumber);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                authenticatedAccountIndex = authenticate(accounts, totalAccounts, accountNumber, pin);
                if (authenticatedAccountIndex != -1) {
                    printf("Enter amount to deposit: ");
                    scanf("%lf", &amount);
                    deposit(accounts, authenticatedAccountIndex, amount, transactionFile);
                    saveAccounts(accounts, totalAccounts); // Save accounts after deposit
                } else {
                    printf("Invalid Account Number or PIN.\n");
                }
                break;
            case 3:
                printf("Enter Account Number: ");
                scanf("%d", &accountNumber);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                authenticatedAccountIndex = authenticate(accounts, totalAccounts, accountNumber, pin);
                if (authenticatedAccountIndex != -1) {
                    printf("Enter amount to withdraw: ");
                    scanf("%lf", &amount);
                    withdraw(accounts, authenticatedAccountIndex, amount, transactionFile);
                    saveAccounts(accounts, totalAccounts); // Save accounts after withdrawal
                } else {
                    printf("Invalid Account Number or PIN.\n");
                }
                break;
            case 4:
                printf("Enter Account Number: ");
                scanf("%d", &accountNumber);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                authenticatedAccountIndex = authenticate(accounts, totalAccounts, accountNumber, pin);
                if (authenticatedAccountIndex != -1) {
                    checkBalance(accounts, authenticatedAccountIndex);
                } else {
                    printf("Invalid Account Number or PIN.\n");
                }
                break;
            case 5:
                printf("Enter Account Number: ");
                scanf("%d", &accountNumber);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                authenticatedAccountIndex = authenticate(accounts, totalAccounts, accountNumber, pin);
                if (authenticatedAccountIndex != -1) {
                    miniStatement(accounts, authenticatedAccountIndex);
                } else {
                    printf("Invalid Account Number or PIN.\n");
                }
                break;
            case 6:
                printf("Enter Account Number: ");
                scanf("%d", &accountNumber);
                printf("Enter PIN: ");
                scanf("%d", &pin);
                authenticatedAccountIndex = authenticate(accounts, totalAccounts, accountNumber, pin);
                if (authenticatedAccountIndex != -1) {
                    int transferAccountNumber;
                    printf("Enter account number to transfer to: ");
                    scanf("%d", &transferAccountNumber);
                    int transferAccountIndex = authenticate(accounts, totalAccounts, transferAccountNumber, pin);
                    if (transferAccountIndex != -1) {
                        printf("Enter amount to transfer: ");
                        scanf("%lf", &amount);
                        transferFunds(accounts, authenticatedAccountIndex, transferAccountIndex, amount, transactionFile);
                        saveAccounts(accounts, totalAccounts); // Save accounts after transfer
                    } else {
                        printf("Transfer account not found.\n");
                    }
                } else {
                    printf("Invalid Account Number or PIN.\n");
                }
                break;
            case 7:
                printf("Thank you for using the Smart ATM System.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    fclose(transactionFile);
    return 0;
}

// Function to create a new account
void createAccount(struct Account accounts[], int *totalAccounts) {
    if (*totalAccounts >= MAX_ACCOUNTS) {
        printf("Account limit reached. Cannot create more accounts.\n");
        return;
    }
    struct Account newAccount;
    newAccount.accountNumber = 100 + *totalAccounts + 1;
    printf("Enter account holder's name: ");
    scanf(" %[^\n]", newAccount.name);
    printf("Set a 4-digit PIN: ");
    scanf("%d", &newAccount.pin);
    newAccount.balance = 0;
    newAccount.transactionCount = 0;

    accounts[*totalAccounts] = newAccount;
    (*totalAccounts)++;

    printf("Account created successfully. Account Number: %d\n", newAccount.accountNumber);
}

// Function to deposit money
void deposit(struct Account accounts[], int index, double amount, FILE *file) {
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    accounts[index].balance += amount;
    accounts[index].lastTransactions[accounts[index].transactionCount % 5] = amount;
    accounts[index].transactionCount++;
    printf("Deposit successful. New balance: $%.2f\n", accounts[index].balance);

    saveTransaction(file, accounts, index, amount, "Deposit");
}

// Function to withdraw money
void withdraw(struct Account accounts[], int index, double amount, FILE *file) {
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    if (amount > accounts[index].balance) {
        printf("Insufficient balance.\n");
        return;
    }
    accounts[index].balance -= amount;
    accounts[index].lastTransactions[accounts[index].transactionCount % 5] = -amount;
    accounts[index].transactionCount++;
    printf("Withdrawal successful. New balance: $%.2f\n", accounts[index].balance);

    saveTransaction(file, accounts, index, -amount, "Withdrawal");
}

// Function to check balance
void checkBalance(struct Account accounts[], int index) {
    printf("Current balance: $%.2f\n", accounts[index].balance);
}

// Function to print mini statement
void miniStatement(struct Account accounts[], int index) {
    printf("Mini Statement:\n");
    int start = accounts[index].transactionCount >= 5 ? accounts[index].transactionCount - 5 : 0;
    for (int i = start; i < accounts[index].transactionCount; i++) {
        printf("Transaction %d: %+.2f\n", i + 1, accounts[index].lastTransactions[i % 5]);
    }
}

// Function to authenticate user
int authenticate(struct Account accounts[], int totalAccounts, int accountNumber, int pin) {
    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].accountNumber == accountNumber && accounts[i].pin == pin) {
            return i;
        }
    }
    return -1;
}

// Function to transfer funds
void transferFunds(struct Account accounts[], int senderIndex, int receiverIndex, double amount, FILE *file) {
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    if (amount > accounts[senderIndex].balance) {
        printf("Insufficient balance for transfer.\n");
        return;
    }
    
    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;
    
    accounts[senderIndex].lastTransactions[accounts[senderIndex].transactionCount % 5] = -amount;
    accounts[senderIndex].transactionCount++;
    
    accounts[receiverIndex].lastTransactions[accounts[receiverIndex].transactionCount % 5] = amount;
    accounts[receiverIndex].transactionCount++;
    
    printf("Transfer successful. New balance: $%.2f\n", accounts[senderIndex].balance);

    saveTransaction(file, accounts, senderIndex, -amount, "Transfer Sent");
    saveTransaction(file, accounts, receiverIndex, amount, "Transfer Received");
}

// Function to save accounts to a binary file
void saveAccounts(struct Account accounts[], int totalAccounts) {
    FILE *file = fopen("accounts.bin", "wb");
    if (file == NULL) {
        printf("Error saving accounts.\n");
        return;
    }
    fwrite(accounts, sizeof(struct Account), totalAccounts, file);
    fclose(file);
}

// Function to load accounts from a binary file
void loadAccounts(struct Account accounts[], int *totalAccounts) {
    FILE *file = fopen("accounts.bin", "rb");
    if (file != NULL) {
        *totalAccounts = fread(accounts, sizeof(struct Account), MAX_ACCOUNTS, file);
        fclose(file);
    } else {
        printf("No existing account data found. Starting fresh.\n");
    }
}

// Function to save a transaction to a binary file
void saveTransaction(FILE *file, struct Account accounts[], int index, double amount, const char *type) {
    struct Transaction transaction;
    transaction.accountNumber = accounts[index].accountNumber;
    strcpy(transaction.type, type);
    transaction.amount = amount;
    transaction.newBalance = accounts[index].balance;

    fwrite(&transaction, sizeof(struct Transaction), 1, file);
}
