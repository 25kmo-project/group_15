const db = require('../routes/database');

const transaction = {
    
    //Get all transactions
    getAll: function(callback) {
        return db.query("SELECT * FROM transaction", callback);
    },

    //Get a specific transaction by its id
    getById: function(id, callback) {
        return db.query("SELECT * FROM transaction WHERE transaction_id=?", [id], callback);
    },

    //Get all transactions for a specific account
    //Ordered by date DESC to show the latest transactions first
    getByAccountId: function(account_id, callback) {
        return db.query(
            "SELECT * FROM transaction WHERE account_id=? ORDER BY transaction_date DESC", 
            [account_id], 
            callback
        );
    },
    //Get all transactions for a specific card
    getByCardId: function(id, callback) {
        return db.query(
            'SELECT * FROM transaction WHERE card_id = ?', 
            [id], 
            callback
        );
    },

    //Add a new transaction record
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

    //Update existing transaction data
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

    //Delete a transaction by id
    delete: function(id, callback) {
        return db.query("DELETE FROM transaction WHERE transaction_id=?", [id], callback);
    },

//thdraw money via stored procedure
withdraw: function (data, callback) {
    const { account_id, card_id, amount } = data;

    // 1. Input validation
    if (!amount || amount <= 0 || amount % 10 !== 0 || amount === 10 || amount === 30) {
        return callback({ error: 'INVALID_AMOUNT', message: 'Amount must be greater than 0 and be possible with 20€ and 50€ notes' });
    }
    
    let count50 = Math.floor(amount / 50);
    let remainder = amount % 50;
    if (remainder % 20 !== 0) {
        count50 -= 1;
    }
    
    let count20 = (amount - (count50 * 50)) / 20;

    //2. Call stored procedure
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
                new_balance: newBalance,
                bills: {
                    notes_50: count50,
                    notes_20: count20
                }
            });
        }
    );
},
//Deposit money via stored procedure
deposit: function (data, callback) {
    const { account_id, card_id, amount } = data;

    //1. Input validation
    if (!amount || amount <= 0) {
        return callback({ error: 'INVALID_AMOUNT', message: 'Amount must be greater than 0' });
    }

    //2. Call stored procedure
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
    
    //View transaction history for a specific account and card
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

    //Banlance inquiry via stored procedure (logs INQUIRY transaction)
    getBalance: function(data, callback) {
        const { account_id, card_id } = data;

       const sql = `CALL sp_get_account_balance(?, ?)`;

        db.query(sql, [account_id, card_id], function(err, results) {
           if (err) return callback({ error: 'DB_ERROR', message: err.sqlMessage });
            const row = results[0][0];

           if (!row) {
            return callback({ error: 'NOT_FOUND', message: 'No data returned from procedure' });
        }
    
            callback(null, {
                owner: `${row.user_name} ${row.user_lastname}`,
                account_number: row.account_number,
                account_type: row.account_type,
                balance: parseFloat(row.balance),
                available_funds: parseFloat(row.available_funds),
                credit_limit: row.credit_limit ? parseFloat(row.credit_limit) : 0
            });
        });
    },
    //Banlance inquiry without logging
    getBalanceNoLog: function(data, callback) {
        this.getBalance(data, callback);
},
//Transfer money via stored procedure  
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