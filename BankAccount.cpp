#include<iostream>     //Bank Management System
#include<vector>
#include<string>
#include<fstream>
#include <csignal>
#include <sstream>
#include <algorithm>

using namespace std;

class BankAccount {
private:
    string name;
    int actNum;
    double balance;
    vector<string> transactionHistory;
public:
    BankAccount(string n, int acc, double bal) {
        name = n;
        actNum = acc;
        balance = bal;
    }
    string getName() const {
        return name;
    }
    int getActNumber() const{
        return actNum;
    }
    double getBalance() const{
        return balance;
    }
    void Deposit(double amt) {
        balance = balance + amt;
        recordTransaction("Deposit", amt);
        cout<<"\t\tDeposit Successful!"<<endl;
        cout<<"\t\tYour new Balance is " << balance << endl;
    }
    void Withdraw(double amt) {
        if(balance >= amt) {
            balance = balance - amt;
            recordTransaction("Withdraw", amt);
            cout<<"\t\tWithdraw Successful!"<<endl;
            cout<<"\t\tYour new Balance is " << balance << endl;
        }
        else {
            cout<<"Your Balance is not sufficient."<<endl;
            cout<<"Current Balance is "<<balance<<endl;
        }
    }
    void recordTransaction(const string& action, double amount) {
    transactionHistory.push_back(action + ": " + to_string(amount));
    // Save the transaction immediately after recording
    saveTransactionHistory("p_" + to_string(actNum) + ".txt");
}

    void saveTransactionHistory(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& transaction : transactionHistory) {
            file << transaction << endl;
        }
        cout << "Transaction history saved."<< endl;
    } 
    else {
        cout << "No transaction for this account" << filename << endl;
    }
    file.close();
}
    void loadAndDisplayTransactionHistory(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << " for reading." << endl;
        return;
    }
    string transaction;
    cout << "\t\tTransaction History:" << endl;
    while (getline(file, transaction)) {
        cout << "\t\t" << transaction << endl;
    }
    file.close();
}

};

class BankManagement {
private:
    vector<BankAccount> accounts;
    
public:
    void AddAccount(string name, int actNum, double balance) {
        accounts.push_back(BankAccount(name,actNum,balance));
    }
    void ShowAllAccounts() {
        for(int i = 0; i < accounts.size(); i++) {
            cout << "\t\tAccount Holder: "<< accounts[i].getName() << endl;
            cout << "\t\tAccount Number: " << accounts[i].getActNumber() << endl;
            cout << "\t\tCurrent Balance: " << accounts[i].getBalance() << endl;
            cout << "\t\t----------------------------" << endl;             
        }
    }
    void SearchAccount(int accountNumber) {
    bool accountFound = false;
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].getActNumber() == accountNumber) {
            cout << "\t\t----------------------------" << endl;
            cout << "\t\tAccount Holder: "<< accounts[i].getName() << endl;
            cout << "\t\tAccount Number: " << accounts[i].getActNumber() << endl;
            cout << "\t\tCurrent Balance: " << accounts[i].getBalance() << endl;
            cout << "\t\t----------------------------" << endl;
            accountFound = true;
            break; // Exit the loop once the account is found
        }
    }

    if (!accountFound) {
        cout << "Account doesn't Exist!! Please enter a valid Account Number" << endl;
    }
}

    BankAccount* findAccount(int accNo) {
        for (int i = 0; i < accounts.size(); i++) {
            if (accNo == accounts[i].getActNumber()) {
                return &accounts[i];
            }
        }
        cout << "Account doesn't exist." << endl;
        return NULL;
    }
    bool accountExists(int actNum) const {
        for (const BankAccount& account : accounts) {
            if (account.getActNumber() == actNum) {
                return true;
            }
        }
        return false;
    }
    void saveAccountsToFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& account : accounts) {
                file << account.getName() << "," << account.getActNumber() << "," << account.getBalance() << endl;
            }
            cout << "Accounts information saved. "<< endl;
        } else {
            cout << "Error: Unable to open file " << filename << " for writing." << endl;
        }
        file.close();
    }
    void loadAccountsFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        int lineNumber = 0; // Add line number counter
        while (getline(file, line)) {
            lineNumber++;
            stringstream ss(line);
            string name, balanceStr, actNumStr;
            double balance;
            int actNum;
            getline(ss, name, ',');
            getline(ss, actNumStr, ',');
            getline(ss, balanceStr, ',');
            try {
                actNum = stoi(actNumStr);
                balance = stod(balanceStr);
                accounts.emplace_back(name, actNum, balance);
            } catch (const std::invalid_argument& e) {
                cerr << "Invalid argument at line " << lineNumber << ": " << line << endl;
                cerr << "Error: " << e.what() << endl;
            }
        }
        cout << "Accounts information loaded from " << filename << endl;
    } else {
        cout << "Error: Unable to open file " << filename << " for reading." << endl;
    }
    file.close();
}
    void DeleteAccount(int accNo, const string& name) {
    auto it = std::remove_if(accounts.begin(), accounts.end(), [&](const BankAccount& acc) {
        return acc.getActNumber() == accNo && acc.getName() == name;
    });

    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
        cout << "Account deleted successfully!" << endl;
        saveAccountsToFile("p.txt");
    } else {
        cout << "Account not found." << endl;
    }
}

    void autosaveAccounts(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        // Write headers
        file << "Account Holder,Account Number,Current Balance,Transaction History" << endl;

        for (const auto& account : accounts) {
            file << account.getName() << ",";
            file << account.getActNumber() << ",";
            file << account.getBalance() << ",";

            // Collate transaction history into a single string, separated by semicolons
            string history;
            string accountFile = "p_" + to_string(account.getActNumber()) + ".txt";
            ifstream transactionFile(accountFile);
            if (transactionFile.is_open()) {
                string transaction;
                while (getline(transactionFile, transaction)) {
                    history += transaction + ";";
                }
                transactionFile.close();
            }
            // Remove the last semicolon if history is not empty
            if (!history.empty() && history.back() == ';') {
                history.pop_back();
            }

            file << "\"" << history << "\""; // Quote the history to ensure commas within do not split the field
            file << endl;
        }
        cout << "Accounts information saved to " << filename << endl;
    } else {
        cout << "Error: Unable to open file " << filename << " for writing." << endl;
    }
    file.close();
}

    void showAccountTransactions(int accNo) {
    BankAccount* account = findAccount(accNo);
    if (account != nullptr) {
        string filename = "p_" + to_string(accNo) + ".txt";
        account->loadAndDisplayTransactionHistory(filename);
    } else {
        cout << "Account not found." << endl;
    }
    }
};

BankManagement bank;

void handleTerminationSignal(int signum) {
    cout << "Saving transaction data before termination..." << endl;
    bank.autosaveAccounts("transaction_history.txt");
    exit(signum);
}

int main() {
    bank.loadAccountsFromFile("accounts_info.txt");
    int choice;
    char option;
    int actNo;
    double amount;
    string name;
    BankAccount* actN = nullptr;
    signal(SIGINT, handleTerminationSignal);
    do {
        system("CLS");
        cout << "\t\t________________________________" << endl;
        cout << "\t\t Bank Management System" << endl << endl;
        cout << "\t\t\tMain Menu" << endl << endl;
        cout << "\t\t1. Create Account" << endl;
        cout << "\t\t2. Show All Accounts" << endl;
        cout << "\t\t3. Search Account" << endl;
        cout << "\t\t4. Deposit Money" << endl;
        cout << "\t\t5. Withdraw Money" << endl;
        cout << "\t\t6. Show Balance" << endl;
        cout << "\t\t7. Show Transaction History" << endl;
        cout << "\t\t8. Delete Account" << endl;
        cout << "\t\t9. Exit" << endl;
        cout << "\t\t________________________________" << endl;
        cout << "Enter Your Choice" << endl;
        cin >> choice;

switch(choice) {
case 1: {
        double balance;
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Name:";
        cin >> name;
        cout << "\t\tEnter the account number: ";
        cin >> actNo;
        if (bank.accountExists(actNo)) {
        cout << "\t\tAccount number already exists. Please choose a different account number." << endl;
        break;
    }
        cout << "\t\tEnter initial deposit amount: ";
        cin >> balance;
        bank.AddAccount(name, actNo, balance);
        cout << "\t\t\tAccount Created Successfully" << endl;
        cout << "\t\t________________________________" << endl;

break;
}
case 2: {
        cout << "\t\t________________________________" << endl;
        bank.ShowAllAccounts();
        cout << "\t\t________________________________" << endl;

break;
}
case 3: {
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number:";
        cin >> actNo;
        bank.SearchAccount(actNo);
        cout << "\t\t________________________________" << endl;

break;
}
case 4: {
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number: ";
        cin >> actNo;
        BankAccount* account = bank.findAccount(actNo);
        cout<<"\t\t\tWelcome Back!"<<endl;
        cout << "\t\tEnter the amount you  want to Deposit: ";
        cin >> amount;
        account->Deposit(amount);
        cout << "\t\tDeposited Amount Successfully!" << endl;
        cout << "\t\t________________________________" << endl;

break;
}
case 5: {
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number: ";
        cin >> actNo;
        BankAccount* act = bank.findAccount(actNo);
        cout<<"\t\t\tWelcome Back!"<<endl;
        cout << "\t\tEnter the amount you  want to Withdraw: ";
        cin >> amount;
        act->Withdraw(amount);
        cout << "\t\t________________________________" << endl;

break;
}
case 6:
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number: ";
        cin >> actNo;
        actN = bank.findAccount(actNo);
        if (actN != NULL) {
            cout << "\t\tYour Account has balance:" << actN->getBalance() << endl;
            }
        cout << "\t\t________________________________" << endl;

break;

case 7: {
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number:";
        cin >> actNo;
        bank.showAccountTransactions(actNo);
        cout << "\t\t________________________________" << endl;
        break;
}
case 8:{
        cout << "\t\t________________________________" << endl;
        cout << "\t\tEnter your Account Number:";
        cin >> actNo;
        cout << "\t\tEnter your Name:";
        cin >> name;
        bank.DeleteAccount(actNo, name);
        cout<< "\t\t________________________________" << endl;
    break;
}
case 9:
    bank.autosaveAccounts("accounts_info.txt");
    cout<< "\t\t________________________________" << endl<<endl;
    cout << "\t\tThank You for using our services!!" << endl<<endl;
    cout<< "\t\t________________________________" << endl;
    exit(0);
    break;

default:
        cout << "No such choice is available" << endl;
break;
}

    cout << "Do you want to continue or not?[y/n]" << endl;
    cin >> option;
} 
    while (option == 'Y' || option == 'y');   
     bank.saveAccountsToFile("p.txt");
}
