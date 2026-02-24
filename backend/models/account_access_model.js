const db = require ( '../routes/database' );

const accountAccess = {

//Get all rows
    getAll: function(callback){
        return db.query('SELECT * FROM account_access', callback);
    },

//Get all access rights for a single card
    getByCardId: function(card_id, callback){
        return db.query("SELECT * FROM account_access WHERE card_id=?", [card_id], callback);
    },

//Get all access rights for a single account
    getByAccountId: function(account_id, callback){
        return db.query("SELECT * FROM account_access WHERE account_id=?", [account_id], callback);
    },

//Get access rights for a specific card and account
    getOne: function(card_id, account_id, callback){
        return db.query("SELECT * FROM account_access WHERE card_id=? AND account_id=?", [card_id, account_id], callback);
    },

//Add new access right
    add: function(row, callback){
        return db.query("INSERT INTO account_access (card_id, account_id, access_type) VALUES (?,?,?)",
        [row.card_id, row.account_id, row.access_type], callback);
    },

//Update access_type
    update: function(card_id, account_id, access_type, callback){
        return db.query("UPDATE account_access SET access_type=? WHERE card_id=? AND account_id=?",
        [access_type, card_id, account_id], callback);
    },

//Delete access right
    delete: function(card_id, account_id, callback){
        return db.query("DELETE FROM account_access WHERE card_id=? AND account_id=?", [card_id, account_id], callback);
    }
};
module.exports = accountAccess;