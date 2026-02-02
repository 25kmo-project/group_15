//haetaan tietokantayhteys database.js-tiedostosta
const db = require('../routes/database');

//luodaan objektin nimeltä user
const login={
    
    //sadaan yhden user ja pincode
    getCardIdPincode: function(card_id, callback) {
        const query = `SELECT card_id, pin_code, status, failed_pin_attempts FROM card WHERE card_id = ?`;
        return db.query(query,[card_id], callback);
    },

    //päivitetään käyttäjän epäonnistuneet pin-koodin yritykset
    updateFailedPinAttempts: function(card_id, callback) {
        return db.query( `UPDATE card SET failed_pin_attempts = failed_pin_attempts + 1 WHERE card_id = ?`,[card_id], callback);
    
    },

    // nollaa yritykset onnistuneessa loginissa
    resetFailedPinAttempts: function(card_id, callback) {
        return db.query(`UPDATE card SET failed_pin_attempts = 0 WHERE card_id = ?`,[card_id],callback);

    },

    // sulje kortti
    closeCard: function(card_id, callback) {
         return db.query("UPDATE card SET status = 'CLOSED' WHERE card_id = ?",[card_id],callback);
    }
};

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports=login;