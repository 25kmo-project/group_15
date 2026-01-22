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
    }
}

module.exports = transaction;