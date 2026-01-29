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
            "SELECT * FROM transaction WHERE account_id=? ORDER BY date DESC", 
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
            "INSERT INTO transaction (type, amount, account_id, card_id) VALUES (?,?,?,?)",
            [
                transaction.type, 
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
            "UPDATE transaction SET type=?, amount=?, date=?, account_id=?, card_id=? WHERE transaction_id=?",
            [
                transaction.type, 
                transaction.amount, 
                transaction.date, 
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

   // withdraw function
    withdraw: function(data, callback) {
        const { account_id, card_id, amount } = data;

        // 1. Input Validation
        if (!amount || amount <= 0) {
            return callback({ error: 'INVALID_AMOUNT', message: 'Amount must be greater than 0' });
        }

        // 2. Get a specific connection from the pool
        db.getConnection(function(err, connection) {
            if (err) return callback(err);

            // 3. Begin Transaction
            connection.beginTransaction(function(err) {
                if (err) {
                    connection.release();
                    return callback(err);
                }

                // 4. Check Card status with Row Locking
                const checkSql = `
                   SELECT 
                       a.balance, 
                       c.status AS card_status,
                       aa.access_type             
                   FROM account a
                   JOIN account_access aa ON a.account_id = aa.account_id 
                   JOIN card c ON aa.card_id = c.card_id                  
                   WHERE a.account_id = ? AND c.card_id = ?
                   FOR UPDATE`;

                // connection.query
                connection.query(checkSql, [account_id, card_id], function(err, results) {
                    if (err) {
                        return connection.rollback(function() {
                            connection.release();
                            callback(err);
                        });
                    }

                    // 5. Integrity & Business Logic Checks
                    if (results.length === 0) {
                        return connection.rollback(function() {
                            connection.release();
                            callback({ error: 'NOT_FOUND', message: 'Invalid Account or Card' });
                        });
                    }

                    const { balance, card_status } = results[0];

                    if ( card_status !== 'ACTIVE') {
                        return connection.rollback(function() {
                            connection.release();
                            callback({ error: 'LOCKED', message: 'Card is not active' });
                        });
                    }

                    if (balance < amount) {
                        return connection.rollback(function() {
                            connection.release();
                            callback({ error: 'INSUFFICIENT_FUNDS', message: 'Insufficient balance' });
                        });
                    }

                    // 6. Update Account Balance
                    connection.query("UPDATE account SET balance = balance - ? WHERE account_id = ?", [amount, account_id], function(err) {
                        if (err) {
                            return connection.rollback(function() {
                                connection.release();
                                callback(err);
                            });
                        }

                        // 7. Log Transaction
                        connection.query(
                            "INSERT INTO transaction (type, amount, account_id, card_id) VALUES ('Withdrawal', ?, ?, ?)",
                            [amount, account_id, card_id],
                            function(err, insRes) {
                                if (err) {
                                    return connection.rollback(function() {
                                        connection.release();
                                        callback(err);
                                    });
                                }

                                // 8. Commit Transaction
                                connection.commit(function(err) {
                                    if (err) {
                                        return connection.rollback(function() {
                                            connection.release();
                                            callback(err);
                                        });
                                    }
                                    
                                    // 9. Release Connection and Return Success
                                    connection.release();
                                    callback(null, { 
                                        transaction_id: insRes.insertId, 
                                        remaining_balance: balance - amount 
                                    });
                                }); // end commit
                            }
                        ); // end insert
                    }); // end update
                }); // end select
            }); // end beginTransaction
        }); // end getConnection
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
            t.type AS 'Tyyppi', 
            t.amount AS 'Määrä', 
            t.date AS 'Aika'  
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

            // 3. Return success data
            callback(null, {
                owner: `${row.user_name} ${row.user_lastname}`,
                balance: row.balance,
                account_number: row.account_number
            });
        });
    }
};
module.exports = transaction;