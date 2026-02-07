const db = require('../routes/database'); // varmista, että tietokantayhteys on oikein

const account = {

    // Haetaan kaikki tilit tietokannasta
    getAll: function (callback) {
        return db.query("SELECT * FROM account", callback);
    },

    // Hae yksi tili tietokannasta ID:n perusteella
    getOne: function (account_id, callback) {
        return db.query("SELECT * FROM account WHERE account_id = ?", [account_id], callback);
    },

    // Luo uusi tili tietokantaan
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

    // Päivitä tili tietokannassa ID:n perusteella
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

    // Poista tili tietokannasta ID:n perusteella
    delete: function (account_id, callback) {
        return db.query(
            "DELETE FROM account WHERE account_id=?",
            [account_id],
            callback
        );
    }

};

module.exports = account;