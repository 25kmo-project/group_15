const db = require('../routes/database');

const transaction = {
    
    // Get all transactions from the table
    getAll: function(callback) {
        return db.query("SELECT * FROM transaction", callback);
    },

    // Get a specific transaction by its ID
    getById: function(id, callback) {
        return db.query("SELECT * FROM transaction WHERE transaction_id=?", [id], callback);
    },

    // Get all transactions for a specific account (Useful for showing statement/history)
    // Ordered by date DESC to show the latest transactions first
    getByAccountId: function(account_id, callback) {
        return db.query(
            "SELECT * FROM transaction WHERE account_id=? ORDER BY transaction_date DESC", 
            [account_id], 
            callback
        );
    },
    getByCardId: function(id, callback) {
        return db.query(
            'SELECT * FROM transaction WHERE card_id = ?', 
            [id], 
            callback
        );
    },

    // Add a new transaction record
    add: function(transaction, callback) {
        return db.query(
            "INSERT INTO transaction (transaction_type, amount, account_id, card_id) VALUES (?,?,?,?)",
            [
                transaction.transaction_type, 
                transaction.amount, 
                transaction.account_id, 
                transaction.card_id
            ],
            callback
        );
    },

    // Update existing transaction data
    update: function(id, transaction, callback) {
        return db.query(
            "UPDATE transaction SET transaction_type=?, amount=?, transaction_date=?, account_id=?, card_id=? WHERE transaction_id=?",
            [
                transaction.transaction_type, 
                transaction.amount, 
                transaction.transaction_date, 
                transaction.account_id, 
                transaction.card_id, 
                id
            ],
            callback
        );
    },

    // Remove a transaction from the database
    delete: function(id, callback) {
        return db.query("DELETE FROM transaction WHERE transaction_id=?", [id], callback);
    },

// withdraw function (STORED PROCEDURE)
withdraw: function (data, callback) {
    const { account_id, card_id, amount } = data;

    // 1) Input validation
    if (!amount || amount <= 0) {
        return callback({ error: 'INVALID_AMOUNT', message: 'Amount must be greater than 0' });
    }

    // 2) Call stored procedure
    return db.query(
        "CALL withdraw_money(?, ?, ?)",
        [account_id, card_id, amount],
        function (err, results) {
            if (err) {
                return callback({
                    error: 'DB_ERROR',
                    message: err.sqlMessage || err.message || 'Withdrawal failed'
                });
            }

            const newBalance =
                results && results[0] && results[0][0] && results[0][0].new_balance !== undefined
                    ? parseFloat(results[0][0].new_balance)
                    : null;

            callback(null, {
                status: "SUCCESS",
                new_balance: newBalance
            });
        }
    );
},
// deposit function (STORED PROCEDURE)
deposit: function (data, callback) {
    const { account_id, card_id, amount } = data;

    // 1) Input validation
    if (!amount || amount <= 0) {
        return callback({ error: 'INVALID_AMOUNT', message: 'Amount must be greater than 0' });
    }

    // 2) Call stored procedure
    return db.query(
        "CALL deposit_money(?, ?, ?)",
        [account_id, card_id, amount],
        function (err, results) {
            if (err) {
                return callback({
                    error: 'DB_ERROR',
                    message: err.sqlMessage || err.message || 'Deposit failed'
                });
            }

            // Stored procedure tekee SELECT new_balance
            const newBalance =
                results && results[0] && results[0][0] && results[0][0].new_balance !== undefined
                    ? parseFloat(results[0][0].new_balance)
                    : null;

            callback(null, {
                status: "SUCCESS",
                new_balance: newBalance
            });
        }
    );
},
    
    //View Transaction History
    getTransactionHistory: function(data, callback) {
        const { account_id, card_id, page } = data;
        const limit = 10;
        const offset = (page - 1) * limit;

        const sql = `
        SELECT 
            u.user_name AS 'Etunimi', 
            u.user_lastname AS 'Sukunimi', 
            t.transaction_id AS 'ID', 
            t.transaction_type AS 'Tyyppi', 
            t.amount AS 'Määrä', 
            t.transaction_date AS 'Aika'  
        FROM account a
        JOIN account_access aa ON a.account_id = aa.account_id
        JOIN card c ON aa.card_id = c.card_id
        JOIN user u ON c.user_id = u.user_id
        LEFT JOIN transaction t ON a.account_id = t.account_id
        WHERE a.account_id = ? AND c.card_id = ?
        ORDER BY t.transaction_id DESC
        LIMIT ? OFFSET ?`;

        return db.query(sql, [account_id, card_id, limit, offset], callback);
    },

    // banlance inquiry
    getBalance: function(data, callback) {
        const { account_id, card_id } = data;

        const sql = `
            SELECT 
                u.user_name, u.user_lastname, 
                a.balance, a.account_number, 
                c.status AS card_status
            FROM account_access aa
            JOIN account a ON aa.account_id = a.account_id
            JOIN user u ON a.user_id = u.user_id
            JOIN card c ON aa.card_id = c.card_id
            WHERE aa.account_id = ? AND aa.card_id = ?`;

        db.query(sql, [account_id, card_id], function(err, results) {
            if (err) return callback(err);

            // 1. Check if the card has access to this account
            if (results.length === 0) {
                return callback({ error: 'NOT_FOUND', message: 'Access denied' });
            }

            const row = results[0];

            // 2. Validate card status
            if (row.card_status !== 'ACTIVE') {
                return callback({ error: 'CARD_LOCKED', message: 'Card is not active' });
            }

            //insert inquiry log
            const logSql = `
            INSERT INTO transaction (account_id, card_id, amount, transaction_type, transaction_date) 
            VALUES (?, ?, 0.00, 'INQUIRY', NOW())`;
        
        db.query(logSql, [account_id, card_id], function(logErr) {
            if (logErr) {
                console.error("Failed to log Inquiry:", logErr.sqlMessage);
            } else {
                console.log(`Inquiry logged for account ${account_id}`);
            }

            // 3. Return success data
            callback(null, {
                owner: `${row.user_name} ${row.user_lastname}`,
                balance: parseFloat(row.balance),
                account_number: row.account_number
            });
        });
       });
    },
    getBalanceNoLog: function(data, callback) {
  const { account_id, card_id } = data;

  const sql = `
    SELECT 
      u.user_name, u.user_lastname, 
      a.balance, a.account_number, 
      c.status AS card_status
    FROM account_access aa
    JOIN account a ON aa.account_id = a.account_id
    JOIN user u ON a.user_id = u.user_id
    JOIN card c ON aa.card_id = c.card_id
    WHERE aa.account_id = ? AND aa.card_id = ?`;

  db.query(sql, [account_id, card_id], function(err, results) {
    if (err) return callback(err);
    if (results.length === 0) {
      return callback({ error: 'NOT_FOUND', message: 'Access denied' });
    }

    const row = results[0];
    if (row.card_status !== 'ACTIVE') {
      return callback({ error: 'CARD_LOCKED', message: 'Card is not active' });
    }

    callback(null, {
      owner: `${row.user_name} ${row.user_lastname}`,
      account_number: row.account_number,
      balance: parseFloat(row.balance)
    });
  });
},
    
    transfer: function(data, callback) {
        const { sender_account_id, receiver_account_number, card_id, amount } = data;
        const sql = "CALL transfer_money(?, ?, ?, ?)";
        
        return db.query(
            sql, 
            [sender_account_id, receiver_account_number, card_id, amount], 
            callback
        );
    }
};
module.exports = transaction;