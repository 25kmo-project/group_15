const db = require('../routes/database'); // varmista, että tietokantayhteys on oikein

const account = {

    //Get all accounts
    getAll: function (callback) {
        return db.query("SELECT * FROM account", callback);
    },

    //Get single account by id
    getOne: function (account_id, callback) {
        return db.query("SELECT * FROM account WHERE account_id = ?", [account_id], callback);
    },

    //Add new account
    add: function (account, callback) {
        return db.query(
            `INSERT INTO account (account_type, balance, credit_limit, status)
             VALUES (?, ?, ?, ?)`,
            [
                account.account_type,
                account.balance,
                account.credit_limit,
                account.status
            ],
            callback
        );
    },

    //Update account by id
    update: function (account, account_id, callback) {
        return db.query(
            `UPDATE account
             SET account_type=?, balance=?, credit_limit=?, status=?
             WHERE account_id=?`,
            [
                account.account_type,
                account.balance,
                account.credit_limit,
                account.status,
                account_id
            ],
            callback
        );
    },

    //Delete account by ID
    delete: function (account_id, callback) {
        return db.query(
            "DELETE FROM account WHERE account_id=?",
            [account_id],
            callback
        );
    }

};

module.exports = account;