//haetaan tietokantayhteys database.js-tiedostosta
const db = require('../routes/database');

//luodaan objektin nimeltä user
const login={
    
    //sadaan yhden user ja pincode
    getCardIdPincode: function(card_id, callback) {
        const query = `SELECT card_id, pin_code, status FROM card WHERE card_id = ?`;
        return db.query(query,[card_id], callback);
    }
};

//tämä antaa mahdollisuus käyttää tätä tiedostoa muissa tiedostoissa
module.exports=login;