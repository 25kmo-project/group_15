# Group 15 Project-ATM Banking System
This is a course project designed to simulate real-world bank ATM operations.

## Project Introduction
The system follows a decoupled architecture (Frontend and Backend separation):  
Frontend (**Qt/C++**): Provides a Graphical User Interface (GUI) for interactive user operations.  
Backend (**Node.js**): Processes core business logic, such as PIN verification and balance management.  
Database (**MySQL/MariaDB**): Stores user information, account balances, and all transaction logs.

## Tech Stack
### Frontend
Framework: Qt (C++)   
Networking: QNetworkAccessManager (For asynchronous REST API communication)  
Data Parsing: QJsonDocument & QJsonObject  
UI Design: QT designer  
Build system: Cmake

### Backend
Runtime: Node.js  
Framework: Express.js  
Environment Management: dotenv    
Security: bcryptjs and jsonwebtoken  
Middleware: cors   
Logging: morgan 
Database Client: mysql2 


### Database
System: MySQL 8.0+ / MariaDB  
Library: mysql2   
Schema: Relational multi-table design with Transactional Integrity  
Data Structure
Database "bank" utilizes a 5-table relational schema:![ER diagram](database/ER_diagram.pdf)

## Key feature  
**Multi-Account Support**: A single card can be linked to multiple accounts. Users can choose between Debit or Credit accounts after logging in.  
**Core Financial Transactions**:
* Withdrawal: Checks balance or credit limits, updates the account in real-time, and records the transaction.
* Deposit: Increases account balance.
* Transfer*: Facilitates fund transfers between different accounts.

**Security Mechanisms**:
* PIN codes are securely hashed using bcryptjs; no plain-text passwords are stored.
* API requests are protected by JWT Tokens to prevent unauthorized access.
* Logic included for handling Blocked card statuses.

**Additional Features**:
* Currency Conversion: Supports calculations between different currency units.
* Digital Receipt: Provides confirmation and details for completed transactions.
## Project stucture
```text
.backend/
├── app.js            
├── bin/www                
├── middleware/
│   └── auth.js                 
├── models/   
│   ├── login_model.js          
│   ├── user_model.js 
│   ├── transaction_model.js    
│   ├── account_model.js  
│   ├── account_access_model.js       
│   └── card_model.js            
└── routes/ 
│   ├── account.js 
│   ├── account_access.js 
│   ├── card.js 
│   ├── currency.js 
│   ├── database.js  
│   ├── index.js
│   ├── transaction.js 
│   ├── user.js       
│   login.js  
├── package.json     
└── package-lock.json            
```

```text
frontend/
├── CMakeLists.txt            
├── bank-automat.iss   
├── main.cpp              
├── headers/    
│   ├── environment.h 
│   ├── mainwindow.h                 
│   ├── menu.h 
│   ├── debitvscredit.h 
│   ├── clientinfo.h 
│   ├── balance.h 
│   ├── deposit.h 
│   ├── debitvscredit.h 
│   ├── withdrawal.h 
│   ├── transaction.h     
│   ├── transfer.h 
│   ├── currency.h                
│   └── receipt.h               
├── sources/   
│   ├── environment.cpp  
│   ├── mainwindow.cpp  
│   ├── menu.cpp  
│   ├── debitvscredit.cpp   
│   ├── clientinfo.cpp   
│   ├── balance.cpp   
│   ├── deposit.cpp   
│   ├── withdrawal.cpp   
│   ├── transaction.cpp   
│   ├── transfer.cpp     
│   ├── currency.cpp           
│   └── receipt.cpp             
└── forms/
    ├── mainwindow.ui                        
    ├── menu.ui  
    ├── debitvscredit.ui  
    ├── clientinfo.ui  
    ├── balance.ui  
    ├── deposit..ui  
    ├── withdrawal.ui  
    ├── transaction.ui  
    ├── transfer.ui  
    ├── currency.ui  
    └── receipt.ui               
```
## Quick install
### Database Setup
For MySQL:
```bash
mysql -u your_username -p < database/sql_script_mysql_version.sql
```
For MariaDB:
```bash
mariadb -u your_username -p < database/sql_script_mariadb_version.sql
```
The project includes database/db_setup.sql for initializing the core database schema.

### Backend Environment
* Install all necessary dependencies.
```bash
cd backend
npm install
```
* Copy **.env.example** to a new file named **.env** and configure your database connection credentials and JWT secret.
* Launch the server.
```bash
npm start
```

### Frontend Environment
* Ensure Qt 5.15+ and CMake are installed.
* in the **bank-automat** directory:
```bash
mkdir build && cd build
cmake ..
make  
```
* Or open CMakeLists.txt directly with Qt Creator

## CI/CD & Automated Workflows
The project utilizes **GitHub Actions** for continuous integration and automated releases:
* **Build Workflows**: Every pull request to the main branch triggers automated builds for Windows (MSVC 2022) and macOS (Apple Silicon/ARM64) to ensure code stability and cross-platform compatibility.
* **Release Workflows**: Pushing a version tag (e.g., v1.0) automatically initiates the distribution process:
    * Windows: Packages the application into a professional installer (.exe) using Inno Setup。
    * macOS: Generates a standard Disk Image (.dmg) for Apple Silicon users。

## Authors

- [@juanyu0417](https://github.com/JuanYu0417)
- [@jummijammi](https://github.com/jummijammi)
- [@tinnihkis](https://github.com/tinnihkis)
- [@ecedevere](https://github.com/ecedevere)

## License
This project is licensed under the [MIT License](LICENSE).

## DRAFT_1 or What should be also included in readme

- how to install our app
- what folder documents have and what it describes(documents are not added yet)
- we have db version for mysql and mariadb
- e have client scenarios (scenarios are not added yet)
- (?)if client has no card, he/she cant use bank
- (?)if card is blocked, no access to bank
- database - procedures's description(not done yet, they will be for nosto/pano)
- app additional features: currency and digital receipt
- database description: if one card holder->menu, if dual card->debit or credit selection->menu
- github windows and mac build and release workflows

## DRAFT_2 or teacher's example
- What was the project: For example the project name, primary idea and target, when, why
- Which technologies were used
- Which tools were used
- Who did what parts of the project (if groupwork)
- Application architecture, database schema, APIs, maybe ABIs, UI/UX plan, protocols
- Instructions how to deploy and use the project code
- Link to the server / service where the project is/was running
- Related presentation material(s): slides, images, videos about the project, posters, links or good quality images to basic project management and tool views (kanban stuff, Trello, Taiga, Jira, Github, Jenkins, CI/CD, IaC etc)
- Actual source codes: Commented code, code hierarchy if it is not obvious, using programming language and framework style guide (code entity identifiers like classes, variables etc named ok, syntax is clear overall…), avoid obsolete stuff etc. Code commits, pull requests, issues, merges etc
