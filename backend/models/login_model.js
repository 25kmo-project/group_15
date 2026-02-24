const db = require('../routes/database');

const login={
    
    //Get card_id, pin_code, status, failed_pin_attempts, account_id, account_type, user_id
    getCardIdPincode: function(card_id, callback) {
        const query = `SELECT c.card_id, c.pin_code, c.status, c.failed_pin_attempts,aa.account_id, a.account_type, a.user_id 
                       FROM card c JOIN account_access aa ON c.card_id = aa.card_id JOIN account a on aa.account_id = a.account_id
                       WHERE c.card_id = ?`;
        return db.query(query,[card_id], callback);
    },

    //increment failed pin attempts counter
    updateFailedPinAttempts: function(card_id, callback) {
        return db.query( `UPDATE card SET failed_pin_attempts = failed_pin_attempts + 1 WHERE card_id = ?`,[card_id], callback);
    
    },

    //Reset failed pin attempts on successful login
    resetFailedPinAttempts: function(card_id, callback) {
        return db.query(`UPDATE card SET failed_pin_attempts = 0 WHERE card_id = ?`,[card_id],callback);

    },

    //Close the card
    closeCard: function(card_id, callback) {
         return db.query("UPDATE card SET status = 'CLOSED' WHERE card_id = ?",[card_id],callback);
    }
};

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports=login;