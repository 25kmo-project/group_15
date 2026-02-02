# Group 15 Project-ATM Banking System
This is a course project designed to simulate real-world bank ATM operations.

## Project Introduction
The system follows a decoupled architecture (Frontend and Backend separation):  
Frontend (**Qt/C++**): Provides a Graphical User Interface (GUI) for interactive user operations.  
Backend (**Node.js**): Processes core business logic, such as PIN verification and balance management.  
Database (**MySQL**): Stores user information, account balances, and all transaction logs.

## Tech Stack
### Frontend
Framework: Qt (C++)   
Networking: QNetworkAccessManager (For asynchronous REST API communication)  
Data Parsing: QJsonDocument & QJsonObject
UI Design: ?

### Backend
Runtime: Node.js
Framework: Express.js
Environment Management: dotenv 
Security: bcryptjs 
Middleware: cors 

### Database
System: MySQL 8.0+  
Library: mysql2   
Schema: Relational multi-table design with Transactional Integrity  
Data Structure
Database "bank" utilizes a 5-table relational schema:![ER diagram](database/ER_diagram.pdf)


## Backend-database
### User case
login by card number + pin code.The database only stores pin codes that have been hashed using bcryptjs.
After a user logs in, the system will provide selectable accounts for the user to operate:debit or credit.

......


### 
#### Table: user
* user_id (PK, INT)
* user_name (VARCHAR(45))
* user_phonenumber (VARCHAR(45))
#### Table: card
Stores authentication credentials and their link to a user.
* card_id (PK, INT)
* card_number (VARCHAR(20), UNIQUE): The number used for ATM login.
* pin_code (VARCHAR(255)): Bcrypt-hashed password.
* status ENUM('Active', 'Blocked', 'Expired'). 
* user_id (FK, INT): Links the card to its owner in the user table.
#### Table: account
Stores financial data and overdraft rules.
* account_id (PK, INT)
* account_type ENUM('DEDIT', 'CREDIT', 'SAVINGS')
* balance (DECIMAL(15,2)): for debit card
* credit_limit (DECIMAL(15,2)): for credit card
#### Table: account_access (Junction Table,"many to many")
Connecting cards to multiple accounts.
* card_id (PK/FK, INT)
* account_id (PK/FK, INT)
* access_type (ENUM): ENUM('FULL', 'VIEW ONLY'):for futher fuctions,eg:parents-child account.
#### Table: transaction
A permanent, immutable audit trail of all account activities.
* transaction_id (PK, INT)
* type ENUM('Withdrawal', 'Deposit', 'Transfer', 'Inquiry')
* amount (DECIMAL(15,2))
* date (DATETIME)
* account_id (FK, INT): Links the transaction to the specific affected account.
## project stucture
```text
.backend/
├── app.js                      # Main server file (configures middleware & routes)
├── .env                        # Private config: DB_USER, DB_PASSWORD, JWT_SECRET
├── package.json                # Dependencies: express, mysql2, dotenv, jsonwebtoken
├── config/
│   └── db.js                   # MySQL connection pool logic
├── middleware/
│   └── auth.js                 # Middleware to verify JWT Token before allowing transactions
├── models/                     # DATA LAYER (SQL Queries)
│   ├── transactionModel.js     # SQL for INSERT/SELECT transactions
│   ├── accountModel.js         # SQL for updating balances (used in your logic)
│   └── cardModel.js            # SQL for card-related checks
├── controllers/                # LOGIC LAYER (The "Brain")
│   ├── transactionController.js # The logic for "Withdraw" (check balance -> update -> record)
│   └── loginController.js      # Logic for authenticating and generating tokens
└── routes/                     # API ENDPOINTS
    ├── transaction.js          # API paths like /transaction/withdraw
    └── login.js                # API path for /login
```

```text
frontend/
├── bank-automat.pro            # Qt project configuration
├── main.cpp                    # Application entry
├── headers/                    # Header files (.h)
│   ├── transactionwindow.h     # Header for the transaction UI
│   ├── mainmenu.h              # Header for the main menu navigation
│   └── restapi.h               # Header for the central API communication class
├── sources/                    # Implementation files (.cpp)
│   ├── transactionwindow.cpp   # Logic for clicking buttons & handling JSON
│   ├── mainmenu.cpp            # Logic for switching between balance and withdrawal
│   └── restapi.cpp             # The class that handles QNetworkAccessManager (HTTP requests)
└── forms/                      # UI Design files (.ui)
    ├── transactionwindow.ui    # Design for the list of transactions
    └── mainmenu.ui             # Design for the main ATM menu
```