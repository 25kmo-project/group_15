const db = require ( '../routes/database' );

const accountAccess = {

// hae kaikki rivit
    getAll: function(callback){
        return db.query('SELECT * FROM account_access', callback);
    },

// hae yhden kortin kaikki oikeudet
    getByCardId: function(card_id, callback){
        return db.query("SELECT * FROM account_access WHERE card_id=?", [card_id], callback);
    },

// hae yhden tilin kaikki oikeudet
    getByAccountId: function(account_id, callback){
        return db.query("SELECT * FROM account_access WHERE account_id=?", [account_id], callback);
    },

// hae yhden kortin ja tilin oikeudet
    getOne: function(card_id, account_id, callback){
        return db.query("SELECT * FROM account_access WHERE card_id=? AND account_id=?", [card_id, account_id], callback);
    },

// lisää oikeus
    add: function(row, callback){
        return db.query("INSERT INTO account_access (card_id, account_id, access_type) VALUES (?,?,?)",
        [row.card_id, row.account_id, row.access_type], callback);
    },

// päivitä access_type
    update: function(card_id, account_id, access_type, callback){
        return db.query("UPDATE account_access SET access_type=? WHERE card_id=? AND account_id=?",
        [access_type, card_id, account_id], callback);
    },

// poista oikeus
    delete: function(card_id, account_id, callback){
        return db.query("DELETE FROM account_access WHERE card_id=? AND account_id=?", [card_id, account_id], callback);
    }
};
module.exports = accountAccess;